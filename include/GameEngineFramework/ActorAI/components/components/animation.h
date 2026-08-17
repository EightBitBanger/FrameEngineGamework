#ifndef AI_ACTOR_ANIMATION
#define AI_ACTOR_ANIMATION

#include <GameEngineFramework/ActorAI/ActorSystem.h>

class ENGINE_API AnimationState {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    AnimationState();
    
private:
    // Animation timer
    float mWalkTime;
    
    // List of animation states for each genetic component
    std::vector<glm::vec4> mAnimation;
};

#endif
