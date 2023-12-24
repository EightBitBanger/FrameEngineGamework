#include <GameEngineFramework/Math/Random.h>

#define STB_PERLIN_IMPLEMENTATION
#include "../../vendor/stb/stb_perlin.h"


NumberGeneration::NumberGeneration() {
    
    srand(100);
    return;
}

void  NumberGeneration::Seed(int value) {
    srand(value);
    stb_perlin_noise3_seed(0, 0, 0, 0, 0, 0, value);
    return;
}

int  NumberGeneration::Range(int min, int max) {
    if (min >= max) return 0;
    return (rand() % (max)) + min;
}

float  NumberGeneration::Range(float min, float max) {
    if (min >= max) return 0;
    return ((float)(rand() % (int)max) + min);
}

double  NumberGeneration::Range(double min, double max) {
    if (min >= max) return 0;
    return ((double)(rand() % (int)max) + min);
}

float NumberGeneration::Perlin(float xcoord, float ycoord, float zcoord) {
    return stb_perlin_noise3(xcoord, ycoord, zcoord, 0, 0, 0);
}
