// Engine component related functions
//

#include "engine.h"

__declspec(dllexport) extern EngineComponents  Components;
__declspec(dllexport) extern ColorPreset       Colors;
__declspec(dllexport) extern RandomGen         Random;
__declspec(dllexport) extern Logger            Log;
__declspec(dllexport) extern Timer             PhysicsTime;
__declspec(dllexport) extern Timer             Time;

__declspec(dllexport) extern Serialization     Serializer;
__declspec(dllexport) extern ResourceManager   Resources;
__declspec(dllexport) extern ScriptSystem      Scripting;
__declspec(dllexport) extern RenderSystem      Renderer;
__declspec(dllexport) extern PhysicsSystem     Physics;
__declspec(dllexport) extern AudioSystem       Audio;
__declspec(dllexport) extern InputSystem       Input;
__declspec(dllexport) extern MathCore          Math;
__declspec(dllexport) extern ActorSystem       AI;

__declspec(dllexport) extern ApplicationLayer      Application;
__declspec(dllexport) extern EngineSystemManager   Engine;



void EngineSystemManager::UpdateUI(void) {
    
    //
    // Check mouse / button interaction
    //
    
    int windowMouseX = Input.mouseX - Renderer.viewport.x;
    int windowMouseY = Input.mouseY - Renderer.viewport.y;
    
    for (unsigned int i=0; i < mButtons.Size(); i++) {
        Button* button = mButtons[i];
        
        // Check butt event
        bool leftActive   = false;
        bool middleActive = false;
        bool rightActive  = false;
        
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
        int xx = button->x;
        int yy = button->y;
        
        int ww = button->w;
        int hh = button->h;
        
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
