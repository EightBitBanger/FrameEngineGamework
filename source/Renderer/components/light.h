#ifndef __COMPONENT_LIGHT
#define __COMPONENT_LIGHT

#include "../../engine/types/transform.h"
#include "../../engine/types/color.h"


class Light {
    
public:
    
    Transform transform;
    
    Color color;
    
    float intensity;
    float range;
    float attenuation;
    
    Light();
    
    
    
private:
    
    
    
    
    
    
};






#endif
