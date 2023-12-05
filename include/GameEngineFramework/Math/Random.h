#ifndef _RANDOM_NUMBER_GENERATION__
#define _RANDOM_NUMBER_GENERATION__

#include <GameEngineFramework/configuration.h>

class ENGINE_API RandomGen {
    
public:
    
    RandomGen();
    
    /// Set the seed for the random number generator.
    void Seed(int value);
    
    /// Return a float between the min and max values.
    float Range(float min, float max);
    
    /// Return a double between the min and max values.
    double Range(double min, double max);
    
    /// Return an integer between the min and max values.
    int Range(int min, int max);
    
    /// Return a perlin noise value based on the input coordinates.
    float Perlin(float xcoord, float ycoord, float zcoord);
    
};

#endif
