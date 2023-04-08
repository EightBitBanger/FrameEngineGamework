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
    
    Timer();
    
    float GetCurrentDelta(void);
    
    bool Update(void);
    
    float Current(void);
    
    void SetRefreshRate(int rate);
    
};

