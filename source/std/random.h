//
// Random number generation class
#ifndef _RANDOM_NUMBER_GENERATOR__
#define _RANDOM_NUMBER_GENERATOR__

#include <cstdlib>

struct RandomGen {
    
    RandomGen();
    
    static void  Seed(int value);
    
    static float Range(float min, float max);
    
    static int Range(int min, int max);
    
};

#endif
