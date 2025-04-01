#ifndef BUTTON_UI_ELEMENT
#define BUTTON_UI_ELEMENT


class Button {
    
public:
    
    /// Is the button currently active in the scene.
    bool isActive;
    
    /// Button X position in the window.
    float x;
    
    /// Button Y position in the window.
    float y;
    
    /// Button width.
    float w;
    
    /// Button height.
    float h;
    
    /// (Read only) State whether the mouse is hovering over the button.
    bool isHovering;
    
    /// Associated panel element.
    Panel* panel;
    
    /// Function pointer to an event callback.
    void(*callback)(Button*);
    
    /// Pointer to the associated game objects
    GameObject* panelOverlay;
    GameObject* textObject;
    
    /// Activate the button, showing it and enabling its callback.
    void Activate(void) {
        
        isActive = true;
        
        panelOverlay->Activate();
        
        textObject->Activate();
        
        return;
    }
    
    /// Deactivate the button, hiding it and ignoring its callback.
    void Deactivate(void) {
        
        isActive = false;
        
        panelOverlay->Deactivate();
        
        textObject->Deactivate();
        
        return;
    }
    
    Button() : 
        isActive(true),
        x(0),
        y(0),
        
        w(16),
        h(16),
        
        isHovering(false),
        
        panel(nullptr),
        
        callback(nullptr),
        panelOverlay(nullptr),
        textObject(nullptr)
        
    {
    }
    
};

#endif
