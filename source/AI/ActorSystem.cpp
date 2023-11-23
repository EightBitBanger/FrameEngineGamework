#include "ActorSystem.h"

#include "../../vendor/codebaselibrary/logging.h"

extern Logger Log;

// Actor system thread
bool isActorThreadActive = true;
void actorThreadMain(void);


ActorSystem::ActorSystem() {
    
}

void ActorSystem::Initiate(void) {
    actorSystemThread = new std::thread( actorThreadMain );
    
    return;
}

void ActorSystem::Shutdown(void) {
    
    isActorThreadActive = false;
    
    actorSystemThread->join();
    
    return;
}

void ActorSystem::Update(void) {
    unsigned int numberOfActors = mActors.Size();
    for (int i=0; i < numberOfActors; i++) {
        if (!mActors[i]->isActive) 
            continue;
        
        mActors[i]->Update();
        
        // Advance actor age
        mActors[i]->age++;
        
        continue;
    }
    return;
}

Actor* ActorSystem::CreateActor(void) {
    return mActors.Create();
}

bool ActorSystem::DestroyActor(Actor* actorPtr) {
    return mActors.Destroy(actorPtr);
}

void actorThreadMain() {
    
    Log.Write( " >> AI thread started" );
    
    while (isActorThreadActive) {
        
        
        
        
        continue;
    }
    
    Log.Write( " >> AI thread stopped" );
    
    return;
}

