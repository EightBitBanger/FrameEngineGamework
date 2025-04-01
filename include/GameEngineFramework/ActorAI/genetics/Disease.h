#ifndef _GENETICS_DISEASE__
#define _GENETICS_DISEASE__

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/ActorAI/Genetics/Codon.h>

class ENGINE_API Disease {
    
public:
    
    /// Amount of time in ticks that this disease should effect the actor.
    float duration;
    
    /// Amount of health to remove from the actor every tick.
    float healthEffect;
    
    /// If contagious, how close should the other
    /// actor be to contract the disease
    float distance;
    
    /// Should this disease spread to other actors
    bool isContagious;
    
    /// Should this disease trigger the actor to die?
    bool isTerminal;
    
    Disease();
    
};

#endif
