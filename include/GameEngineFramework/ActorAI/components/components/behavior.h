#ifndef AI_ACTOR_BEHAVIOR
#define AI_ACTOR_BEHAVIOR

#include <GameEngineFramework/configuration.h>
class Actor;

class ENGINE_API Behavior {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Set the distance to which the actor can focus on another actor or player.
    void SetDistanceToFocus(float distance);
    /// Get the distance to which the actor can focus on another actor or player.
    float GetDistanceToFocus(void);
    
    /// Set the distance to walk when selecting new random positions.
    void SetDistanceToWalk(float distance);
    /// Get the distance to walk when selecting new random positions.
    float GetDistanceToWalk(void);
    
    /// Set the distance to begin attacking a pray actor if any are nearby.
    void SetDistanceToAttack(float distance);
    /// Get the distance to begin attacking a pray actor if any are nearby.
    float GetDistanceToAttack(void);
    
    /// Set the distance to begin running from a predator actor if any are nearby.
    void SetDistanceToFlee(float distance);
    /// Get the distance to begin running from a predator actor if any are nearby.
    float GetDistanceToFlee(void);
    
    /// Distance to inflict damage on a target actors health.
    void SetDistanceToInflict(float distance);
    /// Distance to inflict damage on a target actors health.
    float GetDistanceToInflict(void);
    
    /// Set the idle timeout after a successful attack.
    void SetCooldownAttack(unsigned int ticks);
    /// Get the attack cool down timer.
    unsigned int GetCooldownAttack(void);
    
    /// Set the observation cool down after an local observation.
    void SetCooldownObserve(unsigned int ticks);
    /// Get the observation cool down timer.
    unsigned int GetCooldownObserve(void);
    
    /// Set the socialization cool down after a social event.
    void SetCooldownSocial(unsigned int ticks);
    /// Get the socialization cool down timer.
    unsigned int GetCooldownSocial(void);
    
    /// Set the idle timeout when completing a movement.
    void SetCooldownMove(unsigned int ticks);
    /// Get the movement cool down timer.
    unsigned int GetCooldownMove(void);
    
    /// Set reproduction timeout after just reproducing.
    void SetCooldownBreed(unsigned int ticks);
    /// Get reproduction cool down timer.
    unsigned int GetCooldownBreed(void);
    
    /// Set the minimum height preference when traveling.
    void SetHeightPreferenceMin(float height);
    /// Get the minimum height preference when traveling.
    float GetHeightPreferenceMin(void);
    
    /// Set the maximum height preference when traveling.
    void SetHeightPreferenceMax(float height);
    /// Get the maximum height preference when traveling.
    float GetHeightPreferenceMax(void);
    
    /// Set the attack state.
    void SetPredatorState(bool state);
    /// Get the attack state.
    bool GetPredatorState(void);
    
    /// Set the prey state.
    void SetPreyState(bool state);
    /// Get the prey state.
    bool GetPreyState(void);
    
    Behavior();
    
    void Reset();
    
private:
    
    bool mIsPredator;               // True = can attack prey   False = herbivore
    bool mIsPrey;                   // True = can be attacked   False = docile
    
    float mDistanceToFocus;         // Distance to focus on a near by actor
    float mDistanceToWalk;          // Distance to travel when moving to a random position
    float mDistanceToAttack;        // Distance to begin attacking a pray actor
    float mDistanceToFlee;          // Distance to begin fleeing from a predator actor
    float mDistanceToInflict;       // Distance to register an attack on a target
    
    unsigned int mCooldownAttack;   // Ticks to idle after a successful attack
    unsigned int mCooldownObserve;  // Ticks to focus on a target before switching states
    unsigned int mCooldownSocial;   // Ticks to socialize with a target before switching states
    unsigned int mCooldownMove;     // Ticks to idle before moving randomly again
    unsigned int mCooldownBreed;    // Ticks to idle before reproducing again
    
    float mHeightPreferenceMin;     // Minimum world height this actor prefers to inhabit
    float mHeightPreferenceMax;     // Maximum world height this actor prefers to inhabit
    
};

#endif
