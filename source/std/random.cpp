#include "random.h"

void RandomGen::Seed(int value) {
    srand(value);
}

float  RandomGen::Range(float min, float max) {
    if (min >= max) return 1;
    return ((float)(rand() % (int)max) + min);
}

int  RandomGen::Range(int min, int max) {
    if (min >= max) return 1;
    return (rand() % max) + min;
}

