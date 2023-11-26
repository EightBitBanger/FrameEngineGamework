#ifndef _CANVAS_UI_ELEMENT__
#define _CANVAS_UI_ELEMENT__


class Canvas {
    
public:
    
    /// Anchor the canvas to the top of the window.
    bool anchorTop;
    
    /// Anchor the canvas to the right size of the window.
    bool anchorRight;
    
    /// Center the canvas horizontally in the window.
    bool anchorCenterHorz;
    
    /// Center the canvas vertically in the window.
    bool anchorCenterVert;
    
    /// The x position of the canvas relative to the window.
    unsigned int x;
    
    /// The y position of the canvas relative to the window.
    unsigned int y;
    
    
    Canvas() : 
        anchorTop(false),
        anchorRight(false),
        
        anchorCenterHorz(false),
        anchorCenterVert(false),
        
        x(0),
        y(0)
    {
    }
    
};

#endif
