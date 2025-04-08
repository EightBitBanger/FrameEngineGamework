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
    
    friend class AudioSystem;
    
public:
    
    /// Is this sound currently playing.
    bool isPlaying;
    
    /// Current point in the audio playback.
    unsigned int playbackCursor;
    
    Sound();
    
    void Play(void);
    void Stop(void);
    
    /// Set the playback volume for the sample.
    void SetVolume(float volume);
    
    /// Sample buffer of audio.
    AudioSample* sample;
    
private:
    
    float mVolume;
    
    std::mutex mux;
    
};

#endif
