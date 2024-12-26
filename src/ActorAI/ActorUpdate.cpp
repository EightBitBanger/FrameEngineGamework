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


void ActorSystem::Update(void) {
    
    glm::vec3 forward(0);
    
    int numberOfActors = mActors.Size();
    
    // Update cycle counter
    int numberOfActorsPerCycle = 1;
    
    if (numberOfActors > 10) 
        numberOfActorsPerCycle = numberOfActors / 10;
    
    // Tick counter
    tickCounter++;
    if (tickCounter < 10000000) 
        return;
    
    tickCounter=0;
    
    
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
        
        // Check actor breeding state
        if (actor->mBreedWithActor != nullptr) {
            
            actor->mTargetPoint = actor->mBreedWithActor->mPosition;
            
            if (glm::distance(actor->mPosition, actor->mBreedWithActor->mPosition) > 1.0f) {
                
                actor->mIsWalking = true;
                
            } else {
                
                actor->mIsWalking = false;
            }
            
            actor->mux.unlock();
            continue;
        }
        
        
        // Chance to start walking
        if (actor->mTargetPoint.y != 0) {
            
            if ((Random.Range(0, 1000) < actor->mChanceToWalk)) 
                actor->mIsWalking = true;
            
        }
        
        // Cool down after a change in direction
        if (actor->mObservationCoolDownCounter < 100) 
            actor->mObservationCoolDownCounter++;
        
        // Chance to focus on a near by actor or player
        if ((Random.Range(0, 10000) < actor->mChanceToFocusOnActor)) {
            
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
            if ((Random.Range(0, 1000) < actor->mChanceToChangeDirection)) {
                
                float randA = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
                float randB = Random.Range(0.0f, actor->mDistanceToWalk) + (actor->mMovementCoolDownCounter + 1);
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
                
                if (actor->mMovementCoolDownCounter > 100) 
                    actor->mMovementCoolDownCounter = 100;
                
                // Avoid moving deeper out of range
                if (actor->mPosition.y < mWorldWaterLevel) {
                    
                    if (actor->mTargetPoint.y < actor->mPosition.y) 
                        actor->mIsWalking = false;
                    
                }
                
            }
            
            // Check reached destination
            if (glm::distance( actor->mTargetPoint, actor->mPosition ) < 1.5f) {
                
                actor->mMovementCoolDownCounter = 0;
                
                actor->mIsWalking = false;
            }
            
        }
        
        
        
        
        
        // TODO: Fix neural networking.
        
        
        //if ((Random.Range(0, 10000) < actor->mChanceToStopWalking)) {
        //    actor->mIsWalking = false;
        //}
        
        
        
        
        /*
        // Check neural network control
        int numberOfLayers = actor->mWeightedLayers.size();
        
        if (numberOfLayers < 1) {
            actor->mux.unlock();
            continue;
        }
        
        //
        // Send in the input data
        //
        
        for (int w=0; w < NEURAL_LAYER_WIDTH; w++) 
            actor->mWeightedLayers[0].node[w] = (Random.Range(0, 10) * 0.001) - (Random.Range(0, 10) * 0.001);
        
        //
        // Feed data through the network
        //
        
        for (int a=1; a < numberOfLayers; a++) {
            
            for (int w=0; w < NEURAL_LAYER_WIDTH; w++) 
                actor->mWeightedLayers[a].node[w] = actor->mWeightedLayers[a-1].node[w] * 
                                                    actor->mWeightedLayers[a-1].weight[w];
            
            continue;
        }
        
        // Apply neural plasticity
        // Plasticity is the amount to which the weights will conform to the data coming though
        
        for (int a=0; a < numberOfLayers; a++) {
            
            for (int w=0; w < NEURAL_LAYER_WIDTH; w++) 
                actor->mWeightedLayers[a].weight[w] = Math.Lerp(actor->mWeightedLayers[a].weight[w], 
                                                                actor->mWeightedLayers[a].node[w], 
                                                                actor->mWeightedLayers[a].plasticity);
            
            continue;
        }
        
        // Get the outputs from the last layer
        NeuralLayer outputLayer;
        for (int w=0; w < NEURAL_LAYER_WIDTH; w++) 
            outputLayer.node[w] = actor->mWeightedLayers[ numberOfLayers-1 ].node[w];
        
        // Apply outputs to action
        
        // Move forward
        
        if (outputLayer.node[0] > 0) {
            actor->mIsWalking = true;
        } else {
            actor->mIsWalking = false;
        }
        
        if (outputLayer.node[1] < 0) {
            actor->mRotateTo.y = Random.Range(0, 360);
        }
        
        //actor->mRotateTo.y
        
        */
        
        actor->mux.unlock();
        
        continue;
    }
    
    mux.unlock();
    
    return;
}

