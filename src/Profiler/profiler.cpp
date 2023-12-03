#include <GameEngineFramework/Profiler/profiler.h>

void ProfilerTimer::Begin(void) {
    timer.Update();
    return;
}

float ProfilerTimer::Query(void) {
    return timer.GetCurrentDelta();
}
