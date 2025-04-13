#include <GameEngineFramework/Audio/components/sound.h>

extern bool isAudioDeviceActive;


Sound::Sound() : 
    isActive(true),
    sample(nullptr),
    isSample3D(false),
    mPosition(glm::vec3(0)),
    mRangeMin(0.0f),
    mRangeMax(20.0f),
    mFalloff(6.0f)
{
}

void Sound::SetPosition(glm::vec3 position) {
    std::lock_guard<std::mutex> lock(mux);
    mPosition = position;
    return;
}

void Sound::SetMinimumFalloff(float falloff) {
    std::lock_guard<std::mutex> lock(mux);
    mRangeMin = falloff;
    return;
}

void Sound::SetMaximumFalloff(float falloff) {
    std::lock_guard<std::mutex> lock(mux);
    mRangeMax = falloff;
    return;
}

void Sound::SetFalloffMultiplier(float multiplier) {
    std::lock_guard<std::mutex> lock(mux);
    mFalloff = multiplier;
    return;
}

