//
// Random number generation class
#ifndef _RANDOM_NUMBER_GENERATOR__
#define _RANDOM_NUMBER_GENERATOR__

#include <cstdlib>

struct RandomGen {
    
    void  Seed(uint32_t value)  {srand(value);};
    void  Seed(float value)     {srand((uint32_t)value);};
    
    float  Range(float min, float max) {if (max>min) {return (float)(rand() % (int)max) + (min / 2);}else {return 1.0;}}
    int    Range(int min, int max) {return (rand() % max) + (min / 2);}
    
};



#endif
