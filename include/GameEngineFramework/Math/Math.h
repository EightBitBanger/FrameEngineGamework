#ifndef _MATH_CORE__
#define _MATH_CORE__

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/Math/Random.h>

// Not this "math.h"
#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>

typedef glm::vec2 Vector2;
typedef glm::vec3 Vector3;


class ENGINE_API MathCore {
    
public:
    
    float Lerp(float min, float max, float bias);
    
    glm::vec3 Lerp(glm::vec3 min, glm::vec3 max, float bias);
    
    float Round(float value);
    
};

#endif
