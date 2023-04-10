#include "random.h"

#undef RAND_MAX
#define RAND_MAX 100

#include <random>

void RandomGen::Seed(int value) {
    srand(value);
}

float  RandomGen::Range(float min, float max) {
    if (min == 0) min++;
    if (max == 0) max++;
    if (min >= max) return 1;
    return ((float)(rand() % (int)max) + min);
}

int  RandomGen::Range(int min, int max) {
    if (min == 0) min++;
    if (max == 0) max++;
    if (min >= max) return 1;
    return (rand() % max) + min;
}

RandomGen::RandomGen() {
    
    srand(100);
    
}

