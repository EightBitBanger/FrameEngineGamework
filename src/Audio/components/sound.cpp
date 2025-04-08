#include <GameEngineFramework/Audio/components/sound.h>
#include <SDL3/SDL.h>

extern bool isAudioDeviceActive;


Sound::Sound() : 
    isPlaying(false),
    playbackCursor(0),
    sample(nullptr),
    mVolume(1.0f)
{
}

void Sound::Play(void) {
    
    return;
}

void Sound::Stop(void) {
    isPlaying = false;
    
    
    return;
}

void Sound::SetVolume(float volume) {
    mVolume = volume;
    return;
}

