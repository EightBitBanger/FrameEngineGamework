#ifndef _ACTOR_AI_SYSTEM__
#define _ACTOR_AI_SYSTEM__

#include <thread>
#include <mutex>
#include <chrono>

#include "../../vendor/CodeBaseLibrary/poolallocator.h"

#include "components/actor.h"
#include "Genetics.h"


class __declspec(dllexport) ActorSystem {
    
public:
    
    ActorSystem();
    
    void Update(void);
    
    /// Create an actor and return its pointer.
    Actor* CreateActor(void);
    
    /// Destroy an actor.
    bool DestroyActor(Actor* actorPtr);
    
    /// Initiate the actor AI system.
    void Initiate(void);
    
    /// Shutdown the actor AI system.
    void Shutdown(void);
    
    /// Get the number of actors in the AI simulation.
    unsigned int GetNumberOfActors(void);
    
    /// Get an actor from the simulation by its index.
    Actor* GetActor(unsigned int index);
    
private:
    
    std::thread* actorSystemThread;
    
    std::mutex mux;
    
    PoolAllocator<Actor> mActors;
    
    
};


#endif
