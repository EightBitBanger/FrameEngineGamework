#ifndef AI_ACTOR_STATE
#define AI_ACTOR_STATE

#include <GameEngineFramework/configuration.h>

class ENGINE_API State {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    ActorState::Mode mode;
    
    bool GetStateWalking(void);
    bool GetStateRunning(void);
    bool GetStateFacing(void);
    
    State();
    
    void Reset();
    
private:
    
    bool mIsWalking;
    bool mIsRunning;
    bool mIsFacing;
};

#endif
