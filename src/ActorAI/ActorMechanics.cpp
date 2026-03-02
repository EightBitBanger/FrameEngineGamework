#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


void ActorSystem::HandleMovementMechanics(Actor* actor) {
    glm::vec3 targetPosition(0.0f);
    if (actor->navigation.mTargetActor != nullptr) 
        targetPosition = actor->navigation.mTargetActor->navigation.mPosition;
    
    glm::vec3 position(0);
    glm::vec3 forward(0);
    
    float speedScaler = 1.0f;
    float ageScaler = 1.0f;
    
    if (actor->physical.mAge < actor->physical.mAgeAdult) 
        ageScaler = (float)actor->physical.mAge / (float)actor->physical.mAgeAdult;
    ageScaler = glm::clamp(ageScaler, 0.0f, 1.0f);
    speedScaler = Math.Lerp(actor->physical.mSpeedYouth, actor->physical.mSpeed, ageScaler) * 0.01;
    
    switch (actor->state.mode) {
        
        case ActorState::Mode::Sleeping:
        case ActorState::Mode::Idle:
            actor->state.mIsWalking = false;
            actor->state.mIsRunning = false;
            
            actor->navigation.mVelocity *= glm::vec3(0, 1, 0);
            break;
            
        case ActorState::Mode::MoveAttack:
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = true;
            
            actor->navigation.mTargetPoint.x = targetPosition.x;
            actor->navigation.mTargetPoint.z = targetPosition.z;
            actor->navigation.mTargetLook = targetPosition;
            actor->state.mIsFacing = true;
            
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler * actor->physical.mSpeedMul;
            
            if (actor->navigation.mTargetActor != nullptr) 
                HandleInflictDamage(actor, actor->navigation.mTargetActor);
            break;
            
        case ActorState::Mode::MoveFlee: {
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = true;
            
            actor->navigation.mTargetPoint.x = targetPosition.x;
            actor->navigation.mTargetPoint.z = targetPosition.z;
            actor->navigation.mTargetLook = targetPosition;
            actor->state.mIsFacing = false;
            
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler * actor->physical.mSpeedMul;
            
            //HandleTargetDistance(actor);
            break;
        }
            
        case ActorState::Mode::MoveRandom:
            position = CalculateRandomLocalPoint(actor);
            actor->state.mIsWalking = false;
            actor->state.mIsRunning = false;
            
            actor->navigation.mTargetPoint.x = position.x;
            actor->navigation.mTargetPoint.z = position.z;
            actor->navigation.mTargetLook  = position;
            actor->state.mIsFacing = true;
            
            position.y = 0.0f;
            targetPosition.y = 0.0f;
            
            actor->navigation.mDistanceToTarget = glm::distance(targetPosition, position);
            
            actor->state.mode = ActorState::Mode::MoveToRandom;
            
        case ActorState::Mode::MoveToRandom:
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler;
            actor->state.mIsWalking = true;
            
            HandleTargetDistance(actor);
            break;
            
        case ActorState::Mode::MoveTo:
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler;
            actor->state.mIsWalking = true;
            
            HandleTargetDistance(actor);
            break;
            
        case ActorState::Mode::WalkTo:
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler;
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = false;
            
            HandleTargetDistance(actor);
            break;
            
        case ActorState::Mode::RunTo:
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler * actor->physical.mSpeedMul;
            forward *= actor->physical.mSpeedMul;
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = true;
            
            HandleTargetDistance(actor);
            break;
    }
    
    actor->navigation.mVelocity = forward;
}

