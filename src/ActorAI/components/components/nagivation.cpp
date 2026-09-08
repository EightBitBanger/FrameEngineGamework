#include <GameEngineFramework/ActorAI/ActorSystem.h>

NavigationSystem::NavigationSystem() : 
    mVelocity(glm::vec3(0)),
    mPosition(glm::vec3(0)),
    mRotation(glm::vec3(0)),
    mFacing(glm::vec3(0)),
    mLookAt(glm::vec3(0)),
    mRotateTo(glm::vec3(0)),
    mTargetPoint(glm::vec3(0)),
    mTargetLook(glm::vec3(0)),
    mDistanceToTarget(0),
    mTargetActor(nullptr)
{}

void NavigationSystem::Reset() {
    mVelocity     = glm::vec3(0);
    mPosition     = glm::vec3(0);
    mRotation     = glm::vec3(0);
    mFacing       = glm::vec3(0);
    mLookAt       = glm::vec3(0);
    
    mRotateTo          = glm::vec3(0);
    mTargetPoint       = glm::vec3(0);
    mTargetLook        = glm::vec3(0);
    mDistanceToTarget  = 0;
    mTargetActor       = nullptr;
    mQueryStatic.clear();
    mQueryPoints.clear();
}

void NavigationSystem::SetPosition(glm::vec3 position) {
    mPosition = position;
    mTargetPoint = position;
}

glm::vec3 NavigationSystem::GetPosition(void) {
    return mPosition;
}

void NavigationSystem::SetTargetPoint(glm::vec3 position) {
    mTargetPoint = position;
    return;
}

glm::vec3 NavigationSystem::GetTargetPoint(void) {
    return mTargetPoint;
}

Actor* NavigationSystem::GetTargetActor(void) {
    return mTargetActor;
}

void NavigationSystem::SetVelocity(const glm::vec3& velocity) {
    mVelocity = velocity;
}

glm::vec3 NavigationSystem::GetVelocity(void) {   
    return mVelocity;
}

void NavigationSystem::SetTargetActor(Actor* actorPtr) {
    mTargetActor = actorPtr;
}

float NavigationSystem::GetDistanceToTarget(void) {
    return mDistanceToTarget;
}
