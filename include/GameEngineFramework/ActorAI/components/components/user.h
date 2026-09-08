#ifndef AI_ACTOR_USER
#define AI_ACTOR_USER

#include <GameEngineFramework/configuration.h>

class ENGINE_API UserVariables {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Set the user data pointer.
    void SetUserDataA(void* ptr);
    /// Get the user data pointer.
    void* GetUserDataA(void);
    
    /// Set the user data pointer.
    void SetUserDataB(void* ptr);
    /// Get the user data pointer.
    void* GetUserDataB(void);
    
    UserVariables();
    
    void Reset();
    
private:
    
    uint8_t mBitmask;
    
    void* mUserDataA;
    void* mUserDataB;
};

#endif
