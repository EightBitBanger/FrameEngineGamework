#ifndef CORE_PROFILER
#define CORE_PROFILER

#include "../../../vendor/codebaselibrary/timer.h"

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
    
    ProfilerTimer();
    
private:
    
    Timer timer;
    
};

#endif
