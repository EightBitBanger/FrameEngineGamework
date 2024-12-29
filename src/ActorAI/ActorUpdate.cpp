#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern Logger Log;
extern ActorSystem AI;
extern NumberGeneration Random;
extern MathCore Math;

// Actor system thread
extern bool isActorThreadActive;
extern bool doUpdate;

extern int actorCounter;

extern int tickCounter;

#define NUMBER_OF_TICKS  10000000

#define DECISION_CHANCE_TO_WALK                  1000
#define DECISION_CHANCE_TO_CHANGE_DIRECTION      1000
#define DECISION_CHANCE_TO_FOCUS_NEARBY          10000
#define DECISION_CHANCE_TO_STOP_MOVING           10000
#define COOLDOWN_OBSERVATION_MAX                 100
#define COOLDOWN_MOVEMENT_MAX                    100

#define DISTANCE_MINIMUM_TARGET_REACHED          1.5f
#define DISTANCE_MINIMUM_TARGET_BREEDING         1.0f


void ActorSystem::Update(void) {
    
    // Tick counter
    tickCounter++;
    if (tickCounter < NUMBER_OF_TICKS) 
        return;
    
    tickCounter=0;
    
    int numberOfActors = mActors.Size();
    
    // Update cycle counter
    int numberOfActorsPerCycle = 1;
    
    if (numberOfActors > 10) 
        numberOfActorsPerCycle = numberOfActors / 10;
    
    mux.lock();
    
    for (int i = 0; i < numberOfActorsPerCycle; i++) {
        
        if (actorCounter >= numberOfActors) {
            
            actorCounter = 0;
            
            doUpdate = false;
            
            break;
        }
        
        Actor* actor = mActors[actorCounter];
        
        actorCounter++;
        
        if (!actor->mIsActive) 
            continue;
        
        actor->mux.lock();
        
        // Check actor update distance
        actor->mDistance = glm::distance( mPlayerPosition, actor->mPosition );
        
        if (actor->mDistance > mActorUpdateDistance) {
            actor->mux.unlock();
            continue;
        }
        
        // Advance actor age
        actor->mAge++;
        
        // Check if the actor is aquatic
        bool isAquatic = false;
        
        if (actor->mHeightPreferenceMax < mWorldWaterLevel) 
            isAquatic = true;
        
        // Check actor breeding state
        if ((actor->mTargetPoint.y < actor->mHeightPreferenceMax) | 
            (actor->mTargetPoint.y > actor->mHeightPreferenceMin)) {
            
            if (actor->mBreedWithActor != nullptr) {
                
                // Walk towards the other actor
                actor->mTargetPoint = actor->mBreedWithActor->mPosition;
                
                if (glm::distance(actor->mPosition, actor->mBreedWithActor->mPosition) > DISTANCE_MINIMUM_TARGET_BREEDING) {
                    
                    actor->mIsWalking = true;
                    
                } else {
                    
                    actor->mIsWalking = false;
                }
                
                actor->mux.unlock();
                continue;
            }
            
        }
        
        
        if (!isAquatic) {
            
            // Chance to start walking
            
            if (actor->mTargetPoint.y > mWorldWaterLevel) {
                
                if ((Random.Range(0, DECISION_CHANCE_TO_WALK) < actor->mChanceToWalk)) 
                    actor->mIsWalking = true;
                
            }
            
        }
        
        // Cool down after a change in direction
        if (actor->mObservationCoolDownCounter < COOLDOWN_OBSERVATION_MAX) 
            actor->mObservationCoolDownCounter++;
        
        // Chance to focus on a near by actor or player
        if ((Random.Range(0, DECISION_CHANCE_TO_FOCUS_NEARBY) < actor->mChanceToFocusOnActor)) {
            
            // Distance to focus on player
            if (actor->mDistance < actor->mDistanceToFocusOnActor) {
                
                // Focus on player position
                actor->mTargetPoint = mPlayerPosition;
                
                actor->mObservationCoolDownCounter = 0;
                
                actor->mIsWalking = false;
                
                actor->mIsRunning = false;
                
            } else {
                
                // TODO: Focus on nearby actor.
                
                
                
            }
            
        }
        
        // Not walking and cooling down
        if ((!actor->mIsWalking) & (actor->mObservationCoolDownCounter != 0)) {
            
            actor->mObservationCoolDownCounter = 0;
            
            // Chance to pick new target destination
            if ((Random.Range(0, DECISION_CHANCE_TO_CHANGE_DIRECTION) < actor->mChanceToChangeDirection)) {
                
                // Get random X position
                float randA = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
                float randB = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
                
                // Get random Z position
                float randC = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
                float randD = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
                
                actor->mTargetPoint.x = (randA - randB) + actor->mPosition.x;
                actor->mTargetPoint.z = (randC - randD) + actor->mPosition.z;
                
                
            }
            
        } else {
            
            // Check height preference is out of range
            if ((actor->mTargetPoint.y > actor->mHeightPreferenceMax) | 
                (actor->mTargetPoint.y < actor->mHeightPreferenceMin)) {
                
                // Expand search range for prefered height range
                actor->mMovementCoolDownCounter += 1;
                
                if (actor->mMovementCoolDownCounter > COOLDOWN_MOVEMENT_MAX) 
                    actor->mMovementCoolDownCounter = COOLDOWN_MOVEMENT_MAX;
                
                if (!isAquatic) {
                    
                    // Avoid moving deeper out of range
                    
                    if (actor->mPosition.y < mWorldWaterLevel) {
                        
                        if (actor->mTargetPoint.y < actor->mPosition.y) 
                            actor->mIsWalking = false;
                        
                    }
                    
                } else {
                    
                    // Avoid moving shallower out of range
                    
                    if (actor->mPosition.y > mWorldWaterLevel) {
                        
                        if (actor->mTargetPoint.y > actor->mPosition.y) 
                            actor->mIsWalking = false;
                        
                    }
                    
                }
                
            }
            
            // Check reached destination
            if (glm::distance( actor->mTargetPoint, actor->mPosition ) < DISTANCE_MINIMUM_TARGET_REACHED) {
                
                actor->mMovementCoolDownCounter = 0;
                
                actor->mIsWalking = false;
            }
            
        }
        
        
        // Chance to stop walking
        if ((Random.Range(0, DECISION_CHANCE_TO_STOP_MOVING) < actor->mChanceToStopWalking)) {
            actor->mIsWalking = false;
        }
        
        
        actor->mux.unlock();
        
        continue;
    }
    
    mux.unlock();
    
    return;
}

