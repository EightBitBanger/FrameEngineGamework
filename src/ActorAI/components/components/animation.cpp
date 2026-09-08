#include <GameEngineFramework/ActorAI/ActorSystem.h>

AnimationState::AnimationState() : 
    mWalkRate(8.0f),
    mWalkTime(0.0f)
{}

void AnimationState::SetAnimationCycleRate(float rate) {
    mWalkRate = rate;
}

float AnimationState::GetAnimationCycleRate(void) {
    return mWalkRate;
}
