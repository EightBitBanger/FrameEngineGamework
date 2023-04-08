//
// Random number generation class
#ifndef _RANDOM_NUMBER_GENERATOR__
#define _RANDOM_NUMBER_GENERATOR__

#include <cstdlib>

struct RandomGen {
    
    void  Seed(uint32_t value)  {srand(value);};
    void  Seed(float value)     {srand((uint32_t)value);};
    
    float  Range(float min, float max) {
        if (min >= max) return 1;
        return ((float)(rand() % (int)max) + min);
    }
    
    int  Range(int min, int max) {
        if (min >= max) return 1;
        return (rand() % max) + min;
    }
    
};



#endif
