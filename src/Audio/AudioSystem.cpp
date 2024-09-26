#include <GameEngineFramework/Audio/AudioSystem.h>


extern Logger Log;
extern NumberGeneration Random;


// Audio thread
bool isAudioThreadActive = true;
void AudioThreadMain(void);


void AudioSystem::Initiate(void) {
    
    ALCdevice* mDevice = alcOpenDevice(nullptr);
    
    if (mDevice > 0) {
        
        ALCcontext* mContext = alcCreateContext(mDevice, nullptr);
        
        alcMakeContextCurrent(mContext);
        
    } else {
        
        std::cout << "Unable to open audio device." << std::endl;
    }
    
    
    // Launch the audio thread
    audioThread = new std::thread( AudioThreadMain );
    
    Log.Write( " >> Starting thread audio" );
    
    return;
}

void AudioSystem::Shutdown(void) {
    
    //alcMakeContextCurrent(nullptr);
    
    //if (mContext != 0) 
    //    alcDestroyContext(mContext);
    
    //if (mDevice != 0) 
    //    alcCloseDevice(mDevice);
    
    isAudioThreadActive = false;
    
    audioThread->join();
    
    return;
}


/// Create a new sound object and return its pointer.
Sound* AudioSystem::CreateSound(void) {
    
    Sound* newSoundPtr = mSounds.Create();
    
    return newSoundPtr;
}

/// Destroy an old sound object.
bool AudioSystem::DestroySound(Sound* soundPtr) {
    
    return mSounds.Destroy(soundPtr);
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


