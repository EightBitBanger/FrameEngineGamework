#ifndef _PANEL_UI_ELEMENT__
#define _PANEL_UI_ELEMENT__

#include <GameEngineFramework/Engine/UI/canvas.h>


class Panel {
    
public:
    
    /// Horizontal position in the window in pixels.
    int x;
    
    /// Vertical position in the window in pixels.
    int y;
    
    /// Width of the panel in pixels.
    int width;
    
    /// Height of the panel in pixels.
    int height;
    
    /// Window anchoring layout.
    Canvas canvas;
    
    
    Panel() : 
        x(0),
        y(0),
        
        width(1),
        height(1)
    {
    }
    
};

#endif
