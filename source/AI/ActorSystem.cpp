#include "ActorSystem.h"

#include "../../vendor/codebaselibrary/logging.h"

extern Logger Log;
extern ActorSystem AI;

// Actor system thread
bool isActorThreadActive = true;
void actorThreadMain(void);


ActorSystem::ActorSystem() {
    
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

void ActorSystem::Update(void) {
    unsigned int numberOfActors = mActors.Size();
    
    mux.lock();
    
    for (unsigned int i=0; i < numberOfActors; i++) {
        if (!mActors[i]->isActive) 
            continue;
        
        mActors[i]->mux.lock();
        
        // Advance actor age
        mActors[i]->age++;
        
        mActors[i]->mux.unlock();
        
        continue;
    }
    
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
// Actor system thread
//

void actorThreadMain() {
    
    while (isActorThreadActive) {
        
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
        
        //unsigned int numberOfActors = AI.GetNumberOfActors();
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(1) );
    Log.Write( " >> AI thread stopped" );
    
    return;
}

