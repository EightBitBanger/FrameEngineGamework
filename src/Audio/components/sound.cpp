#include <GameEngineFramework/Audio/components/sound.h>
#include <SDL3/SDL.h>

extern bool isAudioDeviceActive;


Sound::Sound() : 
    mSample(nullptr),
    mStream(nullptr),
    mIsPlaying(false)
{
}

Sound::~Sound() {
    
}


void Sound::Play(void) {
    
    SDL_AudioSpec spec;
    spec.freq = mSample->sample_rate;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;
    
    mStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    
    // Start playback
    SDL_PutAudioStreamData(mStream, mSample->sampleBuffer.data(), mSample->sampleBuffer.size() * sizeof(int16_t));
    SDL_FlushAudioStream(mStream);
    SDL_ResumeAudioStreamDevice(mStream);
    
    mIsPlaying = true;
    return;
}

void Sound::Stop(void) {
    mIsPlaying = false;
    if (mStream == nullptr) 
        return;
    SDL_DestroyAudioStream(mStream);
    mStream = nullptr;
    return;
}

void Sound::SetVolume(float volume) {
    
    return;
}

void Sound::SetPitch(float pitch) {
    
    return;
}

bool Sound::IsSamplePlaying(void) {
    return mIsPlaying;
}

bool Sound::LoadSample(AudioSample* samplePtr) {
    mSample = samplePtr;
    return true;
}

