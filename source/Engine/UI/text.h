#ifndef _ENGINE_TEXT_COMPONENT__
#define _ENGINE_TEXT_COMPONENT__

#include <string>

#include "../types/color.h"


class Text {
    
public:
    
    /// Text to be rendered.
    std::string  text;
    
    /// Color of the text to be render.
    Color color;
    
    // Sprite atlas layout
    
    int spriteAtlasWidth = 15;
    int spriteAtlasHeight = 15;
    
    // Sub sprite  parameters
    
    float spriteWidth  = 0.03127;
    float spriteHeight = 0.0274;
    float spriteStartX = 0;
    float spriteStartY = -0.003;
    
    float spacingWidth  = 0.6;
    float spacingHeight = 0.9;
    
    
    Text();
    
    
};


#endif

