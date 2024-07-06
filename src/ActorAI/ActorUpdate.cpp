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


//
// AI actor processing
//

void ActorSystem::Update(void) {
    
    glm::vec3 forward(0);
    
    int numberOfActors = mActors.Size();
    
    mux.lock();
    
    for (int i = 0; i < 80; i++) {
        
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
        
        
        //
        // TESTING random movement
        //
        
        /*
        
        mIsWalking
        mIsRunning
        
        mIsAttacking
        
        mIsFleeing
        mIsConsuming
        
        mChanceToChangeDirection
        mChanceToFocusOnActor
        mChanceToWalk
        
        mDistanceToFocusOnActor
        mDistanceToWalk
        
        mHeightPreferenceMin
        mHeightPreferenceMax
        
        mDistanceToAttack
        mDistanceToFlee
        
        mDirectionChangeCoolDownCounter
        mObservationCoolDownCounter
        
        if (Random.Range(0, 100) > 98) 
            actor->mIsWalking = false;
        
        if (Random.Range(0, 100) > 98) 
            actor->mIsWalking = true;
        
        if (Random.Range(0, 100) > 97) 
            actor->mRotateTo.y = Random.Range(0, 360);
        
        */
        
        
        // Check actor update distance
        actor->mDistance = glm::distance( mPlayerPosition, actor->mPosition );
        
        if (actor->mDistance > mActorUpdateDistance) {
            actor->mux.unlock();
            continue;
        }
        
        // Advance actor age
        actor->mAge++;
        
        
        // Chance to start walking
        if ((Random.Range(0, 1000) < actor->mChanceToWalk)) 
            actor->mIsWalking = true;
        
        // Cool down after a change in direction
        if (actor->mObservationCoolDownCounter < 100) {
            
            actor->mObservationCoolDownCounter++;
            
        }
        
        
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
                
                // TODO: Focus on nearby actor. Probably islanding into regions.
                
                
                
            }
            
        }
        
        if ((!actor->mIsWalking) & (actor->mObservationCoolDownCounter != 0)) {
            
            actor->mObservationCoolDownCounter = 0;
            
            // Chance to pick new target destination
            if ((Random.Range(0, 1000) < actor->mChanceToChangeDirection)) {
                
                actor->mTargetPoint.x = (Random.Range(0.0f, actor->mDistanceToWalk) - 
                                         Random.Range(0.0f, actor->mDistanceToWalk)) + actor->mPosition.x;
                
                actor->mTargetPoint.z = (Random.Range(0.0f, actor->mDistanceToWalk) - 
                                         Random.Range(0.0f, actor->mDistanceToWalk)) + actor->mPosition.z;
            }
            
        } else {
            
            // Match the height to the target
            actor->mTargetPoint.y = actor->mPosition.y;
            
            // Check reached destination
            float distance = glm::distance( actor->mTargetPoint, actor->mPosition );
            
            if (distance < 1.5) 
                actor->mIsWalking = false;
            
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

