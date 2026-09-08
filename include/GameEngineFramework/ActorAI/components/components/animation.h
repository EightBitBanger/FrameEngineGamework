#ifndef AI_ACTOR_ANIMATION
#define AI_ACTOR_ANIMATION

#include <GameEngineFramework/configuration.h>

class ENGINE_API AnimationState {
    friend class Actor;
    friend class ActorSystem;
    friend class GeneticPresets;
    friend class EngineSystemManager;
public:
    
    /// Set the animation rate.
    void SetAnimationCycleRate(float rate);
    
    /// Get the animation rate.
    float GetAnimationCycleRate(void);
    
    AnimationState();
    
private:
    
    // Rate at which the animation will cycle
    float mWalkRate;
    
    // Animation timer
    float mWalkTime;
    
    // List of animation states for each genetic component
    std::vector<glm::vec4> mAnimation;
};

#endif
