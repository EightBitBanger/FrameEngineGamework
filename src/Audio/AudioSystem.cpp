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
    
    mDevice = alcOpenDevice(nullptr);
    
    if (mDevice == nullptr) {
        
        Log.Write("!! Unable to open audio device.");
        Log.WriteLn();
        
        Log.Write( LogErrors(mDevice) );
        Log.WriteLn();
        
        return;
    }
    
    mContext = alcCreateContext(mDevice, nullptr);
    
    if (mContext == nullptr) {
        
        Log.Write("!! Unable to create audio context.");
        Log.WriteLn();
        
        Log.Write( LogErrors(mDevice) );
        Log.WriteLn();
        
        return;
    }
    
    alcMakeContextCurrent(mContext);
    isAudioDeviceActive = true;
    
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

std::string AudioSystem::LogErrors(ALCdevice* devicePtr) {
    
    ALCenum errorCode = alcGetError(devicePtr);
    
    switch (errorCode) {
        
        case ALC_NO_ERROR: return "No error";
        case ALC_INVALID_DEVICE: return "Invalid device";
        case ALC_INVALID_CONTEXT: return "Invalid context";
        case ALC_INVALID_ENUM: return "Invalid enumerator";
        case ALC_INVALID_VALUE: return "Invalid value";
        case ALC_OUT_OF_MEMORY: return "Out of memory";
        
    }
    
    return "Unknown ALC error";
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


