#ifndef AUDIO_SOUND_COMPONENT
#define AUDIO_SOUND_COMPONENT

#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>
#include <glm/glm.hpp>

#include "../../../vendor/AL/al.h"
#include "../../../vendor/AL/al.h"

#include <iostream>
#include <vector>
#include <cmath>

#include <thread>
#include <mutex>
#include <chrono>

class ENGINE_API Sound {
    
public:
    
    Sound();
    ~Sound();
    
    void Play(void);
    void Stop(void);
    
    /// Set the sample volume.
    void SetVolume(float volume);
    
    /// Check if the sample is currently playing.
    bool IsSamplePlaying(void);
    
    bool Load(std::vector<ALshort>& samples, int sampleRate);
    
    
private:
    
    ALuint mBuffer;
    
    ALuint mSource;
    
    bool mIsBufferConstructed;
    
};

#endif
