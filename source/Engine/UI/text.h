#ifndef _ENGINE_TEXT_COMPONENT__
#define _ENGINE_TEXT_COMPONENT__

#include <string>

#include "canvas.h"
#include "../types/color.h"


class Text {
    
public:
    
    /// Text to be rendered.
    std::string  text;
    
    /// Color of the text to be render.
    Color color;
    
    // Sprite atlas layout
    int spriteAtlasWidth;
    int spriteAtlasHeight;
    
    // Sub sprite parameters
    float spriteStartX;
    float spriteStartY;
    float spriteWidth;
    float spriteHeight;
    
    // Character spacing
    float spacingWidth;
    float spacingHeight;
    
    // Window anchoring
    Canvas canvas;
    
    Text() : 
        
        text(""),
        
        spriteAtlasWidth(15),
        spriteAtlasHeight(15),
        
        spriteStartX(0),
        spriteStartY(-0.003),
        spriteWidth(0.03127),
        spriteHeight(0.0274),
        
        spacingWidth(0.6),
        spacingHeight(0.9)
    {
        color = Color(0, 0, 0);
        return;
    }
    
    
};


#endif

