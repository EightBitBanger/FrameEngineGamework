#ifndef AI_ACTOR_PHYSICAL
#define AI_ACTOR_PHYSICAL

#include <GameEngineFramework/configuration.h>

class ENGINE_API PhysicalAttributes {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Set the age of the actor.
    void SetAge(unsigned long int newAge);
    /// Get the age of the actor.
    unsigned long int GetAge(void);
    
    /// Set the senior age of the actor.
    void SetSeniorAge(float oldAge);
    /// Get the senior age of the actor.
    float GetSeniorAge(void);
    
    /// Set the age at which the actor will begin adulthood.
    void SetAdultAge(float age);
    /// Get the age at which the actor will begin adulthood.
    float GetAdultAge(void);
    
    /// Set the walking speed of the actor.
    void SetSpeed(float newSpeed);
    /// Get the walking speed of the actor.
    float GetSpeed(void);
    
    /// Set the speed the actor can run at as a youth.
    void SetSpeedYouth(float speed);
    /// Get the speed the actor can run at as a youth.
    float GetSpeedYouth(void);
    
    /// Set the running speed multiplier of the actor.
    void SetSpeedMultiplier(float newSpeedMul);
    /// Get the running speed multiplier of the actor.
    float GetSpeedMultiplier(void);
    
    /// Set the initial scale for the actor.
    void SetYouthScale(float scale);
    /// Get the initial scale from the actor.
    float GetYouthScale(void);
    
    /// Set the max adult scale for the actor.
    void SetAdultScale(float scale);
    /// Get the max adult scale from the actor.
    float GetAdultScale(void);
    
    /// Set the sexual orientation for reproduction.
    void SetSexualOrientation(bool orientation);
    /// Get the sexual orientation for reproduction.
    bool GetSexualOrientation(void);
    
    PhysicalAttributes();
    
    void Reset();
    
private:
    
    unsigned long int mAge;      // Number of ticks this actor has accumulated in its lifetime
    float mAgeAdult;             // Age at which the actor is able to reproduce.
    float mAgeSenior;            // Maximum Age at which the actor is considered a senior.
    float mSpeed;                // Maximum speed this actor can travel
    float mSpeedYouth;           // Maximum speed this actor can travel as a youth
    float mSpeedMul;             // Running speed multiplier
    float mSnapSpeed;            // Rotation speed when changing directions
    float mYouthScale;           // Initial scale for this actor
    float mAdultScale;           // Max scale for adult actor
    
    bool mSexualOrientation;     // Reproductive orientation  1=Male 0=Female
};

#endif
