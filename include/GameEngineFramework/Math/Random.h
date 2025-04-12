#ifndef _RANDOM_NUMBER_GENERATION__
#define _RANDOM_NUMBER_GENERATION__

#include <GameEngineFramework/configuration.h>
#include <glm/glm.hpp>

class ENGINE_API NumberGeneration {
    
public:
    
    NumberGeneration();
    
    /// Set the seed for the random number generator.
    void SetSeed(int value);
    
    /// Get the seed from the random number generator.
    int GetSeed(void);
    
    /// Return a float between the min and max values.
    float Range(float min, float max);
    
    /// Return a double between the min and max values.
    double Range(double min, double max);
    
    /// Return an integer between the min and max values.
    int Range(int min, int max);
    
    /// Return a vector 3 with each element ranging between the min and max values.
    glm::vec3 RangedVector(int min, int max);
    
    /// Return a perlin noise value based on the input coordinates.
    float Perlin(float xcoord, float ycoord, float zcoord, int seed);
    
private:
    
    int mSeed;
};

#endif
