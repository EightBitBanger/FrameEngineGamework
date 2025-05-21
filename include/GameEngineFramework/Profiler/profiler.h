#ifndef CORE_PROFILER
#define CORE_PROFILER

#include <GameEngineFramework/Timer/timer.h>

class ENGINE_API ProfilerTimer {
    
public:
    
    /// Profile result for the render system.
    float profileRenderSystem;
    
    /// Profile result for the physics system.
    float profilePhysicsSystem;
    
    /// Profile result for the engine system.
    float profileGameEngineUpdate;
    
    /// Profile result for the AI system.
    float profileActorAI;
    
    /// Begin the profile timer.
    void Begin(void);
    
    /// Check the elapse time since begin.
    float Query(void);
    
    /// Check if the profiler is active.
    bool CheckIsProfilerActive(void);
    
    /// Start the profiler.
    void Activate(void);
    
    /// Stop the profiler.
    void Deactivate(void);
    
    ProfilerTimer();
    
private:
    
    // Should the profiler profile the system update functions
    bool isActive;
    
    Timer timer;
    
};

#endif
