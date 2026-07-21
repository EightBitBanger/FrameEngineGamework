#include <GameEngineFramework/ActorAI/ActorSystem.h>

Behavior::Behavior() : 
    mIsPredator(false),
    mIsPrey(false),
    mDistanceToFocus(0),
    mDistanceToWalk(0),
    mDistanceToAttack(0),
	mDistanceToFlee(0),
    mDistanceToInflict(0),
    
    mCooldownAttack(0),
    mCooldownObserve(0),
    mCooldownSocial(0),
    mCooldownMove(0),
    mCooldownBreed(0),
    
    mHeightPreferenceMin(0),
	mHeightPreferenceMax(0)
{}

void Behavior::SetDistanceToFocus(float distance) {
    mDistanceToFocus = distance;
}

float Behavior::GetDistanceToFocus(void) {
    return mDistanceToFocus;
}

void Behavior::SetDistanceToWalk(float distance) {
    mDistanceToWalk = distance;
}

float Behavior::GetDistanceToWalk(void) {
    return mDistanceToWalk;
}

void Behavior::SetDistanceToAttack(float distance) {
    mDistanceToAttack = distance;
}

float Behavior::GetDistanceToAttack(void) {
    return mDistanceToAttack;
}

void Behavior::SetDistanceToFlee(float distance) {
    mDistanceToFlee = distance;
}

float Behavior::GetDistanceToFlee(void) {
    return mDistanceToFlee;
}

void Behavior::SetDistanceToInflict(float distance) {
    mDistanceToInflict = distance;
}

float Behavior::GetDistanceToInflict(void) {
    return mDistanceToInflict;
}

void Behavior::SetCooldownAttack(unsigned int ticks) {
    mCooldownAttack = ticks;
}

void Behavior::SetCooldownObserve(unsigned int ticks) {
    mCooldownObserve = ticks;
}

void Behavior::SetCooldownSocial(unsigned int ticks) {
    mCooldownSocial = ticks;
}

void Behavior::SetCooldownMove(unsigned int ticks) {
    mCooldownMove = ticks;
}

void Behavior::SetCooldownBreed(unsigned int ticks) {
    mCooldownBreed = ticks;
}

unsigned int Behavior::GetCooldownAttack(void) {
    return mCooldownAttack;
}

unsigned int Behavior::GetCooldownObserve(void) {
    return mCooldownObserve;
}

unsigned int Behavior::GetCooldownSocial(void) {
    return mCooldownSocial;
}

unsigned int Behavior::GetCooldownMove(void) {
    return mCooldownMove;
}

unsigned int Behavior::GetCooldownBreed(void) {
    return mCooldownBreed;
}

void Behavior::SetHeightPreferenceMin(float height) {
    mHeightPreferenceMin = height;
}

float Behavior::GetHeightPreferenceMin(void) {
    return mHeightPreferenceMin;
}

void Behavior::SetHeightPreferenceMax(float height) {
    mHeightPreferenceMax = height;

}

float Behavior::GetHeightPreferenceMax(void) {
    return mHeightPreferenceMax;
}

void Behavior::SetPredatorState(bool state) {
    mIsPredator = state;
}

bool Behavior::GetPredatorState(void) {
    return mIsPredator;
}

void Behavior::SetPreyState(bool state) {
    mIsPrey = state;
}

bool Behavior::GetPreyState(void) {
    return mIsPrey;
}
