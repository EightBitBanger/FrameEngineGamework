#ifndef AUDIO_SOUND_COMPONENT
#define AUDIO_SOUND_COMPONENT

#include <GameEngineFramework/Audio/components/samplebuffer.h>
#include <glm/glm.hpp>
#include <mutex>


class ENGINE_API Sound {
    
    friend class AudioSystem;
    
public:
    
    /// Is this sound currently active.
    bool isActive;
    
    /// Sample buffer of audio.
    AudioSample* sample;
    
    /// Should this sample be treated as a spacial 3D sound.
    bool isSample3D;
    
    Sound();
    
    /// Set the position of the sound in the world.
    void SetPosition(glm::vec3 position);
    
    /// Set the minimum falloff limit
    void SetMinimumFalloff(float falloff);
    
    /// Set the maximum falloff limit
    void SetMaximumFalloff(float falloff);
    
    /// Set the multiplier for the rate of fall off over distance.
    void SetFalloffMultiplier(float multiplier);
    
private:
    
    // Spacial world position
    glm::vec3 mPosition;
    
    // Distance range for 3D spacial audio
    float mRangeMin;
    float mRangeMax;
    
    // Rate of audio drop off
    float mFalloff;
    
    std::mutex mux;
    
};

#endif
