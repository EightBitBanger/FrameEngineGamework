#include <GameEngineFramework/Timer/timer.h>

Timer::Timer() : 
    delta(0),
    units(0),
    accumulator(0),
    updateRateMs(1000.0 / 30.0),
    updateRateMax(updateRateMs + (updateRateMs / 4.0)),
    
    interpolationFactor(0),
    timeLast(0)
{
}


double Timer::GetCurrentDelta(void) {
    
    return (SDL_GetTicksNS() / 1000000.0f) - timeLast;
}


bool Timer::Update(void) {
    double timeCurrent = SDL_GetTicksNS() / 1000000.0f;
    delta = timeCurrent - timeLast;
    timeLast = timeCurrent;
    
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
    
    return SDL_GetTicksNS() / 1000000.0f;
}

void Timer::SetRefreshRate(int rate) {
    updateRateMs = 1000.0 / (double)rate;
    updateRateMax = updateRateMs + (updateRateMs / 4.0);
    return;
}

