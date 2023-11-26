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
    
    /// Color of the text to be rendered.
    Color color;
    
    /// Window anchoring layout.
    Canvas canvas;
    
    /// Font sub sprite atlas.
    Sprite sprite;
    
    /// Size of the font.
    unsigned int size;
    
    /// Sprite spacing width.
    float width;
    
    /// Sprite spacing height.
    float height;
    
    /// Width of the text glyphs on screen.
    float glyphWidth;
    
    /// Height of the text glyphs on screen.
    float glyphHeight;
    
    
    Text() : 
        text(""),
        
        size(0),
        
        width(0.5),
        height(0.9),
        
        glyphWidth(0.9),
        glyphHeight(0.9)
    {
        color = Color(0, 0, 0);
        return;
    }
    
    
};


#endif
