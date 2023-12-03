#ifndef _ENGINE_UI_SPRITE_COMPONENT__
#define _ENGINE_UI_SPRITE_COMPONENT__


class Sprite {
    
public:
    
    /// Width of the sprite texture.
    int width;
    
    /// Height of the sprite texture.
    int height;
    
    // Sub sprite atlas layout
    
    /// Starting X offset position to start rendering from the main texture.
    float subSpriteX;
    
    /// Starting Y offset position to start rendering from the main texture.
    float subSpriteY;
    
    /// Width to render from the main texture.
    float subSpriteWidth;
    
    /// Height to render from the main texture.
    float subSpriteHeight;
    
    
    Sprite() : 
        width(15),
        height(15),
        
        subSpriteX(0),
        subSpriteY(-0.003),
        subSpriteWidth(0.03127),
        subSpriteHeight(0.0274)
    {
    }
    
    
};

#endif
