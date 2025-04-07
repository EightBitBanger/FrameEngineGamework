#include <GameEngineFramework/Audio/components/sound.h>
#include <SDL3/SDL.h>

extern bool isAudioDeviceActive;


Sound::Sound() : 
    isPlaying(false),
    sample(nullptr),
    playbackCursor(0)
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
    
    return;
}

void Sound::SetPitch(float pitch) {
    
    return;
}

