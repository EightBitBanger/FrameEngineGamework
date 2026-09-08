#include <GameEngineFramework/ActorAI/ActorSystem.h>

State::State() : 
    mode(ActorState::Mode::Idle),
    mIsWalking(false),
    mIsRunning(false),
    mIsFacing(false)
{}

void State::Reset() {
    mode          = ActorState::Mode::Idle;
    mIsWalking    = false;
    mIsRunning    = false;
    mIsFacing     = true;
}

bool State::GetStateWalking(void) {
    return mIsWalking;
}

bool State::GetStateRunning(void) {
    return mIsRunning;
}

bool State::GetStateFacing(void) {
    return mIsFacing;
}
