#ifndef _CANVAS_UI_ELEMENT__
#define _CANVAS_UI_ELEMENT__


class Canvas {
    
public:
    
    bool anchorBottom;
    bool anchorRight;
    
    Vector2 position;
    
    float top;
    float left;
    
    float width;
    float height;
    
    Canvas() : 
        anchorBottom(false),
        anchorRight(false),
        
        position(Vector2(0, 0)),
        
        top(0),
        left(0),
        
        width(0),
        height(0)
    {
    }
    
};

#endif
