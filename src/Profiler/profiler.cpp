#include <GameEngineFramework/Profiler/profiler.h>

ProfilerTimer::ProfilerTimer() : 
    profileRenderSystem(0),
    profilePhysicsSystem(0),
    profileGameEngineUpdate(0),
    profileActorAI(0),
    isActive(false)
{
}

void ProfilerTimer::Begin(void) {
    timer.Update();
    return;
}

float ProfilerTimer::Query(void) {
    return timer.GetCurrentDelta();
}

bool ProfilerTimer::CheckIsProfilerActive(void) {
    return isActive;
}

void ProfilerTimer::Activate(void) {
    isActive = true;
    return;
}

void ProfilerTimer::Deactivate(void) {
    isActive = false;
    return;
}
