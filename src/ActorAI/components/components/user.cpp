#include <GameEngineFramework/ActorAI/ActorSystem.h>

UserVariables::UserVariables() : 
    mBitmask(0),
    mUserDataA(nullptr),
    mUserDataB(nullptr)
{}

void UserVariables::SetUserDataA(void* ptr) {
    mUserDataA = ptr;
}

void* UserVariables::GetUserDataA(void) {
    return mUserDataA;
}

void UserVariables::SetUserDataB(void* ptr) {
    mUserDataB = ptr;
}

void* UserVariables::GetUserDataB(void) {
    return mUserDataB;
}
