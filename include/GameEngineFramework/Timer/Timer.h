#ifndef TIMER_WINDOWS
#define TIMER_WINDOWS

#include <GameEngineFramework/configuration.h>


#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <windows.h>


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
    
    // Precision frequency adjustment
    double timeFrequency;
    
    // Previous frame time
    LARGE_INTEGER tLast;
    
};

#endif
