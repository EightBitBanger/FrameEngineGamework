#include <GameEngineFramework/Math/Random.h>

#define STB_PERLIN_IMPLEMENTATION
#include "../../vendor/stb/stb_perlin.h"


NumberGeneration::NumberGeneration() :
    mSeed(100),
    mGenerator(100)
{
    return;
}

void NumberGeneration::SetSeed(int seed) {
    mGenerator.seed(seed);
    mSeed = seed;
    return;
}

int NumberGeneration::GetSeed(void) {
    return mSeed;
}

int NumberGeneration::Range(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max - 1);
    return dist(mGenerator);
}

unsigned int NumberGeneration::Range(unsigned int min, unsigned int max) {
    std::uniform_int_distribution<int> dist(min, max - 1);
    return static_cast<unsigned int>( dist(mGenerator) );
}

glm::vec3 NumberGeneration::RangedVector(int min, int max) {
    glm::vec3 vec(Range(min, max), 
                  Range(min, max), 
                  Range(min, max));
    return vec;
}


float NumberGeneration::Range(float min, float max) {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(mGenerator);
}

double NumberGeneration::Range(double min, double max) {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(mGenerator);
}

float NumberGeneration::Perlin(float xcoord, float ycoord, float zcoord, int seed) {
    return stb_perlin_noise3_seed(xcoord, ycoord, zcoord, 0, 0, 0, seed);
}

