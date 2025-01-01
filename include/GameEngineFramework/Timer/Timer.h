#ifndef TIMER_WINDOWS
#define TIMER_WINDOWS

#include <GameEngineFramework/configuration.h>

#ifdef PLATFORM_WINDOWS
 
 #ifndef _WIN32_WINNT
 #define _WIN32_WINNT 0x500
 #endif
 
 #define WIN32_LEAN_AND_MEAN
 
 #include <sdkddkver.h>
 #include <windows.h>
 
#endif

#ifdef PLATFORM_LINUX
 
 #include <chrono>
 #include <unistd.h>
 
#endif


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
    
#ifdef PLATFORM_WINDOWS
    
    double timeFrequency;
    long long int tLast;
    
#endif
    
#ifdef PLATFORM_LINUX
    
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    
#endif
    
};

#endif





/*

#ifndef TIMER_WINDOWS
#define TIMER_WINDOWS

#include <GameEngineFramework/configuration.h>
#include <chrono>

class ENGINE_API Timer {
    
public:
    
    /// Time difference between frames.
    double delta;
    
    /// Delta time scaled to seconds.
    double units;
    
    /// Amount of accumulated frame time
    double accumulator;
    
    /// Rate of update in milliseconds.
    double updateRateMs;
    
    /// Maximum update in milliseconds.
    double updateRateMax;
    
    /// Time interpolation factor between frames.
    double interpolationFactor;
    
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
    
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    
};

#endif



*/
