#ifndef BUTTON_UI_ELEMENT
#define BUTTON_UI_ELEMENT


struct Button {
    
    /// Button X position in the window.
    unsigned int x;
    
    /// Button Y position in the window.
    unsigned int y;
    
    /// Button width.
    unsigned int w;
    
    /// Button height.
    unsigned int h;
    
    /// (Read only) State whether the mouse is hovering over the button.
    bool isHovering;
    
    /// State whether this is a simple button element or a drag and drop element.
    bool isDragAndDrop;
    
    /// Trigger on the left mouse button event.
    bool triggerOnLeftButton;
    
    /// Trigger on the middle mouse button event.
    bool triggerOnMiddleButton;
    
    /// Trigger on the right mouse button event.
    bool triggerOnRightButton;
    
    /// Trigger when the mouse button is pressed.
    bool triggerOnPressed;
    
    /// Trigger when the mouse button is released.
    bool triggerOnRelease;
    
    /// Function pointer to an event callback.
    void(*callback)();
    
    Button() : 
        x(0),
        y(0),
        
        w(0),
        h(0),
        
        isHovering(false),
        
        triggerOnLeftButton(false),
        triggerOnMiddleButton(false),
        triggerOnRightButton(false),
        
        triggerOnPressed(false),
        triggerOnRelease(false),
        
        callback(nullptr)
    {
    }
    
};

#endif
