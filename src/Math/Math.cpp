#include <GameEngineFramework/Math/Math.h>

float MathCore::Lerp(float min, float max, float bias) {
    return glm::lerp(min, max, bias);
}

glm::vec3 MathCore::Lerp(glm::vec3 min, glm::vec3 max, float bias) {
    glm::vec3 vec( glm::lerp(min.x, max.x, bias), 
                   glm::lerp(min.y, max.y, bias), 
                   glm::lerp(min.z, max.z, bias) );
    return vec;
}

float MathCore::Round(float value) {
    return round(value);
}

float MathCore::Diff(float value, float target) {
    return std::abs(value - target);
}

float MathCore::NormDiff(float a, float b, float max) {
    if (a == b) return 0.0f;
    return (a - b) / max;
}
