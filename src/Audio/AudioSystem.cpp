#include <GameEngineFramework/Audio/AudioSystem.h>

extern Logger Log;
extern NumberGeneration Random;

// Audio thread
bool isAudioDeviceActive = false;
bool isAudioThreadActive = true;
void AudioThreadMain(void);


void AudioSystem::Initiate(void) {
    
    // Launch the audio thread
    audioThread = new std::thread(AudioThreadMain);
    Log.Write(" >> Starting thread audio");
    
    isAudioDeviceActive = true;
    
    SDL_Init(SDL_INIT_AUDIO);
    
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

bool AudioSystem::CheckIsAudioEndpointActive(void) {
    return mIsDeviceActive;
}


//
// Audio thread entry point
//

void AudioThreadMain(void) {
    
    while (isAudioThreadActive) {
        
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(2) );
    Log.Write(" >> Shutting down on thread audio");
    
    return;
}
