#ifndef AI_ACTOR_BIOLOGICAL
#define AI_ACTOR_BIOLOGICAL

#include <GameEngineFramework/configuration.h>

class ENGINE_API BiologicalSystem {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Amount of health the actor currently has.
    float health;
    
    /// Maximum amount of health the actor can have.
    float healthMax;
    
    /// How hungry the actor is.
    float hunger;
    /// How full the actor is.
    float saturation;
    
    /// Attack damage ability.
    float strength;
    /// Resistance to attack damage.
    float defense;
    
    BiologicalSystem();
    
    void Reset();
    
private:
    
    // Biological elements effecting this actor
    std::vector<Bio> mBiologics;
    
    unsigned int mHungerCounter;
};

#endif
