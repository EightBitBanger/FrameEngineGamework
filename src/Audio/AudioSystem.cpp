#include <GameEngineFramework/Audio/AudioSystem.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

extern Logger Log;
extern NumberGeneration Random;

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
    spec.channels = 1;
    
    mStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
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

bool AudioSystem::Play(Sound* soundPtr) {
    if (soundPtr->isPlaying) 
        return false;
    mActiveSounds.push_back(soundPtr);
    soundPtr->isPlaying = true;
    return true;
}

//
// Audio thread entry point
//

extern AudioSystem Audio;

void AudioThreadMain(void) {
    
    while (isAudioThreadActive) {
        
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(5) );
        
        // Check if the buffer is low
        int available = SDL_GetAudioStreamAvailable(Audio.mStream);
        if (available > 8192) 
            continue;
        
        std::lock_guard<std::mutex> lock(Audio.mux);
        
        // Temporary mix buffer
        std::vector<int32_t> buffer;
        buffer.resize(512);
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



void AudioSystem::MixActiveSounds(std::vector<int32_t>& buffer) {
    
    for (unsigned int i=0; i < mActiveSounds.size(); i++) {
        Sound* soundPtr = mActiveSounds[i];
        std::lock_guard<std::mutex> lock(soundPtr->mux);
        
        // Check sound has stopped playing
        if (!soundPtr->isPlaying) {
            mActiveSounds.erase(mActiveSounds.begin() + i);
            continue;
        }
        
        // Get the audio sample
        std::vector<int32_t>& sample = soundPtr->sample->sampleBuffer;
        
        // Mix the next section of samples
        for (unsigned int s = 0; s < buffer.size(); s++) {
            
            // Check sound has ended
            if (soundPtr->playbackCursor >= sample.size()) {
                soundPtr->isPlaying = false;
                soundPtr->playbackCursor = 0;
                mActiveSounds.erase(mActiveSounds.begin() + i);
                break;
            }
            
            // Get source sample
            int32_t currentSample = buffer[s];
            int32_t sourceSample = static_cast<int32_t>(sample[soundPtr->playbackCursor]);
            
            // Sample volume
            sourceSample *= soundPtr->mVolume;
            
            // Mix the audio
            int32_t mixed = (currentSample + sourceSample) / 2;
            
            soundPtr->playbackCursor++;
            
            buffer[s] = mixed;
        }
        
        continue;
    }
    return;
}
