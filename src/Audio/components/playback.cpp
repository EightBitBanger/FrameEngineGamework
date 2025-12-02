#include <GameEngineFramework/Audio/components/playback.h>
#include <glm/glm.hpp>

Playback::Playback() : 
    doRepeat(false),
    isGarbage(false),
    mIsPlaying(true),
    mCursor(0.0f),
    mVolume(1.0f),
    mSample(nullptr),
    mSound(nullptr)
{
}


void Playback::SetVolume(float volume) {
    std::lock_guard<std::mutex> lock(mux);
    mVolume = volume;
}

void Playback::Seek(unsigned int position) {
    std::lock_guard<std::mutex> lock(mux);
    mCursor = position;
}

bool Playback::CheckIsPlaying(void) {
    std::lock_guard<std::mutex> lock(mux);
    return mIsPlaying;
}

void Playback::Replay(void) {
    std::lock_guard<std::mutex> lock(mux);
    mIsPlaying = true;
    mCursor = 0;
}

void Playback::Stop(void) {
    std::lock_guard<std::mutex> lock(mux);
    mIsPlaying = false;
}
