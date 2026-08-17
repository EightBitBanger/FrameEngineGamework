#ifndef CORE_PROFILER
#define CORE_PROFILER

#include <GameEngineFramework/Profiler/scopeprofile.h>

#include <map>
#include <string>
#include <mutex>

class ENGINE_API ProfilerTimer {
public:
    void SubmitProfile(ScopeProfile& profile);
    
    /// Safely retrieves a snapshot of current profiling results
    std::map<std::string, float> GetProfiles();
    
    /// Clear the frame timers for the next frame update loop
    void Reset();
    
private:
    std::map<std::string, float> mDeltaList;
    std::mutex mux;
};

#endif
