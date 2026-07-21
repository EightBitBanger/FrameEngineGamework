#ifndef AUDIO_PLAYBACK_COMPONENT
#define AUDIO_PLAYBACK_COMPONENT

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Audio/components/sound.h>
#include <GameEngineFramework/Audio/components/samplebuffer.h>

#include <mutex>

class ENGINE_API Playback {
public:
    
    Playback();
    
    /// Is this a repeating sound
    bool doRepeat;
    
    /// Destroy when sound finishes playing
    bool isGarbage;
    
    /// Set the playback volume for the sample.
    void SetVolume(float volume);
    
    /// Set the cursor seek position in the sample.
    void Seek(unsigned int position);
    
    /// Check if the sample is still playing.
    bool CheckIsPlaying(void);
    
    /// Replay the sound sample.
    void Replay(void);
    
    /// End the sound sample playback.
    void Stop(void);
    
private:
    
    // Playing / paused state
    bool mIsPlaying;
    
    // Position in the sample
    unsigned int mCursor;
    
    // Volume multiplier
    float mVolume;
    
    // Sample buffer of audio
    AudioSample* mSample;
    
    // Link to the sound that created this playback
    Sound* mSound;
    
    std::mutex mux;
    
    friend class AudioSystem;
};

#endif
