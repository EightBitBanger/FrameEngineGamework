#include <GameEngineFramework/Profiler/profiler.h>
#include <SDL3/SDL.h>

void ProfilerTimer::SubmitProfile(ScopeProfile& profile) {
    Uint64 endTimeNS = SDL_GetTicksNS();
    
    // Convert nanosecond difference to milliseconds in 64-bit precision
    double elapsedMs = static_cast<double>(endTimeNS - profile.mStartTimeNS) / 1000000.0;
    
    std::lock_guard<std::mutex> lock(mux);
    // Accumulate time so multiple iterations in a frame add together
    mDeltaList[profile.mName] += static_cast<float>(elapsedMs);
}

std::map<std::string, float> ProfilerTimer::GetProfiles() {
    std::lock_guard<std::mutex> lock(mux);
    return mDeltaList;
}

void ProfilerTimer::Reset() {
    std::lock_guard<std::mutex> lock(mux);
    for (std::pair<const std::string, float>& pair : mDeltaList) {
        pair.second = 0.0f;
    }
}
