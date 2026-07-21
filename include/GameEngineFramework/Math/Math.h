#ifndef _MATH_CORE__
#define _MATH_CORE__

#include <GameEngineFramework/configuration.h>


#ifndef __USE_MINGW_ANSI_STDIO
#define __USE_MINGW_ANSI_STDIO 1
#endif

//#include <math.h>
//#include <cmath>

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
    
    float Diff(float value, float target);
    
    float NormDiff(float a, float b, float max);
    
};

#endif
