#include "light.h"

extern ColorPreset Colors;


Light::Light() {
    
    color = Colors.white;
    
    intensity   = 0;
    range       = 0;
    attenuation = 0;
    
}


