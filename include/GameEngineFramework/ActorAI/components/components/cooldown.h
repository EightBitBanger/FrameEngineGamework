#ifndef AI_ACTOR_COOLDOWN
#define AI_ACTOR_COOLDOWN

#include <GameEngineFramework/ActorAI/ActorSystem.h>

class ENGINE_API CooldownCounters {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Set the current cool down counter for actor observation.
    void SetCoolDownObservation(unsigned int counter);
    /// Get the current cool down counter for actor observation.
    unsigned int GetCoolDownObservation(void);
    
    /// Set the current cool down counter for actor socialization.
    void SetCoolDownSocial(unsigned int counter);
    /// Get the current cool down counter for actor socialization.
    unsigned int GetCoolDownSocial(void);
    
    /// Set the current cool down counter for actor movement.
    void SetCoolDownMovement(unsigned int counter);
    /// Get the current cool down counter for actor movement.
    unsigned int GetCoolDownMovement(void);
    
    /// Set the current cool down counter for actor attacking.
    void SetCoolDownAttack(unsigned int counter);
    /// Get the current cool down counter for actor attacking.
    unsigned int GetCoolDownAttack(void);
    
    /// Set the current cool down counter for actor breeding.
    void SetCoolDownBreeding(unsigned int counter);
    /// Get the current cool down counter for actor breeding.
    unsigned int GetCoolDownBreeding(void);
    
    CooldownCounters();
    
private:
    
    unsigned int mObservationCoolDownCounter;
    unsigned int mSocialCoolDownCounter;
    unsigned int mMovementCoolDownCounter;
    unsigned int mAttackCoolDownCounter;
    unsigned int mBreedingCoolDownCounter;
};

#endif
