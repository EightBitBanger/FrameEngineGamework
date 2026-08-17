#ifndef CORE_SCOPE_PROFILER
#define CORE_SCOPE_PROFILER

#include <GameEngineFramework/configuration.h>

#include <SDL3/SDL.h>

class ENGINE_API ScopeProfile {
    friend class ProfilerTimer;
public:
    ScopeProfile(const char* name);
    ~ScopeProfile();

private:
    const char* mName;
    Uint64 mStartTimeNS;
};

#endif
