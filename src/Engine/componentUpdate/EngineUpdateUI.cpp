#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateUI(void) {
    
    float mouseX = Input.mouseX - Platform.clientArea.x;
    float mouseY = Input.mouseY - Platform.clientArea.y;
    
    float windowWidth  = Platform.windowArea.w;
    float windowHeight = Platform.windowArea.h;
    
    float mouseNormX = mouseX / windowWidth;
    float mouseNormY = mouseY / windowHeight;
    
    for (unsigned int i = 0; i < mButtons.Size(); i++) {
        
        Button* button = mButtons[i];
        
        if (!button->isActive) 
            continue;
        
        // Check button anchoring
        if (button->panelOverlay != nullptr) {
            
            if (button->panel->canvas.anchorCenterHorz) {
                
                glm::vec3 position = button->panelOverlay->GetPosition();
                
                Text* text = button->textObject->GetComponent<Text>();
                
                button->x = ((position.z / text->size) + text->x);
                button->y = ((position.x / text->size) - text->y);
                
                text->canvas.x = (button->panel->canvas.x / text->size) + ((position.z / text->size) + text->x);
                text->canvas.y = (button->panel->canvas.y / text->size) + ((position.x / text->size) - text->y);
                
            }
            
        }
        
        // Button parameters
        float xx = button->x / windowWidth;
        float yy = button->y / windowHeight;
        
        float ww = button->w / windowWidth;
        float hh = button->h / windowHeight;
        
        // Check hovered
        if ((mouseNormX > xx) && (mouseNormX < (xx + ww)) && 
            (mouseNormY > yy) && (mouseNormY < (yy + hh))) {
            
            button->isHovering = true;
            
        } else {
            
            button->isHovering = false;
        }
        
        // Call the button payload
        if (button->callback != nullptr) 
            button->callback(button);
        
    }
    
    // Reset input states
    Input.SetMouseLeftPressed(false);
    Input.SetMouseMiddlePressed(false);
    Input.SetMouseRightPressed(false);
    
    Input.SetMouseLeftReleased(false);
    Input.SetMouseMiddleReleased(false);
    Input.SetMouseRightReleased(false);
    
    return;
}


