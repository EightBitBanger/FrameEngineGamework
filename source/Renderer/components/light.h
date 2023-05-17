#ifndef __COMPONENT_LIGHT
#define __COMPONENT_LIGHT

#include "../types/transform.h"
#include "../types/color.h"


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
