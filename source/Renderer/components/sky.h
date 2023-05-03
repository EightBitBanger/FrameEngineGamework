#ifndef __COMPONENT_SKY
#define __COMPONENT_SKY

#include "../types/color.h"


struct Sky {
    
    /// The background view port color.
    Color background;
    
    Sky();
    
    /// Set the sky color.
    void SetColor(Color newColor);
    
};


#endif
