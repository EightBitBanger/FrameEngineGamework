#include "light.h"

extern ColorPreset Colors;


Light::Light() {
    
    color = Colors.white;
    
    intensity   = 0.87;
    range       = 30;
    attenuation = 0;
    
}


