#include <GameEngineFramework/Audio/components/sound.h>

Sound::Sound() : 
    isActive(true),
    sample(nullptr),
    isSample3D(false),
    mPosition(glm::vec3(0)),
    mRangeMin(0.0f),
    mRangeMax(20.0f),
    mFalloff(6.0f) {}

void Sound::SetPosition(glm::vec3 position) {
    mPosition = position;
}

void Sound::SetMinimumFalloff(float falloff) {
    mRangeMin = falloff;
}

void Sound::SetMaximumFalloff(float falloff) {
    mRangeMax = falloff;
}

void Sound::SetFalloffMultiplier(float multiplier) {
    mFalloff = multiplier;
}
