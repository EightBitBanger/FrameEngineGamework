#ifndef _BUTTON_UI_ELEMENT__
#define _BUTTON_UI_ELEMENT__

#include "canvas.h"


class Button {
    
public:
    
    /// Horizontal position in the window in pixels.
    unsigned int x;
    
    /// Vertical position in the window in pixels.
    unsigned int y;
    
    /// Width of the button in pixels.
    unsigned int width;
    
    /// Height of the button in pixels.
    unsigned int height;
    
    /// Window anchoring layout.
    Canvas canvas;
    
    
    Button() : 
        x(0),
        y(0),
        
        width(0),
        height(0)
    {
    }
    
};

#endif
