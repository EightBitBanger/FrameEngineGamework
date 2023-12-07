#ifndef _ENGINE_UI_TEXT_COMPONENT__
#define _ENGINE_UI_TEXT_COMPONENT__

#include <GameEngineFramework/Engine/UI/canvas.h>
#include <GameEngineFramework/Engine/UI/sprite.h>
#include <GameEngineFramework/Engine/types/color.h>

#include <string>


class Text {
    
public:
    
    /// Text to be rendered.
    std::string text;
    
    /// Color of the text to be rendered.
    Color color;
    
    /// Size of the font.
    unsigned int size;
    
    /// Window anchoring layout.
    Canvas canvas;
    
    /// Font sub sprite atlas.
    Sprite sprite;
    
    // Text sprite layout
    
    /// Width between the sprite glyphs.
    float width;
    
    /// Height between the sprite glyphs.
    float height;
    
    /// Width of the sprite glyphs.
    float glyphWidth;
    
    /// Height of the sprite glyphs.
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
