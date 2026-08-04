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
            
            if (actor->navigation.mTargetActor != nullptr) {
                glm::vec3 targetPos = actor->navigation.mTargetActor->navigation.mPosition;
                
                // 1. Generate a deterministic unique angle for this actor (using its memory address)
                uintptr_t actorId = reinterpret_cast<uintptr_t>(actor);
                float angleDeg = static_cast<float>(actorId % 360);
                float angleRad = glm::radians(angleDeg);
                
                // 2. Set the offset radius slightly inside their maximum attack range
                float attackRadius = actor->behavior.GetDistanceToAttack() * 0.5f;
                
                // 3. Offset the target point around the enemy
                actor->navigation.mTargetPoint.x = targetPos.x + std::cos(angleRad) * attackRadius;
                actor->navigation.mTargetPoint.y = targetPos.y;
                actor->navigation.mTargetPoint.z = targetPos.z + std::sin(angleRad) * attackRadius;
                
                // 4. Keep looking directly at the center of the target actor
                actor->navigation.mTargetLook = targetPos;
            }
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
            break;
        }
            
        case ActorState::Mode::MoveRandom:
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler;
            actor->state.mIsWalking = true;
            
            HandleTargetDistance(actor);
            break;
            
            // Special movement and interaction
            
        case ActorState::Mode::MoveHunting:
            
            
            break;
            
        case ActorState::Mode::MoveBreed:
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = false;
            
            actor->navigation.mTargetPoint.x = targetPosition.x;
            actor->navigation.mTargetPoint.z = targetPosition.z;
            actor->navigation.mTargetLook = targetPosition;
            actor->state.mIsFacing = true;
            
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler * actor->physical.mSpeedMul;
            
            if (actor->navigation.mTargetActor != nullptr) {
                if (!HandleBreedWith(actor, actor->navigation.mTargetActor)) {
                    if (glm::distance(actor->navigation.mPosition, actor->navigation.mTargetActor->navigation.mPosition) < actor->behavior.mDistanceToInflict) {
                        // Failure to reproduce
                        actor->counters.mBreedingCoolDownCounter = actor->behavior.mCooldownBreed;
                        actor->state.mode = ActorState::Mode::Idle;
                    }
                }
            }
            
            break;
            
        case ActorState::Mode::MoveSocialize:
            if (actor->navigation.mTargetActor == nullptr || 
                !actor->navigation.mTargetActor->isActive || 
                actor->navigation.mTargetActor->isGarbage) {
                actor->navigation.mTargetActor = nullptr;
                actor->state.mode = ActorState::Mode::Idle;
                break;
            }
            
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = false;
            
            actor->navigation.mTargetPoint.x = targetPosition.x;
            actor->navigation.mTargetPoint.z = targetPosition.z;
            actor->navigation.mTargetLook = targetPosition;
            actor->state.mIsFacing = true;
            
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler * actor->physical.mSpeedMul;
            
            if (!HandleSocializeWith(actor, actor->navigation.mTargetActor)) {
                if (glm::distance(actor->navigation.mPosition, actor->navigation.mTargetActor->navigation.mPosition) < actor->behavior.mDistanceToInflict) {
                    // Failure to socialize - set social cooldown counter
                    
                    actor->counters.mSocialCoolDownCounter = actor->behavior.mCooldownSocial;
                    actor->state.mode = ActorState::Mode::Idle;
                }
            }
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

