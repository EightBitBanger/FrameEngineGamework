#include <ActorAI/ActorSystem.h>

#include "../../vendor/codebaselibrary/logging.h"
#include "../../vendor/codebaselibrary/random.h"

extern Logger Log;
extern ActorSystem AI;
extern RandomGen Random;

// Actor system thread
bool isActorThreadActive = true;
bool doUpdate = false;
void actorThreadMain(void);


ActorSystem::ActorSystem() : 
    playerPosition(0)
{
}

void ActorSystem::Initiate(void) {
    
    actorSystemThread = new std::thread( actorThreadMain );
    
    Log.Write( " >> AI thread started" );
    
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
    doUpdate = true;
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
    unsigned int numberOfActors = (unsigned int)mActors.Size() - 1;
    
    float force = 0.005;
    
    glm::vec3 forward(0);
    
    mux.lock();
    
    for (int i=numberOfActors; i >= 0; i--) {
        if (!mActors[i]->isActive) 
            continue;
        
        mActors[i]->mux.lock();
        
        // Advance actor age
        mActors[i]->age++;
        
        
        
        
        
        
        
        // Chance to start moving
        if (Random.Range(0, 10) > mActors[i]->chanceToMove) {
            forward.x = (Random.Range(0, 100) - Random.Range(0, 100)) * force;
            forward.y = (Random.Range(0, 100) - Random.Range(0, 100)) * force;
            forward.z = (Random.Range(0, 100) - Random.Range(0, 100)) * force;
        }
        
        
        
        
        
        
        
        
        // Apply forward velocity
        mActors[i]->velocity = forward;
        
        
        mActors[i]->mux.unlock();
        
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
    Log.Write( " >> AI thread stopped" );
    
    return;
}

