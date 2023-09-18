#include "ActorSystem.h"


ActorSystem::ActorSystem() {
    
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
