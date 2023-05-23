#include "light.h"

extern ColorPreset Colors;


Light::Light() : 
    
    color(Colors.white),
    
    intensity(100),
    range(300),
    attenuation(0.008) 
{
}

