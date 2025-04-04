#ifndef AUDIO_RENDER_SYSTEM
#define AUDIO_RENDER_SYSTEM

#include <SDL3/SDL.h>

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Audio/components/sound.h>
#include <GameEngineFramework/Audio/components/samplebuffer.h>

#include <GameEngineFramework/MemoryAllocation/poolallocator.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <cmath>

#include <thread>
#include <mutex>


class ENGINE_API AudioSystem {
    
public:
    
    glm::vec3 listenerPosition;
    
    AudioSystem() : 
        mIsDeviceActive(false)
    {
    }
    
    /// Create a new sound object and return its pointer.
    Sound* CreateSound(void);
    
    /// Destroy an old sound object.
    bool DestroySound(Sound* soundPtr);
    
    /// Create a new audio sample object and return its pointer.
    AudioSample* CreateAudioSample(void);
    
    /// Destroy an old audio sample object.
    bool DestroyAudioSample(AudioSample* samplePtr);
    
    void Initiate(void);
    void Shutdown(void);
    
    /// Check if the audio device was initialized.
    bool CheckIsAudioEndpointActive(void);
    
    /// Log errors to the log file.
    std::string LogErrors(void);
    
private:
    
    bool mIsDeviceActive;
    
    PoolAllocator<Sound> mSounds;
    PoolAllocator<AudioSample> mSamples;
    
    std::thread* audioThread;
    
};

#endif
