#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern Logger Log;
extern ActorSystem AI;
extern NumberGeneration Random;
extern MathCore Math;

// Actor system thread
bool isActorThreadActive = true;
bool doUpdate = false;
void actorThreadMain(void);

int actorCounter=0;


ActorSystem::ActorSystem() : 
    mPlayerPosition(0)
{
}

void ActorSystem::Initiate(void) {
    
    mActorSystemThread = new std::thread( actorThreadMain );
    
    Log.Write( " >> Starting thread AI" );
    
    return;
}

void ActorSystem::Shutdown(void) {
    
    mux.lock();
    isActorThreadActive = false;
    mux.unlock();
    
    mActorSystemThread->join();
    
    return;
}

void ActorSystem::SetPlayerWorldPosition(glm::vec3 position) {
    mux.lock();
    mPlayerPosition = position;
    mux.unlock();
    return;
}

glm::vec3 ActorSystem::GetPlayerWorldPosition(void) {
    mux.lock();
    glm::vec3 position(mPlayerPosition);
    mux.unlock();
    return position;
}

void ActorSystem::UpdateSendSignal(void) {
    mux.lock();
    doUpdate = true;
    mux.unlock();
    return;
}

Actor* ActorSystem::CreateActor(void) {
    mux.lock();
    Actor* newActor = mActors.Create();
    mux.unlock();
    return newActor;
}

bool ActorSystem::DestroyActor(Actor* actorPtr) {
    mux.lock();
    bool state = mActors.Destroy(actorPtr);
    mux.unlock();
    return state;
}

unsigned int ActorSystem::GetNumberOfActors(void) {
    mux.lock();
    unsigned int value = mActors.GetObjectCount();
    mux.unlock();
    return value;
}

Actor* ActorSystem::GetActor(unsigned int index) {
    mux.lock();
    Actor* actorPtr = mActors[index];
    mux.unlock();
    return actorPtr;
}


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
            break;
        }
        
        Actor* actor = mActors[actorCounter];
        
        actorCounter++;
        
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
        
        
        if (!actor->mIsActive) {
            actor->mux.unlock();
            continue;
        }
        
        // Advance actor age
        actor->mAge++;
        
        
        
        if ((Random.Range(0, 1000) < actor->mChanceToWalk)) {
            actor->mIsWalking = true;
            actor->mRotateTo.y = Random.Range(0, 360);
        }
        
        if (!actor->mIsWalking) {
            
            // Chance to pick new target destination
            if ((Random.Range(0, 1000) < actor->mChanceToChangeDirection)) {
                
                actor->mTargetPoint.x = (Random.Range(0.0f, actor->mDistanceToWalk) - 
                                         Random.Range(0.0f, actor->mDistanceToWalk)) + actor->mPosition.x;
                
                actor->mTargetPoint.z = (Random.Range(0.0f, actor->mDistanceToWalk) - 
                                         Random.Range(0.0f, actor->mDistanceToWalk)) + actor->mPosition.z;
            }
            
        } else {
            
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



//
// Actor system thread
//

void actorThreadMain() {
    
    while (isActorThreadActive) {
        
        if (!doUpdate) {
            std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
            continue;
        }
        
        AI.Update();
        
        doUpdate = false;
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
    Log.Write( " >> Shutting down on thread AI" );
    
    return;
}

