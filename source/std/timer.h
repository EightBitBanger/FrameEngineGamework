//
// General purpose timer class


class Timer {
    
    double timeFrequency;
    LARGE_INTEGER tLast;
    
    float accumulator;
    
    
    public:
    
    float  delta;
    float  units;
    float  updateRateMs;
    float  updateRateMax;
    float  interpolationFactor;
    
    Timer() {
        
        delta=0;
        units=0;
        accumulator=0;
        
        updateRateMs  = 30 / 1000;
        updateRateMax = updateRateMs + (updateRateMs / 4);
        
        interpolationFactor = 0;
        
        LARGE_INTEGER tFrequency;
        
        QueryPerformanceFrequency(&tFrequency);
        timeFrequency = tFrequency.QuadPart / 1000;
        
        QueryPerformanceCounter(&tLast);
    }
    
    
    float GetCurrentDelta(void) {
        
        LARGE_INTEGER tCurrent;
        QueryPerformanceCounter(&tCurrent);
        
        return (tCurrent.QuadPart - tLast.QuadPart) / timeFrequency;
    }
    
    
    bool Update(void) {
        
        LARGE_INTEGER tCurrent;
        QueryPerformanceCounter(&tCurrent);
        
        delta = (tCurrent.QuadPart - tLast.QuadPart) / timeFrequency;
        tLast = tCurrent;
        
        accumulator += delta;
        
        if (accumulator >= updateRateMs) {
            
            if (accumulator > updateRateMax) accumulator = updateRateMs;
            
            delta = accumulator / 1000;
            units = accumulator;
            
            accumulator -= updateRateMs;
            
            interpolationFactor = accumulator / updateRateMs;
            
            return true;
        }
        
        return false;
    }
    
    float Current(void) {
        LARGE_INTEGER tCurrent;
        QueryPerformanceCounter(&tCurrent);
        return tCurrent.QuadPart;
    }
    
    void SetRefreshRate(int rate) {
        updateRateMs = 1000 / rate;
        updateRateMax = updateRateMs + (updateRateMs / 4);
    }
    
};


