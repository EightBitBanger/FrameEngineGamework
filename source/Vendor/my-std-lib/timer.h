#ifndef TIMER_WINDOWS
#define TIMER_WINDOWS

#include <windows.h>

class Timer {
    
public:
    
    float  delta;
    float  units;
    float  updateRateMs;
    float  updateRateMax;
    float  interpolationFactor;
    
    Timer();
    
    float GetCurrentDelta(void);
    
    bool Update(void);
    
    float Current(void);
    
    void SetRefreshRate(int rate);
    
    
private:
    
    double timeFrequency;
    LARGE_INTEGER tLast;
    
    float accumulator;
    
};

#endif
