#ifndef TIMER_WINDOWS
#define TIMER_WINDOWS

#include <GameEngineFramework/configuration.h>

#include <SDL3/SDL.h>


class ENGINE_API Timer {
    
public:
    
    /// Time difference between frames.
    double  delta;
    
    /// Delta time scaled to seconds.
    double  units;
    
    /// Amount of accumulated frame time
    double accumulator;
    
    /// Rate of update in milliseconds.
    double  updateRateMs;
    
    /// Maximum update in milliseconds.
    double  updateRateMax;
    
    /// Time interpolation factor between frames.
    double  interpolationFactor;
    
    Timer();
    
    /// Return the current time delta between frames.
    double GetCurrentDelta(void);
    
    /// Update and return the timer state.
    bool Update(void);
    
    /// Return the current time delta without resetting the delta value.
    double Current(void);
    
    /// Set the update rate in milliseconds.
    void SetRefreshRate(int rate);
    
    
private:
    
    double timeLast;
    
};

#endif
