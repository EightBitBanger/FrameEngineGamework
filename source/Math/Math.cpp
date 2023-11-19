#include "math.h"

float MathCore::Lerp(float min, float max, float bias) {
    return glm::lerp(min, max, bias);
}

float MathCore::Round(float value) {
    return round(value);
}
