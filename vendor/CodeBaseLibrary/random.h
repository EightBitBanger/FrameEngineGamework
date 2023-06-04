#ifndef RANDOM_NUMBER_GENERATOR
#define RANDOM_NUMBER_GENERATOR

#include <cstdlib>

class RandomGen {
    
public:
    
    RandomGen();
    
    void  Seed(int value);
    float Range(float min, float max);
    int   Range(int min, int max);
    
};

#endif
