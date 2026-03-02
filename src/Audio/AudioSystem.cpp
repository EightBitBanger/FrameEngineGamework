#include <GameEngineFramework/Audio/AudioSystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <thread>
#include <chrono>

extern Logger           Log;
extern NumberGeneration Random;

static inline float ClampFloat(float v, float a, float b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

static inline double ClampDouble(double v, double a, double b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

static inline glm::vec2 SafeNormalize2(const glm::vec2& v, const glm::vec2& fallback) {
    float len2 = glm::dot(v, v);
    if (len2 > 1.0e-12f) {
        return v / std::sqrt(len2);
    }
    float flen2 = glm::dot(fallback, fallback);
    if (flen2 > 1.0e-12f) {
        return fallback / std::sqrt(flen2);
    }
    return glm::vec2(0.0f, 1.0f);
}

static inline glm::vec3 SafeNormalize3(const glm::vec3& v, const glm::vec3& fallback) {
    float len2 = glm::dot(v, v);
    if (len2 > 1.0e-12f) {
        return v / std::sqrt(len2);
    }
    float flen2 = glm::dot(fallback, fallback);
    if (flen2 > 1.0e-12f) {
        return fallback / std::sqrt(flen2);
    }
    return glm::vec3(0.0f, 0.0f, 1.0f);
}

static inline int32_t ClampS32(double v) {
    if (v >  2147483647.0) return  2147483647;
    if (v < -2147483648.0) return -2147483648LL;
    return (int32_t)v;
}

AudioSystem::AudioSystem() :
    listenerPosition(glm::vec3(0.0f)),
    mMasterVolume(1.0f),
    mStream(NULL)
{}

// Audio thread
bool isAudioThreadActive = true;
void AudioThreadMain(void);

void AudioSystem::Initiate(void) {
    
    isAudioThreadActive = true;
    
    // Launch the audio thread
    audioThread = new std::thread(AudioThreadMain);
    Log.Write(" >> Starting thread audio");
    
    SDL_Init(SDL_INIT_AUDIO);
    
    // Initiate the audio stream
    SDL_AudioSpec spec = {};
    spec.freq = 44100;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 2;
    
    mStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (mStream == NULL) {
        Log.Write(" !! Error initiating audio system");
        return;
    }
    SDL_ResumeAudioStreamDevice(mStream);
}

void AudioSystem::Shutdown(void) {
    isAudioThreadActive = false;
    if (audioThread) {
        audioThread->join();
        delete audioThread;
        audioThread = NULL;
    }
}

Sound* AudioSystem::CreateSound(void) {
    Sound* newSoundPtr = mSounds.Create();
    return newSoundPtr;
}

bool AudioSystem::DestroySound(Sound* soundPtr) {
    std::lock_guard<std::mutex> lock(mux);
    return mSounds.Destroy(soundPtr);
}

AudioSample* AudioSystem::CreateAudioSample(void) {
    AudioSample* newSample = mSamples.Create();
    return newSample;
}

bool AudioSystem::DestroyAudioSample(AudioSample* samplePtr) {
    std::lock_guard<std::mutex> lock(mux);
    return mSamples.Destroy(samplePtr);
}

Playback* AudioSystem::Play(Sound* soundPtr) {
    std::lock_guard<std::mutex> lock(mux);
    Playback* playback = mPlaybacks.Create();
    playback->mSample  = soundPtr->sample;
    playback->mSound   = soundPtr;
    mActiveSounds.push_back(playback);
    return playback;
}

SDL_AudioStream* AudioSystem::GetOutputStream(void) {
    return mStream;
}

unsigned int AudioSystem::GetNumberOfSounds(void) {
    return mSounds.Size();
}

void AudioSystem::SetVolume(float volume) {
    mMasterVolume = volume;
}

static inline float Normalize01(float value, float minv, float maxv) {
    // 1.0 at min, 0.0 at max
    if (maxv <= minv) return 0.0f;
    float t = (value - minv) / (maxv - minv);
    return 1.0f - ClampFloat(t, 0.0f, 1.0f);
}

void AudioSystem::MixActiveSounds(std::vector<int32_t>& buffer) {
    for (unsigned int i = 0; i < mActiveSounds.size(); i++) {
        Playback* playback = mActiveSounds[i];
        Sound*    sound    = playback->mSound;
        
        if (!sound->isActive)
            continue;
        
        if (!playback->mIsPlaying) {
            mActiveSounds.erase(mActiveSounds.begin() + i);
            i--;
            
            if (playback->isGarbage) {
                mPlaybacks.Destroy(playback);
            }
            continue;
        }
        
        std::lock_guard<std::mutex> lockSound(playback->mux);
        
        std::vector<int32_t>& sample = playback->mSample->sampleBuffer;
        
        double attenuation = 1.0;
        double pan         = 0.5;
        
        // Check 3D audio sample
        if (sound->isSample3D) {
            double distance = (double)glm::distance(listenerPosition, sound->mPosition);
            
            if (distance <= (double)sound->mRangeMin)
                distance = (double)sound->mRangeMin;
            
            if (distance >= (double)sound->mRangeMax) 
                continue;
            
            attenuation = (double)Normalize01((float)distance, (float)sound->mRangeMin, (float)sound->mRangeMax);
            
            glm::vec3 toSound3   = sound->mPosition - listenerPosition;
            glm::vec3 srcDir3    = SafeNormalize3(toSound3, glm::vec3(0.0f, 0.0f, 1.0f));
            
            glm::vec2 forwardXZ  = SafeNormalize2(glm::vec2(listenerDirection.x, listenerDirection.z),
                                                glm::vec2(0.0f, 1.0f));
            glm::vec2 soundXZ    = SafeNormalize2(glm::vec2(srcDir3.x, srcDir3.z),
                                                glm::vec2(0.0f, 1.0f));
            
            // 2D cross product: negative=left, positive=right
            float side = forwardXZ.x * soundXZ.y - forwardXZ.y * soundXZ.x;
            
            // Dot gives angle between [-1,1]
            float d = glm::dot(forwardXZ, soundXZ);
            d = glm::clamp(d, -1.0f, 1.0f);
            
            float theta = std::acos(d); // [0..pi]
            float t     = theta / glm::pi<float>(); // [0..1]
            
            // Centered at 0.5, push to sides proportional to angle and side
            pan = 0.5 + 0.5 * (double)side * (double)t;
            pan = ClampDouble(pan, 0.0, 1.0);
        }
        
        double volume = (double)playback->mVolume * attenuation;
        
        for (unsigned int s = 0; s < buffer.size(); s += 2) {
            if (playback->mCursor >= sample.size()) {
                
                if (playback->doRepeat) {
                    playback->mCursor = 0;
                } else {
                    playback->mIsPlaying = false;
                    break;
                }
            }
            
            double sourceSample = (double)sample[playback->mCursor];
            
            // Split to channels via constant-power-ish linear pan
            double left  = sourceSample * volume * (1.0 - pan);
            double right = sourceSample * volume * pan;
            
            // Mix in (keep headroom by dividing by 2, matching your original behavior)
            double mixedLeft  = (double)buffer[s]     + left  / 2.0;
            double mixedRight = (double)buffer[s + 1] + right / 2.0;
            
            // Apply master volume at write time; clamp to int32 range
            buffer[s]     = ClampS32(mixedLeft  * (double)mMasterVolume);
            buffer[s + 1] = ClampS32(mixedRight * (double)mMasterVolume);
            
            playback->mCursor++;
        }
    }
}

//
// Audio thread entry point
//

extern AudioSystem Audio;

void AudioThreadMain(void) {
    
    // Temporary mix buffer (int32 stereo interleaved).
    // 512 samples = 256 stereo frames.
    std::vector<int32_t> buffer;
    buffer.resize(512);
    
    // For SDL_AUDIO_S32 stereo, bytes per frame = 2 channels * 4 bytes = 8.
    // We gate feeding when the stream already has "enough" queued bytes.
    const int kHighWaterBytes = 32768;
    
    while (isAudioThreadActive) {
        std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(10));
        
        SDL_AudioStream* stream = Audio.GetOutputStream();
        if (stream == NULL) {
            continue;
        }
        
        int available = SDL_GetAudioStreamAvailable(stream);
        if (available > kHighWaterBytes)
            continue;
        
        std::lock_guard<std::mutex> lock(Audio.mux);
        
        std::fill(buffer.begin(), buffer.end(), 0);
        
        // Mix the currently playing sounds
        Audio.MixActiveSounds(buffer);
        
        // Send in the next section of mixed audio
        SDL_PutAudioStreamData(stream, buffer.data(), (int)(buffer.size() * sizeof(int32_t)));
    }
    
    std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(2));
    Log.Write(" >> Shutting down on thread audio");
}
