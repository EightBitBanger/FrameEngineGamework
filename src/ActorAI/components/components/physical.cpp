#include <GameEngineFramework/ActorAI/ActorSystem.h>

PhysicalAttributes::PhysicalAttributes() : 
    mAge(0),
    mAgeAdult(0),
    mAgeSenior(0),
    mSpeed(0),
    mSpeedYouth(0),
    mSpeedMul(0),
    mSnapSpeed(0.07f),
    mYouthScale(1),
    mAdultScale(1),
    mSexualOrientation(false)
{}

void PhysicalAttributes::SetAge(unsigned long int newAge) {
    mAge = newAge;
}

unsigned long int PhysicalAttributes::GetAge(void) {
    return mAge;
}

void PhysicalAttributes::SetSeniorAge(float oldAge) {
    mAgeSenior = oldAge;
}

float PhysicalAttributes::GetSeniorAge(void) {
    return mAgeSenior;
}

void PhysicalAttributes::SetAdultAge(float age) {
    mAgeAdult = age;
}

float PhysicalAttributes::GetAdultAge(void) {
    return mAgeAdult;
}

void PhysicalAttributes::SetSpeed(float newSpeed) {
    mSpeed = newSpeed;
}

float PhysicalAttributes::GetSpeed(void) {
    return mSpeed;
}

void PhysicalAttributes::SetSpeedYouth(float newSpeed) {
    mSpeedYouth = newSpeed;
}

float PhysicalAttributes::GetSpeedYouth(void) {
    return mSpeedYouth;
}

void PhysicalAttributes::SetSpeedMultiplier(float newSpeedMul) {
    mSpeedMul = newSpeedMul;
}

float PhysicalAttributes::GetSpeedMultiplier(void) {
    return mSpeedMul;
}

void PhysicalAttributes::SetYouthScale(float scale) {
    mYouthScale = scale;
}

float PhysicalAttributes::GetYouthScale(void) {
    return mYouthScale;
}

void PhysicalAttributes::SetAdultScale(float scale) {
    mAdultScale = scale;
}

float PhysicalAttributes::GetAdultScale(void) {
    return mAdultScale;
}

void PhysicalAttributes::SetSexualOrientation(bool orientation) {
    mSexualOrientation = orientation;
}

bool PhysicalAttributes::GetSexualOrientation(void) {
    return mSexualOrientation;
}
