#ifndef _MATH_CORE__
#define _MATH_CORE__

#include <GameEngineFramework/configuration.h>

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/compatibility.hpp>


class ENGINE_API MathCore {
    
public:
    
    float Lerp(float min, float max, float bias);
    
    float Round(float value);
    
};

#endif
