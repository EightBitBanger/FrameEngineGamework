#ifndef _ACTOR_AI_SYSTEM__
#define _ACTOR_AI_SYSTEM__

#include <thread>
#include <mutex>
#include <chrono>

#include "../../../vendor/CodeBaseLibrary/poolallocator.h"

#include "components/actor.h"
#include "components/neuron.h"
#include "Genetics.h"


class ENGINE_API ActorSystem {
    
public:
    
    ActorSystem();
    
    /// Create an actor and return its pointer.
    Actor* CreateActor(void);
    
    /// Destroy an actor.
    bool DestroyActor(Actor* actorPtr);
    
    /// Initiate the actor AI system.
    void Initiate(void);
    
    /// Shutdown the actor AI system.
    void Shutdown(void);
    
    /// Set the player position in the simulation.
    void SetPlayerWorldPosition(glm::vec3 position);
    
    /// Get the number of actors in the AI simulation.
    unsigned int GetNumberOfActors(void);
    
    /// Get an actor from the simulation by its index.
    Actor* GetActor(unsigned int index);
    
    /// Signal to the AI thread to update the simulation.
    void UpdateSendSignal(void);
    
    /// Update simulation actors. (called internally from another thread)
    void Update();
    
    
private:
    
    // Current position of the player in the world
    glm::vec3 playerPosition;
    
    std::thread* actorSystemThread;
    
    std::mutex mux;
    
    PoolAllocator<Actor> mActors;
    
    PoolAllocator<Neuron> mNeurons;
    
};


#endif
