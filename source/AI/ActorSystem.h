#ifndef _ACTOR_AI_SYSTEM__
#define _ACTOR_AI_SYSTEM__

#include "../../vendor/CodeBaseLibrary/poolallocator.h"

#include "components/actor.h"


class ActorSystem {
    
public:
    
    ActorSystem();
    
    void Update(void);
    
    /// Create an actor and return its pointer.
    Actor* CreateActor(void);
    /// Destroy an actor.
    bool DestroyActor(Actor* actorPtr);
    
    
private:
    
    PoolAllocator<Actor> mActors;
    
    
};


#endif
