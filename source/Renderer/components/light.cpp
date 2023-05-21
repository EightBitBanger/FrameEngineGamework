#include "light.h"

extern ColorPreset Colors;


Light::Light() {
    
    color = Colors.white;
    
    intensity   = 1;
    range       = 20;
    attenuation = 0;
    
}


