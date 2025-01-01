#include <GameEngineFramework/Timer/timer.h>

Timer::Timer() : 
    delta(0),
    units(0),
    accumulator(0),
    updateRateMs(1000.0 / 30.0),
    updateRateMax(updateRateMs + (updateRateMs / 4.0)),
    
#ifdef PLATFORM_WINDOWS
    
    interpolationFactor(0),
    timeFrequency(),
    tLast(0)
    
#endif
    
#ifdef PLATFORM_LINUX
    
    interpolationFactor(0)
    //startTime( std::chrono::high_resolution_clock::now() )
    
#endif
    
{
    
#ifdef PLATFORM_WINDOWS
    
    LARGE_INTEGER tFrequency;
    
    QueryPerformanceFrequency(&tFrequency);
    timeFrequency = tFrequency.QuadPart / 1000.0;
    
#endif
    
    return;
}


double Timer::GetCurrentDelta(void) {
    
#ifdef PLATFORM_WINDOWS
    
    LARGE_INTEGER tCurrent;
    QueryPerformanceCounter(&tCurrent);
    
    LARGE_INTEGER integerLarge;
    integerLarge.QuadPart = tLast;
    
    return (tCurrent.QuadPart - integerLarge.QuadPart) / timeFrequency;
    
#endif
    
#ifdef PLATFORM_LINUX
    
    std::chrono::time_point<std::chrono::high_resolution_clock> current = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = current - startTime;
    
    return duration.count();
    
#endif
    
    
}


bool Timer::Update(void) {
    
#ifdef PLATFORM_WINDOWS
    
    LARGE_INTEGER tCurrent;
    QueryPerformanceCounter(&tCurrent);
    
    LARGE_INTEGER integerLarge;
    integerLarge.QuadPart = tLast;
    
    delta = (tCurrent.QuadPart - integerLarge.QuadPart) / timeFrequency;
    tLast = tCurrent.QuadPart;
    
#endif
    
#ifdef PLATFORM_LINUX
    
    std::chrono::time_point<std::chrono::high_resolution_clock> current = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = current - startTime;
    
    delta = duration.count();
    
    //startTime = std::chrono::high_resolution_clock::now();
    
#endif
    
    accumulator += delta;
    
    if (accumulator >= updateRateMs) {
        
        if (accumulator > updateRateMax) 
            accumulator = updateRateMax;
        
        accumulator -= updateRateMs;
        
        units = accumulator * 1000;
        
        interpolationFactor = accumulator / updateRateMs;
        
        return true;
    }
    
    return false;
}

double Timer::Current(void) {
    
#ifdef PLATFORM_WINDOWS
    
    LARGE_INTEGER tCurrent;
    QueryPerformanceCounter(&tCurrent);
    
    return tCurrent.QuadPart;
    
#endif
    
#ifdef PLATFORM_LINUX
    
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = now - startTime;
    
    return duration.count();
    
#endif
    
}

void Timer::SetRefreshRate(int rate) {
    updateRateMs = 1000.0 / (double)rate;
    updateRateMax = updateRateMs + (updateRateMs / 4.0);
    return;
}

/*

#include <GameEngineFramework/Timer/timer.h>
#include <chrono>

Timer::Timer() : 
    delta(0),
    units(0),
    accumulator(0),
    updateRateMs(1000.0 / 30.0),
    updateRateMax(updateRateMs + (updateRateMs / 4.0)),
    interpolationFactor(0),
    startTime(std::chrono::high_resolution_clock::now())
{
}

double Timer::GetCurrentDelta(void) {
    std::chrono::time_point<std::chrono::high_resolution_clock> current = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = current - startTime;
    return duration.count();
}

bool Timer::Update(void) {
    std::chrono::time_point<std::chrono::high_resolution_clock> current = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = current - startTime;
    delta = duration.count();
    startTime = current;

    accumulator += delta;

    if (accumulator >= updateRateMs) {
        if (accumulator > updateRateMax) 
            accumulator = updateRateMax;
        accumulator -= updateRateMs;
        units = accumulator * 1000;
        interpolationFactor = accumulator / updateRateMs;
        return true;
    }
    return false;
}

double Timer::Current(void) {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = now - startTime;
    return duration.count();
}

void Timer::SetRefreshRate(int rate) {
    updateRateMs = 1000.0 / static_cast<double>(rate);
    updateRateMax = updateRateMs + (updateRateMs / 4.0);
}


*/
