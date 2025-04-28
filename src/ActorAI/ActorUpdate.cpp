#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern Logger Log;
extern ActorSystem AI;
extern NumberGeneration Random;
extern MathCore Math;

extern bool isActorThreadActive;
extern bool doUpdate;

extern int actorCounter;
extern int tickCounter;

#define DECISION_CHANCE_TO_WALK                  1000
#define DECISION_CHANCE_TO_CHANGE_DIRECTION      1000
#define DECISION_CHANCE_TO_FOCUS_NEARBY          10000
#define DECISION_CHANCE_TO_STOP_MOVING           10000
#define COOLDOWN_OBSERVATION_MAX                 100
#define COOLDOWN_MOVEMENT_MAX                    100

#define DISTANCE_MINIMUM_TARGET_REACHED          1.5f
#define DISTANCE_MINIMUM_TARGET_BREEDING         1.0f


void ActorSystem::Update(void) {
    
    // Update animation states
    if (mAnimationTimer.Update()) {
        unsigned int numberOfActors = mActiveActors.size();
        for (unsigned int i = 0; i < numberOfActors; i++) {
            Actor* actor = mActiveActors[i];
            
            // Cycle the animation states
            UpdateAnimationState(actor);
            
            // Target tracking rotation
            UpdateTargetRotation(actor);
            
            // Express genetic phenotypes
            ExpressActorGenetics(actor);
            
        }
    }
    
    // Update AI states
    if (mMainTimer.Update()) {
        unsigned int numberOfActors = mActiveActors.size();
        if (numberOfActors == 0) 
            return;
        
        unsigned int numberOfActorsPerCycle = (numberOfActors > 10) ? (numberOfActors / 10) : 1;
        
        for (unsigned int i = 0; i < numberOfActorsPerCycle; i++) {
            
            if (actorCounter >= numberOfActors) {
                actorCounter = 0;
                doUpdate = false;
                break;
            }
            
            Actor* actor = mActiveActors[actorCounter++];
            if (!actor->state.mIsActive) 
                continue;
            
            
            actor->navigation.mDistance = glm::distance(mPlayerPosition, actor->navigation.mPosition);
            if (actor->navigation.mDistance > mActorUpdateDistance) 
                continue;
            
            // Update AI states
            UpdateActorState(actor);
            
            // Update genetics if requested
            UpdateActorGenetics(actor);
            
        }
        
    }
    
    return;
}

void ActorSystem::UpdateTargetRotation(Actor* actor) {
    
    // Face toward target point
    glm::vec3 position = actor->navigation.mPosition;
    
    float xx = position.x - actor->navigation.mTargetPoint.x;
    float zz = position.z - actor->navigation.mTargetPoint.z;
    
    actor->navigation.mRotateTo.y = glm::degrees( glm::atan(xx, zz) ) + 180;
    
    // Check to invert facing direction
    if (!actor->state.mIsFacing) {
        
        actor->navigation.mRotateTo.y += 180;
        
        if (actor->navigation.mRotateTo.y > 360) 
            actor->navigation.mRotateTo.y -= 360;
    }
    
    // Check actor target direction
    
    // Wrap euler rotations
    if (actor->navigation.mRotation.y < 90) 
        if (actor->navigation.mRotateTo.y > 270) 
            actor->navigation.mRotation.y += 360;
    
    if (actor->navigation.mRotation.y > 270) 
        if (actor->navigation.mRotateTo.y < 90) 
            actor->navigation.mRotation.y -= 360;
    
    // Rotate actor toward the focal point
    if (actor->navigation.mRotation != actor->navigation.mRotateTo) {
        
        glm::vec3 fadeFrom( actor->navigation.mRotation );
        glm::vec3 fadeTo( actor->navigation.mRotateTo );
        
        glm::vec3 fadeValue;
        fadeValue.x = Math.Lerp(fadeFrom.x, fadeTo.x, actor->physical.mSnapSpeed);
        fadeValue.y = Math.Lerp(fadeFrom.y, fadeTo.y, actor->physical.mSnapSpeed);
        fadeValue.z = Math.Lerp(fadeFrom.z, fadeTo.z, actor->physical.mSnapSpeed);
        
        actor->navigation.mRotation = fadeValue;
    }
    
    return;
}

void ActorSystem::UpdateActorState(Actor* actor) {
    
    actor->physical.mAge++;
    
    if (actor->idiosyncrasies.mNeuralNetwork.GetNumberOfLayers() != 0) 
        HandleNeuralNetwork(actor);
    
    // Check running speed multiplier
    if (actor->state.mIsRunning) {
        actor->navigation.mVelocity.x *= actor->physical.mSpeedMul;
        actor->navigation.mVelocity.z *= actor->physical.mSpeedMul;
    }
    
    // Check old age
    if (actor->physical.mAge > actor->physical.mAgeSenior) 
        if (actor->biological.health > 1.0f) 
            actor->biological.health -= 1.0f;
    
    // Count down breeding counter
    if (actor->counters.mBreedingCoolDownCounter != 0) 
        actor->counters.mBreedingCoolDownCounter--;
    
    // Check arrived at target point
    if (glm::distance( actor->navigation.mTargetPoint, actor->navigation.mPosition ) < 1.5f) {
        actor->state.mIsWalking = false;
        actor->navigation.mTargetPoint = actor->navigation.mPosition;
    }
    
    
    // Check focus on nearby actors
    /*
    if (mActiveActors.size() > 0) {
        Actor* actorTarget = mActiveActors[Random.Range(0, mActiveActors.size() - 1)];
        
        glm::vec3 posA = actor->navigation.mPosition;
        glm::vec3 posB = actorTarget->navigation.mPosition;
        
        if (glm::distance(posA, posB) < actor->behavior.mDistanceToFocus) 
            actor->navigation.SetTargetActor( actorTarget->navigation.GetTargetActor() );
    }
    */
    
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
    
    // Check walking state
    if (actor->state.mIsWalking) {
        glm::vec3 forward;
        
        // Calculate forward velocity
        forward.x = cos( glm::radians( -(actor->navigation.mRotation.y - 90.0f) ) );
        // TODO: Should actors fly???
        //forward.y = tan( glm::radians( -(actor->mRotation.x - 90) ) );
        forward.z = sin( glm::radians( -(actor->navigation.mRotation.y - 90) ) );
        
        float actorSpeed = actor->physical.mSpeed;
        
        glm::vec3 actorVelocity = forward * (actorSpeed * 0.1f) * 0.1f;
        
        actor->navigation.mVelocity.x = actorVelocity.x;
        actor->navigation.mVelocity.z = actorVelocity.z;
        
    } else {
        
        // Stop moving but keep falling
        actor->navigation.mVelocity *= glm::vec3(0, 1, 0);
    }
    
    return;
}

bool ActorSystem::HandleBreedingState(Actor* actor) {
    
    // Check breeding is withing the prefered range
    if ((actor->navigation.mTargetPoint.y > actor->behavior.mHeightPreferenceMax) | 
        (actor->navigation.mTargetPoint.y < actor->behavior.mHeightPreferenceMin)) 
        return false;
    
    if (actor->navigation.mTargetBreeding == nullptr) 
        return false;
    
    actor->navigation.mTargetPoint.x = actor->navigation.mTargetBreeding->navigation.mPosition.x;
    actor->navigation.mTargetPoint.y = -100000.0f;
    actor->navigation.mTargetPoint.z = actor->navigation.mTargetBreeding->navigation.mPosition.z;
    
    // Walk the male towards the female target actor
    if (actor->physical.GetSexualOrientation() == true) 
        actor->state.mIsWalking = glm::distance(actor->navigation.mPosition, actor->navigation.mTargetBreeding->navigation.mPosition) > DISTANCE_MINIMUM_TARGET_BREEDING;
    
    return true;
}

void ActorSystem::HandleWalkingChance(Actor* actor, bool isAquatic) {
    
    if (actor->navigation.mTargetPoint == actor->navigation.mPosition) 
        UpdateTargetPoint(actor);
    
    if (actor->state.mIsWalking) 
        return;
    
    // Target height will be updated by the physics system.
    // When this occurs, the height will be changed, triggering the
    // actor to begin walking
    if (actor->navigation.mTargetPoint.y != -100000.0f) {
        
        // Check outside prefered range
        if (actor->navigation.mTargetPoint.y > actor->behavior.mHeightPreferenceMax || 
            actor->navigation.mTargetPoint.y < actor->behavior.mHeightPreferenceMin) {
            
            // Increment distance expansion
            if (actor->counters.mMovementCoolDownCounter < COOLDOWN_MOVEMENT_MAX) 
                actor->counters.mMovementCoolDownCounter++;
            
            actor->counters.mObservationCoolDownCounter++;
            
            if (actor->counters.mObservationCoolDownCounter == 10) {
                actor->counters.mObservationCoolDownCounter = 0;
                
                UpdateTargetPoint(actor);
            }
            
        }
        
        
        // Check within prefered range
        HandleMovementHeight(actor, isAquatic);
        
    }
    
    return;
}

void ActorSystem::HandleFocusOnNearbyActor(Actor* actor) {
    
    if (actor->navigation.mDistance < actor->behavior.mDistanceToFocus) {
        
        // Focus on player
        
        actor->navigation.mTargetPoint.x = mPlayerPosition.x;
        actor->navigation.mTargetPoint.z = mPlayerPosition.z;
        
        actor->counters.mObservationCoolDownCounter = 0;
        
        actor->state.mIsWalking = false;
        actor->state.mIsRunning = false;
        
    } else {
        
        // TODO: Focus on nearby actor.
        if (actor->navigation.mTargetActor != nullptr) {
            
            //actor->navigation.mTargetPoint.x = actor->mTargetActor->mPosition.x;
            //actor->navigation.mTargetPoint.z = actor->mTargetActor->mPosition.z;
            
            //actor->state.mIsWalking = false;
            //actor->state.mIsRunning = false;
            
        }
            
        
    }
    
    return;
}

void ActorSystem::UpdateTargetPoint(Actor* actor) {
    
    float randA = Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    float randB = Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    float randC = Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    float randD = Random.Range(0.0f, actor->behavior.mDistanceToWalk);
    
    randA += actor->counters.mMovementCoolDownCounter + 1;
    randB += actor->counters.mMovementCoolDownCounter + 1;
    randC += actor->counters.mMovementCoolDownCounter + 1;
    randD += actor->counters.mMovementCoolDownCounter + 1;
    
    actor->navigation.mTargetPoint.x = (randA - randB) + actor->navigation.mPosition.x;
    actor->navigation.mTargetPoint.y = -100000.0f;
    actor->navigation.mTargetPoint.z = (randC - randD) + actor->navigation.mPosition.z;
    
    return;
}

void ActorSystem::CheckTargetReached(Actor* actor) {
    
    if (glm::distance(actor->navigation.mTargetPoint, actor->navigation.mPosition) < DISTANCE_MINIMUM_TARGET_REACHED) {
        
        actor->counters.mMovementCoolDownCounter = 0;
        
        actor->state.mIsWalking = false;
    }
    
    return;
}

void ActorSystem::HandleMovementHeight(Actor* actor, bool isAquatic) {
    
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
    
    return;
}
