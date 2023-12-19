#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

extern Logger Log;
extern ActorSystem AI;
extern RandomGen Random;
extern MathCore Math;

// Actor system thread
bool isActorThreadActive = true;
bool doUpdate = false;
void actorThreadMain(void);

int actorCounter=0;


ActorSystem::ActorSystem() : 
    playerPosition(0)
{
}

void ActorSystem::Initiate(void) {
    
    actorSystemThread = new std::thread( actorThreadMain );
    
    Log.Write( " >> Starting thread AI" );
    
    return;
}

void ActorSystem::Shutdown(void) {
    
    isActorThreadActive = false;
    
    actorSystemThread->join();
    
    return;
}

void ActorSystem::SetPlayerWorldPosition(glm::vec3 position) {
    mux.lock();
    playerPosition = position;
    mux.unlock();
    return;
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
    
    float force = 0.0001;
    
    glm::vec3 forward(0);
    
    mux.lock();
    
    for (int i = 0; i < 80; i++) {
        
        mActors[actorCounter]->mux.lock();
        
        unsigned int numberOfActors = (unsigned int)mActors.Size();
        
        actorCounter++;
        if (actorCounter >= numberOfActors) {
            actorCounter = 0;
            
            //mActors[actorCounter]->mux.unlock();
            //break;
        }
        
        Actor* actor = mActors[actorCounter];
        
        if (!actor->isActive) {
            actor->mux.unlock();
            continue;
        }
        
        // Advance actor age
        actor->age++;
        
        
        
        // Must have at least two layers to feed the data forward
        if (actor->mWeightedLayers.size() < 2) {
            actor->mux.unlock();
            continue;
        }
        
        
        // Feed forward the weighted layers
        for (int a=1; a < actor->mWeightedLayers.size(); a++) {
            
            for (int w=0; w < NEURAL_LAYER_WIDTH; w++) 
                actor->mWeightedLayers[a].node[w] = actor->mWeightedLayers[a-1].node[w] * 
                                                    actor->mWeightedLayers[a-1].weight[w];
            
            continue;
        }
        
        // Apply neural plasticity
        for (int a=0; a < actor->mWeightedLayers.size(); a++) {
            
            for (int w=0; w < NEURAL_LAYER_WIDTH; w++) 
                actor->mWeightedLayers[a].weight[w] = Math.Lerp(1.0, actor->mWeightedLayers[a].node[w], actor->mWeightedLayers[a].plasticity);
            
            //actor->mWeightedLayers[a].plasticity) / 2;
            
            continue;
        }
        
        // Feed in new input data
        for (int a=0; a < NEURAL_LAYER_WIDTH; a++) 
            actor->mWeightedLayers[0].node[a] = actor->mNeuralLayerInput.node[a];
        
        
        
        // Chance to start moving
        //forward.x = (Random.Range(0, 100) - Random.Range(0, 100)) * force;
        //forward.y = (Random.Range(0, 100) - Random.Range(0, 100)) * force;
        //forward.z = (Random.Range(0, 100) - Random.Range(0, 100)) * force;
        
        
        // Apply forward velocity
        mActors[actorCounter]->velocity = forward;
        
        
        mActors[actorCounter]->mux.unlock();
        
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

