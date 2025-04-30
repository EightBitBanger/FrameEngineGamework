#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


#define DECISION_CHANCE_TO_WALK                  1000
#define DECISION_CHANCE_TO_CHANGE_DIRECTION      1000
#define DECISION_CHANCE_TO_FOCUS_NEARBY          10000
#define DECISION_CHANCE_TO_STOP_MOVING           10000
#define COOLDOWN_OBSERVATION_MAX                 100
#define COOLDOWN_MOVEMENT_MAX                    100

#define DISTANCE_MINIMUM_TARGET_REACHED          1.5f
#define DISTANCE_MINIMUM_TARGET_BREEDING         1.0f


void ActorSystem::UpdateActorMechanics(Actor* actor) {
    
    HandleWalkingMechanics(actor);
    HandleTargettingMechanics(actor);
    HandleBreedingMechanics(actor);
    
    return;
}

void ActorSystem::HandleWalkingMechanics(Actor* actor) {
    //if (mMovementCoolDownCounter != 0) 
    //    return;
    
    // Stop moving but keep falling
    if (!actor->state.mIsWalking) {
        actor->navigation.mVelocity *= glm::vec3(0, 1, 0);
        return;
    }
    
    // Calculate forward velocity
    glm::vec3 forward;
    forward.x = cos( glm::radians( -(actor->navigation.mRotation.y - 90.0f) ) );
    // TODO: Should actors fly???
    //forward.y = tan( glm::radians( -(actor->mRotation.x - 90) ) );
    forward.z = sin( glm::radians( -(actor->navigation.mRotation.y - 90) ) );
    
    float actorSpeed = actor->physical.mSpeed;
    
    glm::vec3 actorVelocity = forward * (actorSpeed * 0.1f) * 0.1f;
    
    // Check running speed multiplier
    if (actor->state.mIsRunning) 
        actorVelocity *= actor->physical.mSpeedMul;
    
    actor->navigation.mVelocity.x = actorVelocity.x;
    actor->navigation.mVelocity.z = actorVelocity.z;
    
    // Check walking state
    if (actor->state.current == NeuralState::flee) {
        if (!actor->state.mIsWalking) {
            actor->state.mIsWalking = true;
            
            // Select a random destination if a target was not set
            if (actor->navigation.mTargetActor == nullptr && 
                actor->navigation.mTargetBreeding == nullptr) {
                
                actor->navigation.mTargetPoint.x = (Random.Range(0.0f, actor->behavior.mDistanceToWalk) - 
                                                    Random.Range(0.0f, actor->behavior.mDistanceToWalk)) + 
                                                    actor->navigation.mPosition.x;
                
                actor->navigation.mTargetPoint.z = (Random.Range(0.0f, actor->behavior.mDistanceToWalk) - 
                                                    Random.Range(0.0f, actor->behavior.mDistanceToWalk)) + 
                                                    actor->navigation.mPosition.z;
            }
            
            // The physics system will update this position to the actors current height
            actor->navigation.mTargetPoint.y = -100000.0f;
        }
    }
    
    return;
}


void ActorSystem::HandleTargettingMechanics(Actor* actor) {
    
    // Check arrived at target point
    if (glm::distance(actor->navigation.mTargetPoint, actor->navigation.mPosition) < DISTANCE_MINIMUM_TARGET_REACHED) {
        actor->counters.mMovementCoolDownCounter = 0;
        actor->state.mIsWalking = false;
        actor->navigation.mTargetPoint = actor->navigation.mPosition;
    }
    
    if (actor->navigation.mTargetActor != nullptr) {
        
        // Attack state
        if (actor->state.current == NeuralState::attack) {
            actor->state.mIsWalking = true;
            
            // Track the target
            actor->navigation.mTargetPoint.x = actor->navigation.mTargetActor->navigation.mPosition.x;
            actor->navigation.mTargetPoint.z = actor->navigation.mTargetActor->navigation.mPosition.z;
            
            // Face away from the target
            actor->state.mIsFacing = true;
            
        }
        
        // Flee state
        if (actor->state.current == NeuralState::flee) {
            actor->state.mIsWalking = true;
            
            // Track the target
            actor->navigation.mTargetPoint.x = actor->navigation.mTargetActor->navigation.mPosition.x;
            actor->navigation.mTargetPoint.z = actor->navigation.mTargetActor->navigation.mPosition.z;
            
            // Face away from the target
            actor->state.mIsFacing = false;
            
        }
        
        // Check arrived at target actor
        if (glm::distance(actor->navigation.mTargetActor->navigation.mPosition, actor->navigation.mPosition) < DISTANCE_MINIMUM_TARGET_REACHED) {
            actor->counters.mMovementCoolDownCounter = 0;
            actor->state.mIsWalking = false;
            actor->navigation.mTargetPoint = actor->navigation.mPosition;
        }
        
    }
    
    
    /*
    // Check target reached
    if (glm::distance(actor->navigation.mTargetPoint, actor->navigation.mPosition) < DISTANCE_MINIMUM_TARGET_REACHED) {
        actor->counters.mMovementCoolDownCounter = 0;
        actor->state.mIsWalking = false;
        actor->navigation.mTargetPoint = actor->navigation.mPosition;
        
        actor->state.current = NeuralState::idle;
    }
    
    // Check arrived at target actor
    if (actor->navigation.mTargetActor != nullptr) {
        
        if (glm::distance( actor->navigation.mTargetActor->navigation.mPosition, actor->navigation.mPosition ) < 1.5f) {
            actor->state.mIsWalking = false;
            actor->navigation.mTargetPoint = actor->navigation.mPosition;
            
            // Null the target if we are the target
            if (actor->navigation.mTargetActor->navigation.mTargetActor == actor) 
                actor->navigation.mTargetActor->navigation.mTargetActor = nullptr;
            
            actor->navigation.mTargetActor = nullptr;
            
            
            
        }
    }
    
    */
    return;
}


void ActorSystem::HandleBreedingMechanics(Actor* actor) {
    /*
    // Count down breeding counter
    if (actor->counters.mBreedingCoolDownCounter > 0) 
        actor->counters.mBreedingCoolDownCounter--;
    
    if (actor->navigation.mTargetBreeding == nullptr) 
        return;
    
    // Check breeding is withing the prefered range
    if ((actor->navigation.mTargetPoint.y > actor->behavior.mHeightPreferenceMax) | 
        (actor->navigation.mTargetPoint.y < actor->behavior.mHeightPreferenceMin)) 
        return;
    
    actor->navigation.mTargetPoint = actor->navigation.mTargetBreeding->navigation.mPosition;
    // The physics system will update this position, until then we
    // ignore it. When it updates we will know the current height of the actor
    actor->navigation.mTargetPoint.y = -100000.0f;
    
    // Walk the male towards the female target actor
    if (actor->physical.GetSexualOrientation() == true) 
        actor->state.mIsWalking = glm::distance(actor->navigation.mPosition, actor->navigation.mTargetBreeding->navigation.mPosition) > DISTANCE_MINIMUM_TARGET_BREEDING;
    
    // Check arrived at breeding partner
    if (actor->navigation.mTargetBreeding != nullptr) {
        
        if (glm::distance( actor->navigation.mTargetBreeding->navigation.mPosition, actor->navigation.mPosition ) < 1.0f) {
            actor->state.mIsWalking = false;
            actor->navigation.mTargetPoint = actor->navigation.mPosition;
            
            // Null the target if we arrived
            if (actor->navigation.mTargetBreeding->navigation.mTargetBreeding == actor->navigation.mTargetBreeding) 
                actor->navigation.mTargetBreeding->navigation.mTargetBreeding = nullptr;
            
            actor->navigation.mTargetBreeding = nullptr;
        }
    }
    */
    return;
}

void ActorSystem::HandleTargetRandomPoint(Actor* actor) {
    
    if (actor->counters.mObservationCoolDownCounter > 0) 
        actor->counters.mObservationCoolDownCounter--;
    
    /*
    float randA = Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    float randB = Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    randA -= Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    randB -= Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    
    actor->navigation.mTargetPoint.x = randA + actor->navigation.mPosition.x;
    // The physics system will update this position to the actors current height
    actor->navigation.mTargetPoint.y = -100000.0f;
    actor->navigation.mTargetPoint.z = randB + actor->navigation.mPosition.z;
    */
    return;
}

void ActorSystem::HandleMovementMechanics(Actor* actor) {
    /*
    // Height preference max below water suggests
    // the actor is an aquatic actor
    bool isAquatic = actor->behavior.mHeightPreferenceMax < mWorldWaterLevel;
    
    if (actor->navigation.mTargetPoint.y > actor->behavior.mHeightPreferenceMax && 
        actor->navigation.mTargetPoint.y < actor->behavior.mHeightPreferenceMin) 
        return;
    
    if (!isAquatic) {
        
        // Terrestrial
        if (actor->navigation.mTargetPoint.y > mWorldWaterLevel) {
            actor->state.mIsWalking = true;
            actor->counters.mMovementCoolDownCounter = 0;
        }
        
        // Check under water, navigate to higher ground
        if (actor->navigation.mPosition.y < mWorldWaterLevel && 
            actor->navigation.mTargetPoint.y > actor->navigation.mPosition.y) {
            actor->state.mIsWalking = true;
            actor->counters.mMovementCoolDownCounter = 0;
        }
        
    } else {
        
        // Aquatic
        if (actor->navigation.mTargetPoint.y < mWorldWaterLevel) {
            actor->state.mIsWalking = true;
            actor->counters.mMovementCoolDownCounter = 0;
        }
        
        // Check above water, navigate to lower ground
        if (actor->navigation.mPosition.y > mWorldWaterLevel && actor->navigation.mTargetPoint.y < actor->navigation.mPosition.y) {
            actor->state.mIsWalking = true;
            actor->counters.mMovementCoolDownCounter = 0;
        }
    }
    */
    return;
}

