#ifndef AI_ACTOR_EMOTIONAL
#define AI_ACTOR_EMOTIONAL

#include <GameEngineFramework/configuration.h>

class ENGINE_API EmotionalState {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    EmotionalEmbedding current;
    
    /// Set the fear level for the actor.
    void SetFear(float fear);
    /// Get the fear level for the actor.
    float GetFear(void);
    /// Add fear to the actor.
    void AddFear(float additive);
    
    /// Set the anger level for the actor.
    void SetAnger(float anger);
    /// Get the anger level for the actor.
    float GetAnger(void);
    /// Add anger to the actor.
    void AddAnger(float additive);
    
    /// Set the fatigue level for the actor.
    void SetFatigue(float fatigue);
    /// Get the fatigue level for the actor.
    float GetFatigue(void);
    /// Add fatigue to the actor.
    void AddFatigue(float additive);
    
    /// Set the stress level for the actor.
    void SetStress(float stress);
    /// Get the stress level for the actor.
    float GetStress(void);
    /// Add stress to the actor.
    void AddStress(float additive);
    
    /// Set the curiosity level for the actor.
    void SetCuriosity(float curiosity);
    /// Get the curiosity level for the actor.
    float GetCuriosity(void);
    /// Add curiosity to the actor.
    void AddCuriosity(float additive);
    
    /// Set the comfort level for the actor.
    void SetComfort(float comfort);
    /// Get the comfort level for the actor.
    float GetComfort(void);
    /// Add comfort to the actor.
    void AddComfort(float additive);
    
    /// Set the libido level for the actor.
    void SetLibido(float libido);
    /// Get the libido level for the actor.
    float GetLibido(void);
    /// Add libido to the actor.
    void AddLibido(float additive);
    
    EmotionalState();
    
    void Reset();
};

#endif
