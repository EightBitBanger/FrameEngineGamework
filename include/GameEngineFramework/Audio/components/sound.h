#ifndef AUDIO_SOUND_COMPONENT
#define AUDIO_SOUND_COMPONENT

#include <SDL3/SDL.h>

#include <GameEngineFramework/Audio/components/samplebuffer.h>

#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>
#include <glm/glm.hpp>

#include <vector>

#include <thread>
#include <mutex>

class ENGINE_API Sound {
    
public:
    
    Sound();
    ~Sound();
    
    void Play(void);
    void Stop(void);
    
    /// Set the sample volume.
    void SetVolume(float volume);
    
    /// Set the sample pitch.
    void SetPitch(float pitch);
    
    /// Check if the sample is currently playing.
    bool IsSamplePlaying(void);
    
    /// Load an audio sample for playback.
    bool LoadSample(AudioSample* samplePtr);
    
    
private:
    
    // Sample buffer
    AudioSample* mSample;
    
    // Audio output stream
    SDL_AudioStream* mStream;
    
    bool mIsPlaying = false;
    
};

#endif
