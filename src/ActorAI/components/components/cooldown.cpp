#include <GameEngineFramework/ActorAI/ActorSystem.h>

CooldownCounters::CooldownCounters() : 
    mObservationCoolDownCounter(0),
    mSocialCoolDownCounter(0),
    mMovementCoolDownCounter(0),
    mAttackCoolDownCounter(0),
    mBreedingCoolDownCounter(0)
{}

void CooldownCounters::Reset() {
    mObservationCoolDownCounter = 0;
    mSocialCoolDownCounter      = 0;
    mAttackCoolDownCounter      = 0;
    mMovementCoolDownCounter    = 0;
    mBreedingCoolDownCounter    = 0;
}

void CooldownCounters::SetCoolDownObservation(unsigned int counter) {
    mObservationCoolDownCounter = counter;
}
unsigned int CooldownCounters::GetCoolDownObservation(void) {
    return mObservationCoolDownCounter;
}

void CooldownCounters::SetCoolDownSocial(unsigned int counter) {
    mSocialCoolDownCounter = counter;
}
unsigned int CooldownCounters::GetCoolDownSocial(void) {
    return mSocialCoolDownCounter;
}

void CooldownCounters::SetCoolDownMovement(unsigned int counter) {
    mMovementCoolDownCounter = counter;
}
unsigned int CooldownCounters::GetCoolDownMovement(void) {
    return mMovementCoolDownCounter;
}

void CooldownCounters::SetCoolDownAttack(unsigned int counter) {
    mAttackCoolDownCounter = counter;
}
unsigned int CooldownCounters::GetCoolDownAttack(void) {
    return mAttackCoolDownCounter;
}

void CooldownCounters::SetCoolDownBreeding(unsigned int counter) {
    mBreedingCoolDownCounter = counter;
}
unsigned int CooldownCounters::GetCoolDownBreeding(void) {
    return mBreedingCoolDownCounter;
}
