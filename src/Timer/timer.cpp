#include <GameEngineFramework/Timer/timer.h>

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
    
    QueryPerformanceCounter(&tLast);
    return;
}


float Timer::GetCurrentDelta(void) {
    
    LARGE_INTEGER tCurrent;
    QueryPerformanceCounter(&tCurrent);
    
    return (tCurrent.QuadPart - tLast.QuadPart) / timeFrequency;
}


bool Timer::Update(void) {
    
    LARGE_INTEGER tCurrent;
    QueryPerformanceCounter(&tCurrent);
    
    delta = (tCurrent.QuadPart - tLast.QuadPart) / timeFrequency;
    tLast = tCurrent;
    
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

float Timer::Current(void) {
    LARGE_INTEGER tCurrent;
    QueryPerformanceCounter(&tCurrent);
    return tCurrent.QuadPart;
}

void Timer::SetRefreshRate(int rate) {
    updateRateMs = 1000.0 / (float)rate;
    updateRateMax = updateRateMs + (updateRateMs / 4.0);
    return;
}

