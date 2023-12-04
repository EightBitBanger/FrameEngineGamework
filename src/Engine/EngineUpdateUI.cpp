// Engine component related functions
//

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern RandomGen         Random;
ENGINE_API extern Logger            Log;
ENGINE_API extern Timer             PhysicsTime;
ENGINE_API extern Timer             Time;

ENGINE_API extern Serialization     Serializer;
ENGINE_API extern ResourceManager   Resources;
ENGINE_API extern ScriptSystem      Scripting;
ENGINE_API extern RenderSystem      Renderer;
ENGINE_API extern PhysicsSystem     Physics;
ENGINE_API extern AudioSystem       Audio;
ENGINE_API extern InputSystem       Input;
ENGINE_API extern MathCore          Math;
ENGINE_API extern ActorSystem       AI;

ENGINE_API extern ApplicationLayer      Application;
ENGINE_API extern EngineSystemManager   Engine;



void EngineSystemManager::UpdateUI(void) {
    
    //
    // Check mouse / button interaction
    //
    
    unsigned int windowMouseX = Input.mouseX - Application.windowLeft;
    unsigned int windowMouseY = Input.mouseY - Application.windowTop;
    
    for (unsigned int i=0; i < mButtons.Size(); i++) {
        Button* button = mButtons[i];
        
        bool leftActive   = false;
        bool middleActive = false;
        bool rightActive  = false;
        
        // Check button event
        if (button->triggerOnPressed) {
            if (Input.CheckMouseLeftPressed())   {leftActive   = true; Input.SetMouseLeftPressed(false);}
            if (Input.CheckMouseMiddlePressed()) {middleActive = true; Input.SetMouseMiddlePressed(false);}
            if (Input.CheckMouseRightPressed())  {rightActive  = true; Input.SetMouseRightPressed(false);}
        } else {
            if (Input.CheckMouseLeftReleased())   {leftActive   = true; Input.SetMouseLeftReleased(false);}
            if (Input.CheckMouseMiddleReleased()) {middleActive = true; Input.SetMouseMiddleReleased(false);}
            if (Input.CheckMouseRightReleased())  {rightActive  = true; Input.SetMouseRightReleased(false);}
        }
        
        // Button parameters
        unsigned int xx = button->x;
        unsigned int yy = button->y;
        
        unsigned int ww = button->w;
        unsigned int hh = button->h;
        
        // Check hovered
        if ((windowMouseX > xx) & (windowMouseX < (xx + ww)) & 
            (windowMouseY > yy) & (windowMouseY < (yy + hh))) {
            
            button->isHovering = true;
            
            // Get clicked state
            bool isClicked = false;
            if (button->triggerOnLeftButton)   if (leftActive)   isClicked = true;
            if (button->triggerOnMiddleButton) if (middleActive) isClicked = true;
            if (button->triggerOnRightButton)  if (rightActive)  isClicked = true;
            
            // Check clicked
            if (isClicked) {
                
                // Call the button payload
                if (button->callback != nullptr) 
                    button->callback();
                
            }
            
        } else {
            
            // Not hovering
            mButtons[i]->isHovering = false;
            
        }
        
    }
    
    return;
}
