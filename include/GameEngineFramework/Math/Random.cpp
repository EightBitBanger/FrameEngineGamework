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
    return (rand() % max) + min;
}

float  NumberGeneration::Range(float min, float max) {
    return ((float)(rand() % (int)(max * 100.0f)) * 0.01f) + min;
}

double  NumberGeneration::Range(double min, double max) {
    return ((double)(rand() % (int)(max * (double)100.0)) * (double)0.01) + min;
}

float NumberGeneration::Perlin(float xcoord, float ycoord, float zcoord) {
    return stb_perlin_noise3(xcoord, ycoord, zcoord, 0, 0, 0);
}
