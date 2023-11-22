#ifndef __COMPONENT_LIGHT
#define __COMPONENT_LIGHT

#include "../../engine/types/transform.h"
#include "../../engine/types/color.h"


class __declspec(dllexport) Light {
    
public:
    
    bool isActive;
    
    float renderDistance;
    
    glm::vec3 position;
    glm::vec3 direction;
    
    Color color;
    
    float intensity;
    float range;
    float attenuation;
    
    Light();
    
    
    
private:
    
    
    
    
    
    
};






#endif
