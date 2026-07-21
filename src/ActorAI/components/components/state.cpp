#include <GameEngineFramework/ActorAI/ActorSystem.h>

State::State() : 
    mode(ActorState::Mode::Idle),
    mIsWalking(false),
    mIsRunning(false),
    mIsFacing(false)
{}
