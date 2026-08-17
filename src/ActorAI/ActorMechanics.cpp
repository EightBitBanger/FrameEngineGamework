#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

void ActorSystem::HandleMovementMechanics(Actor* actor) {
    ScopeProfile profile("AI-mechanics");
    
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
            
            // Apply group noise offsets
            CalculateTargetOffsetting(actor, 1.1f);
            actor->state.mIsFacing = true;
            
            forward = CalculateForwardVelocity(actor);
            forward *= speedScaler * actor->physical.mSpeedMul;
            
            if (actor->navigation.mTargetActor != nullptr) 
                HandleInflictDamage(actor, actor->navigation.mTargetActor);
            break;
            
        case ActorState::Mode::MoveFlee: {
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = true;
            
            bool hasHome = false;
            glm::vec3 homePos(0.0f);
            
            // Direct zero-allocation vector retrieval
            std::unordered_map<std::string, std::vector<MemoryTrigger>>::iterator homeIt = actor->memories.mMemoryTriggers.find("home");
            if (homeIt != actor->memories.mMemoryTriggers.end() && !homeIt->second.empty()) {
                homePos = homeIt->second[0].vector;
                hasHome = true;
            }
            
            if (hasHome) {
                float walkRadius = actor->behavior.GetDistanceToWalk();
                float distToTarget = glm::distance(
                    glm::vec3(actor->navigation.mPosition.x, 0.0f, actor->navigation.mPosition.z),
                    glm::vec3(actor->navigation.mTargetPoint.x, 0.0f, actor->navigation.mTargetPoint.z)
                );
                float distTargetToHome = glm::distance(
                    glm::vec3(actor->navigation.mTargetPoint.x, 0.0f, actor->navigation.mTargetPoint.z),
                    glm::vec3(homePos.x, 0.0f, homePos.z)
                );
                
                // Pick a new point inside the home radius if outside or arrived
                if (distTargetToHome > walkRadius || distToTarget < actor->behavior.GetDistanceToInflict()) {
                    actor->navigation.mTargetPoint.x = homePos.x + Random.Range(-walkRadius, walkRadius);
                    actor->navigation.mTargetPoint.y = homePos.y;
                    actor->navigation.mTargetPoint.z = homePos.z + Random.Range(-walkRadius, walkRadius);
                }
                
                actor->navigation.mTargetLook = actor->navigation.mTargetPoint;
                actor->state.mIsFacing = true;
            } else {
                // Fallback: Run directly away from target
                actor->navigation.mTargetPoint.x = targetPosition.x;
                actor->navigation.mTargetPoint.z = targetPosition.z;
                actor->navigation.mTargetLook = targetPosition;
                actor->state.mIsFacing = false;
            }
            
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
            
        case ActorState::Mode::MoveHunting:
            break;
            
        case ActorState::Mode::MoveBreed: {
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
            float breedDistance = actor->behavior.GetDistanceToInflict();
            forward *= ApplyApproachSlowdown(actor, targetPosition, speedScaler, breedDistance);
            
            if (actor->navigation.mTargetActor != nullptr) {
                if (!HandleBreedWith(actor, actor->navigation.mTargetActor)) {
                    float dist = glm::distance(actor->navigation.mPosition, actor->navigation.mTargetActor->navigation.mPosition);
                    // Relaxed distance threshold ensures actors stop walking if breeding fails or completes
                    if (dist <= breedDistance * 1.25f) {
                        actor->counters.mBreedingCoolDownCounter = actor->behavior.mCooldownBreed;
                        actor->state.mode = ActorState::Mode::Idle;
                        actor->state.mIsWalking = false;
                        forward = glm::vec3(0.0f);
                    }
                }
            }
            break;
        }
            
        case ActorState::Mode::MoveSocialize: {
            if (actor->navigation.mTargetActor == nullptr || 
                !actor->navigation.mTargetActor->isActive || 
                actor->navigation.mTargetActor->isGarbage) {
                actor->navigation.mTargetActor = nullptr;
                actor->state.mode = ActorState::Mode::Idle;
                break;
            }
            
            actor->state.mIsWalking = true;
            actor->state.mIsRunning = false;
            
            // Direct movement target prevents mutual orbiting/chasing loops
            actor->navigation.mTargetPoint.x = targetPosition.x;
            actor->navigation.mTargetPoint.z = targetPosition.z;
            actor->navigation.mTargetLook = targetPosition;
            actor->state.mIsFacing = true;
            
            forward = CalculateForwardVelocity(actor);
            float socialDistance = actor->behavior.GetDistanceToInflict() * 1.5f;
            forward *= ApplyApproachSlowdown(actor, targetPosition, speedScaler, socialDistance);
            
            if (!HandleSocializeWith(actor, actor->navigation.mTargetActor)) {
                float dist = glm::distance(actor->navigation.mPosition, actor->navigation.mTargetActor->navigation.mPosition);
                if (dist <= socialDistance) {
                    actor->counters.mSocialCoolDownCounter = actor->behavior.mCooldownSocial;
                    actor->state.mode = ActorState::Mode::Idle;
                    actor->state.mIsWalking = false;
                    forward = glm::vec3(0.0f);
                }
            }
            break;
        }
            
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
