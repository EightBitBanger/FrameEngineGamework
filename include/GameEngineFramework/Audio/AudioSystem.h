#ifndef AUDIO_RENDER_SYSTEM
#define AUDIO_RENDER_SYSTEM

#include <SDL3/SDL.h>

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Audio/components/sound.h>
#include <GameEngineFramework/Audio/components/samplebuffer.h>
#include <GameEngineFramework/Audio/SamplePresets.h>

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
    
    /// World listening position in 3D space.
    glm::vec3 listenerPosition;
    
    AudioSystem() : 
        mStream(nullptr)
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
    
    /// Sends in a sound to be played by the mixer.
    bool Play(Sound* soundPtr);
    
    
    void Initiate(void);
    void Shutdown(void);
    
    AudioPreset Presets;
    
    // Caution used be the audio thread
    std::mutex mux;
    
    SDL_AudioStream* mStream;
    
    void MixActiveSounds(std::vector<int32_t>& buffer);
    
private:
    
    // List of currently playing sounds
    std::vector<Sound*> mActiveSounds;
    
    PoolAllocator<Sound> mSounds;
    PoolAllocator<AudioSample> mSamples;
    
    std::thread* audioThread;
    
};

#endif
