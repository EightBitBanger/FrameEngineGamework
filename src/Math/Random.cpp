#include <GameEngineFramework/Math/Random.h>

#define STB_PERLIN_IMPLEMENTATION
#include "../../vendor/stb/stb_perlin.h"


RandomGen::RandomGen() {
    
    srand(100);
    return;
}

void  RandomGen::Seed(int value) {
    srand(value);
    stb_perlin_noise3_seed(0, 0, 0, 0, 0, 0, value);
    return;
}

int  RandomGen::Range(int min, int max) {
    if (min >= max) return 0;
    return (rand() % (max)) + min;
}

float  RandomGen::Range(float min, float max) {
    if (min >= max) return 0;
    return ((float)(rand() % (int)max) + min);
}

double  RandomGen::Range(double min, double max) {
    if (min >= max) return 0;
    return ((double)(rand() % (int)max) + min);
}

float RandomGen::Perlin(float xcoord, float ycoord, float zcoord) {
    return stb_perlin_noise3(xcoord, ycoord, zcoord, 0, 0, 0);
}
