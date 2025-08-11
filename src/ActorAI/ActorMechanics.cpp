#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


void ActorSystem::HandleMovementMechanics(Actor* actor) {
    glm::vec3 forward(0);
    glm::vec3 position(0);
    
    float speedScaler = 0.0f;
    float ageScaler = 1.0f;
    if (actor->physical.mAge < actor->physical.mAgeSenior) 
        ageScaler = (float)actor->physical.mAge / (float)actor->physical.mAgeSenior;
    speedScaler = Math.Lerp(actor->physical.mSpeedYouth, actor->physical.mSpeed, ageScaler) * 0.01f;
    
    switch (actor->state.mode) {
        
        case ActorState::Mode::Sleeping:
        case ActorState::Mode::Idle:
            actor->state.mIsWalking = false;
            actor->state.mIsRunning = false;
            
            actor->navigation.mVelocity *= glm::vec3(0, 1, 0);
            break;
            
        case ActorState::Mode::MoveRandom:
            position = CalculateRandomLocalPoint(actor);
            actor->state.mIsWalking = false;
            actor->state.mIsRunning = false;
            
            actor->navigation.mDistanceToTarget = 9999.0f;
            actor->navigation.mTargetLook  = position;
            actor->state.mIsFacing = true;
            
            actor->state.mode = ActorState::Mode::MoveTo;
            
        case ActorState::Mode::MoveTo:
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler;
            actor->state.mIsWalking = true;
            actor->state.mode = ActorState::Mode::WalkTo;
            break;
            
        case ActorState::Mode::WalkTo:
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler;
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = false;
            break;
            
        case ActorState::Mode::RunTo:
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler;
            forward *= actor->physical.mSpeedMul;
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = true;
            break;
    }
    
    actor->navigation.mVelocity = forward;
    return;
}


void ActorSystem::HandleCooldownCounters(Actor* actor) {
    if (actor->counters.mMovementCoolDownCounter > 0) 
        actor->counters.mMovementCoolDownCounter--;
    
    if (actor->counters.mAttackCoolDownCounter > 0) {
        actor->counters.mAttackCoolDownCounter--;
        if (actor->counters.mAttackCoolDownCounter == 0) {
            actor->state.mode = ActorState::Mode::RunTo;
        }
    }
    
    if (actor->counters.mObservationCoolDownCounter > 0) {
        actor->counters.mObservationCoolDownCounter--;
        if (actor->counters.mObservationCoolDownCounter == 0) {
            actor->state.current = ActorState::State::None;
            actor->state.mode = ActorState::Mode::Idle;
        }
    }
    
    if (actor->counters.mBreedingCoolDownCounter > 0) {
        actor->counters.mBreedingCoolDownCounter--;
        if (actor->counters.mBreedingCoolDownCounter == 0) {
            actor->state.current = ActorState::State::None;
            actor->state.mode = ActorState::Mode::Idle;
        }
    }
}

glm::vec3 ActorSystem::CalculateForwardVelocity(Actor* actor) {
    glm::vec3 forward(0);
    forward.x = cos( glm::radians( -(actor->navigation.mRotation.y - 90.0f) ) );
    forward.z = sin( glm::radians( -(actor->navigation.mRotation.y - 90.0f) ) );
    // TODO: Should actors fly???
    //forward.y = tan( glm::radians( -(actor->mRotation.x - 90) ) );
    return forward;
}


glm::vec3 ActorSystem::CalculateRandomLocalPoint(Actor* actor) {
    glm::vec3 position(0);
    position.x = actor->navigation.mPosition.x;
    position.z = actor->navigation.mPosition.z;
    
    position.x += Random.Range(0.0f, actor->behavior.GetDistanceToWalk()) - Random.Range(0.0f, actor->behavior.GetDistanceToWalk());
    position.z += Random.Range(0.0f, actor->behavior.GetDistanceToWalk()) - Random.Range(0.0f, actor->behavior.GetDistanceToWalk());
    
    actor->navigation.mTargetPoint.x = position.x;
    actor->navigation.mTargetPoint.y = 0.0f;
    actor->navigation.mTargetPoint.z = position.z;
    return position;
}

