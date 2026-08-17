#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Profiler/profiler.h>
#include <GameEngineFramework/Profiler/scopeprofile.h>

extern ProfilerTimer Profiler;

ScopeProfile::ScopeProfile(const char* name) 
    : mName(name), mStartTimeNS(SDL_GetTicksNS()) {
}

ScopeProfile::~ScopeProfile() {
    Profiler.SubmitProfile(*this);
}
