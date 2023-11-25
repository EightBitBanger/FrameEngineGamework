#ifndef TIMER_WINDOWS
#define TIMER_WINDOWS

#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <windows.h>


class __declspec(dllexport) Timer {
    
public:
    
    /// Time delta from the last frame.
    float  delta;
    
    /// Time delta scaled to units per second.
    float  units;
    
    /// Rate of update in milliseconds.
    float  updateRateMs;
    
    /// Maximum update in milliseconds.
    float  updateRateMax;
    
    /// Time interpolation factor between frames.
    float  interpolationFactor;
    
    Timer();
    
    /// Return the current time delta between frames.
    float GetCurrentDelta(void);
    
    /// Update and return the timer state.
    bool Update(void);
    
    /// Return the current time delta without resetting the delta.
    float Current(void);
    
    /// Set the update rate in milliseconds.
    void SetRefreshRate(int rate);
    
    
private:
    
    // Precision frequency adjustment
    double timeFrequency;
    
    // Previous frame time
    LARGE_INTEGER tLast;
    
    // Amount of accumulated frame time
    float accumulator;
    
};

#endif
