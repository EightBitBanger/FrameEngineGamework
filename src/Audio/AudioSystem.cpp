#include <GameEngineFramework/Audio/AudioSystem.h>

extern Logger Log;
extern NumberGeneration Random;


// Audio thread
bool isAudioDeviceActive = false;
bool isAudioThreadActive = true;
void AudioThreadMain(void);


void AudioSystem::Initiate(void) {
    
    // Launch the audio thread
    audioThread = new std::thread( AudioThreadMain );
    
    Log.Write( " >> Starting thread audio" );
    
    mDevice = alcGetContextsDevice(mContext);
    
    if (!mDevice) 
        mDevice = alcOpenDevice(nullptr);
    
    if (mDevice) {
        
        mContext = alcGetCurrentContext();
        
        if (!mContext) {
            
            mContext = alcCreateContext(mDevice, nullptr);
            
            alcMakeContextCurrent(mContext);
            
            isAudioDeviceActive = true;
        }
        
    } else {
        
        Log.Write("!! Unable to open audio device.");
        Log.WriteLn();
    }
    
    return;
}

void AudioSystem::Shutdown(void) {
    
    alcMakeContextCurrent(nullptr);
    
    if (mContext != 0) 
        alcDestroyContext(mContext);
    
    if (mDevice != 0) 
        alcCloseDevice(mDevice);
    
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


