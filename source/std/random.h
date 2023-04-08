//
// Random number generation class
#ifndef _RANDOM_NUMBER_GENERATOR__
#define _RANDOM_NUMBER_GENERATOR__

#include <cstdlib>

struct RandomGen {
    
    void  Seed(int value);
    
    float Range(float min, float max);
    
    int Range(int min, int max);
    
};



#endif
