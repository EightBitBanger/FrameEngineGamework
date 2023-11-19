#ifndef RANDOM_NUMBER_GENERATOR
#define RANDOM_NUMBER_GENERATOR

#undef RAND_MAX
#define RAND_MAX 100


#include <cstdlib>
//#include <random>

class __declspec(dllexport) RandomGen {
    
public:
    
    RandomGen();
    
    /// Set the seed for the random number generator.
    void  Seed(int value);
    /// Return a float between the min and max values.
    float Range(float min, float max);
    /// Return an integer between the min and max values.
    int   Range(int min, int max);
    
    /// Return a perlin noise value based on the input location.
    float Perlin(float xcoord, float ycoord, float zcoord);
    
};

#endif
