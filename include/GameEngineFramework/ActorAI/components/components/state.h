#ifndef AI_ACTOR_STATE
#define AI_ACTOR_STATE

#include <GameEngineFramework/ActorAI/ActorSystem.h>

class ENGINE_API State {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    ActorState::Mode mode;
    
    State();
    
private:
    
    bool mIsWalking;
    bool mIsRunning;
    bool mIsFacing;
};

#endif
