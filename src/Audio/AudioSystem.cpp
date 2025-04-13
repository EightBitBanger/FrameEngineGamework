#include <GameEngineFramework/Audio/AudioSystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

extern Logger Log;
extern NumberGeneration Random;
AudioSystem::AudioSystem() : 
    listenerPosition(glm::vec3(100)),
    mStream(NULL),
    mMasterVolume(1.0f)
{
}

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
    
    return;
}


void AudioSystem::Shutdown(void) {
    
    isAudioThreadActive = false;
    
    audioThread->join();
    
    return;
}

Sound* AudioSystem::CreateSound(void) {
    Sound* newSoundPtr = mSounds.Create();
    return newSoundPtr;
}

bool AudioSystem::DestroySound(Sound* soundPtr) {
    return mSounds.Destroy(soundPtr);
}

AudioSample* AudioSystem::CreateAudioSample(void) {
    AudioSample* newSample = mSamples.Create();
    return newSample;
}

bool AudioSystem::DestroyAudioSample(AudioSample* samplePtr) {
    return mSamples.Destroy(samplePtr);
}

Playback* AudioSystem::Play(Sound* soundPtr) {
    Playback* playback = mPlaybacks.Create();
    playback->mSample = soundPtr->sample;
    playback->mSound = soundPtr;
    mActiveSounds.push_back(playback);
    return playback;
}

void AudioSystem::SetVolume(float volume) {
    mMasterVolume = volume;
    return;
}

//
// Audio thread entry point
//

extern AudioSystem Audio;

void AudioThreadMain(void) {
    
    // Temporary mix buffer
    std::vector<int32_t> buffer;
    buffer.resize(512);
    
    while (isAudioThreadActive) {
        
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(10) );
        
        // Check if the buffer is low
        int available = SDL_GetAudioStreamAvailable(Audio.mStream);
        if (available > 8192) 
            continue;
        
        std::lock_guard<std::mutex> lock(Audio.mux);
        
        std::fill(buffer.begin(), buffer.end(), 0);
        
        // Mix the currently playing sounds
        Audio.MixActiveSounds(buffer);
        
        // Send in the next section of mixed audio
        SDL_PutAudioStreamData(Audio.mStream, buffer.data(), buffer.size() * sizeof(int32_t));
        SDL_FlushAudioStream(Audio.mStream);
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(2) );
    Log.Write(" >> Shutting down on thread audio");
    
    return;
}


float normalize(float value, float min, float max) {
    return 1.0f - (value - min) / (max - min);
}


void AudioSystem::MixActiveSounds(std::vector<int32_t>& buffer) {
    
    for (unsigned int i = 0; i < mActiveSounds.size(); i++) {
        Playback* playback = mActiveSounds[i];
        Sound* sound = playback->mSound;
        
        std::lock_guard<std::mutex> lock(playback->mux);
        
        if (!sound->isActive) 
            continue;
        
        if (!playback->mIsPlaying) {
            mActiveSounds.erase(mActiveSounds.begin() + i);
            i--;
            continue;
        }
        
        std::vector<int32_t>& sample = playback->mSample->sampleBuffer;
        
        double attenuation = 1.0f;
        double pan = 0.5f;
        
        // Check 3D audio sample
        if (sound->isSample3D) {
            
            // Attenuation
            double distance = glm::distance(listenerPosition, sound->mPosition);
            
            if (distance <= sound->mRangeMin) 
                distance = sound->mRangeMin;
            
            // Check outside sound attenuation range
            else if (distance >= sound->mRangeMax) {
                attenuation = 0.0f;
                continue;
            } else {
                attenuation = normalize(distance, sound->mRangeMin, sound->mRangeMax);
            }
            
            // Listener orientation panning
            glm::vec3 sourceDirection = glm::normalize(sound->mPosition - listenerPosition);
            
            glm::vec2 forwardDirection = glm::normalize(glm::vec2(listenerDirection.x, listenerDirection.z));
            glm::vec2 directionToSound = glm::normalize(glm::vec2(sourceDirection.x, sourceDirection.z));
            
            // Calculate side
            // Negative = left | Positive = right
            float side = forwardDirection.x * directionToSound.y - 
                         forwardDirection.y * directionToSound.x;
            
            // Calculate pan angle
            float angle = glm::dot(forwardDirection, directionToSound);
            angle = glm::clamp(angle, -1.0f, 1.0f); // Give it the ol` clamp, just in case
            float theta = std::acos(angle); // Angle in radians
            
            // 0.0 = left, 1.0 = right
            pan = 0.5f + 0.5f * side * (theta / glm::pi<float>());
        }
        
        
        double volume = playback->mVolume * attenuation;
        
        for (unsigned int s = 0; s < buffer.size(); s += 2) {
            
            if (playback->mCursor >= sample.size()) {
                
                if (playback->doRepeat) {
                    playback->mCursor = 0;
                } else {
                    playback->mIsPlaying = false;
                    
                    // Check to destroy sound
                    if (playback->isGarbage) {
                        mActiveSounds.erase(mActiveSounds.begin() + i);
                        mPlaybacks.Destroy(playback);
                        i--;
                    }
                    break;
                }
            }
            
            // Mix the sample
            double sourceSample = glm::clamp(static_cast<double>(sample[playback->mCursor]), -32767.0d, 32767.0d);
            
            double left  = static_cast<double>(sourceSample * volume * (1.0d - pan));
            double right = static_cast<double>(sourceSample * volume * pan);
            
            double mixedLeft  = static_cast<double>(buffer[s])     + left  / 2.0d;
            double mixedRight = static_cast<double>(buffer[s + 1]) + right / 2.0d;
            
            buffer[s]     = static_cast<int32_t>(mixedLeft  * mMasterVolume);
            buffer[s + 1] = static_cast<int32_t>(mixedRight * mMasterVolume);
            
            playback->mCursor++;
        }
    }
}
