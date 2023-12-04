#include <GameEngineFramework/Profiler/profiler.h>
ProfilerTimer::ProfilerTimer() : 
    profileRenderSystem(0),
    profilePhysicsSystem(0),
    profileGameEngineUpdate(0),
    profileActorAI(0)
{
}

void ProfilerTimer::Begin(void) {
    timer.Update();
    return;
}

float ProfilerTimer::Query(void) {
    return timer.GetCurrentDelta();
}
