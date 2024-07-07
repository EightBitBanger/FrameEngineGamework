#include <GameEngineFramework/Timer/timer.h>

#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <windows.h>


Timer::Timer() {
    
    delta=0;
    units=0;
    accumulator=0;
    
    updateRateMs  = 1000.0 / 30.0;
    updateRateMax = updateRateMs + (updateRateMs / 4.0);
    
    interpolationFactor = 0;
    
    LARGE_INTEGER tFrequency;
    
    QueryPerformanceFrequency(&tFrequency);
    timeFrequency = tFrequency.QuadPart / 1000.0;
    
    LARGE_INTEGER integerLarge;
    integerLarge.QuadPart = tLast;
    
    QueryPerformanceCounter(&integerLarge);
    return;
}


double Timer::GetCurrentDelta(void) {
    
    LARGE_INTEGER tCurrent;
    QueryPerformanceCounter(&tCurrent);
    
    LARGE_INTEGER integerLarge;
    integerLarge.QuadPart = tLast;
    
    return (tCurrent.QuadPart - integerLarge.QuadPart) / timeFrequency;
}


bool Timer::Update(void) {
    
    LARGE_INTEGER tCurrent;
    QueryPerformanceCounter(&tCurrent);
    
    LARGE_INTEGER integerLarge;
    integerLarge.QuadPart = tLast;
    
    delta = (tCurrent.QuadPart - integerLarge.QuadPart) / timeFrequency;
    tLast = tCurrent.QuadPart;
    
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
    LARGE_INTEGER tCurrent;
    QueryPerformanceCounter(&tCurrent);
    return tCurrent.QuadPart;
}

void Timer::SetRefreshRate(int rate) {
    updateRateMs = 1000.0 / (double)rate;
    updateRateMax = updateRateMs + (updateRateMs / 4.0);
    return;
}

