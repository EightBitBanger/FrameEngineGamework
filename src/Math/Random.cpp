#include <GameEngineFramework/Math/Random.h>

#define STB_PERLIN_IMPLEMENTATION
#include "../../vendor/stb/stb_perlin.h"


NumberGeneration::NumberGeneration() :
    mSeed(100)
{
    
    srand(100);
    
    return;
}

void NumberGeneration::SetSeed(int seed) {
    
    srand( seed );
    
    mSeed = seed;
    
    return;
}

int NumberGeneration::GetSeed(void) {
    
    return mSeed;
}

int NumberGeneration::Range(int min, int max) {
    return (rand() % max) + min;
}

glm::vec3 NumberGeneration::RangedVector(int min, int max) {
    glm::vec3 vec(Range(min, max), 
                  Range(min, max), 
                  Range(min, max));
    return vec;
}


float NumberGeneration::Range(float min, float max) {
    return ((float)(rand() % (int)(max * 100.0f)) * 0.01f) + min;
}

double NumberGeneration::Range(double min, double max) {
    return ((double)(rand() % (int)(max * (double)100.0)) * (double)0.01) + min;
}

float NumberGeneration::Perlin(float xcoord, float ycoord, float zcoord, int seed) {
    return stb_perlin_noise3_seed(xcoord, ycoord, zcoord, 0, 0, 0, seed);
}



/*

#include <GameEngineFramework/Math/Random.h>

#define STB_PERLIN_IMPLEMENTATION
#include "../../vendor/stb/stb_perlin.h"

NumberGeneration::NumberGeneration() :
    mSeed(100)
{
    srand(100);
    return;
}

void NumberGeneration::SetSeed(int seed) {
    srand(seed);
    mSeed = seed;
    return;
}

int NumberGeneration::GetSeed(void) {
    return mSeed;
}

int NumberGeneration::Range(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}

float NumberGeneration::Range(float min, float max) {
    return ((float)rand() / (float)RAND_MAX) * (max - min) + min;
}

double NumberGeneration::Range(double min, double max) {
    return ((double)rand() / (double)RAND_MAX) * (max - min) + min;
}

float NumberGeneration::Perlin(float xcoord, float ycoord, float zcoord, int seed) {
    return stb_perlin_noise3_seed(xcoord, ycoord, zcoord, 0, 0, 0, seed);
}

*/
