#ifndef _ENGINE_UI_SPRITE_COMPONENT__
#define _ENGINE_UI_SPRITE_COMPONENT__

#include <string>

#include "canvas.h"


class Sprite {
    
public:
    
    /// Width of the main texture.
    int textureWidth;
    
    /// Height of the main texture.
    int textureHeight;
    
    // Sub sprite atlas layout
    
    /// Starting X offset position to start rendering from the main texture.
    float spriteStartX;
    /// Starting Y offset position to start rendering from the main texture.
    float spriteStartY;
    /// Width to render from the main texture.
    float spriteWidth;
    /// Height to render from the main texture.
    float spriteHeight;
    
    /// Window anchoring
    Canvas canvas;
    
    
    Sprite() : 
        textureWidth(15),
        textureHeight(15),
        
        spriteStartX(0),
        spriteStartY(-0.003),
        spriteWidth(0.03127),
        spriteHeight(0.0274)
    {
    }
    
    
};

#endif
