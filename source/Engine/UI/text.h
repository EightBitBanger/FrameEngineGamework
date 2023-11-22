#ifndef _ENGINE_UI_TEXT_COMPONENT__
#define _ENGINE_UI_TEXT_COMPONENT__

#include <string>

#include "canvas.h"
#include "sprite.h"
#include "../types/color.h"


class Text {
    
public:
    
    /// Text to be rendered.
    std::string text;
    
    /// Color of the text to be render.
    Color color;
    
    /// Image dimensions.
    Sprite sprite;
    
    /// Window anchoring layout.
    Canvas canvas;
    
    /// Sprite spacing width.
    float spacingWidth;
    
    /// Sprite spacing height.
    float spacingHeight;
    
    
    Text() : 
        text(""),
        
        spacingWidth(0.6),
        spacingHeight(0.9)
    {
        color = Color(0, 0, 0);
        return;
    }
    
    
};


#endif

