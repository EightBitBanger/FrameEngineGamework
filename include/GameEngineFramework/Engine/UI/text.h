#ifndef _ENGINE_UI_TEXT_COMPONENT__
#define _ENGINE_UI_TEXT_COMPONENT__

#include <GameEngineFramework/Engine/UI/canvas.h>
#include <GameEngineFramework/Engine/UI/sprite.h>
#include <GameEngineFramework/Engine/types/color.h>

#include <string>


class Text {
    
public:
    
    friend class EngineSystemManager;
    
    /// Offset X position of the text.
    float x;
    
    /// Offset Y position of the text.
    float y;
    
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
        x(0),
        y(0),
        
        text(""),
        
        size(0),
        
        width(0.5f),
        height(0.9f),
        
        glyphWidth(0.9f),
        glyphHeight(0.9f),
        
        mCurrentText("")
    {
        color = Color(0, 0, 0);
        return;
    }
    
private:
    
    // Current state of the text string
    std::string mCurrentText;
    
};


#endif
