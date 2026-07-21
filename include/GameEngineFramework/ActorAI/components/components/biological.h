#ifndef AI_ACTOR_BIOLOGICAL
#define AI_ACTOR_BIOLOGICAL

#include <GameEngineFramework/ActorAI/ActorSystem.h>

class ENGINE_API BiologicalSystem {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Amount of health the actor currently has.
    float health;
    /// How hungry the actor is.
    float hunger;
    
    /// Attack damage ability.
    float strength;
    /// Resistance to attack damage.
    float defense;
    
    BiologicalSystem();
    
private:
    
    // Biological elements
    std::vector<Bio> mBiologics;
    
};

#endif
