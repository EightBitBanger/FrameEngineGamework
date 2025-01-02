#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdatePanelUI(unsigned int index) {
    
    // Anchor RIGHT
    if (mStreamBuffer[index].panel->canvas.anchorRight) {
        mStreamBuffer[index].transform->position.z = Renderer.viewport.w + 
                                                     mStreamBuffer[index].panel->width * 
                                                     mStreamBuffer[index].panel->canvas.x;
        
    } else {
        
        // Anchor LEFT
        mStreamBuffer[index].transform->position.z  = (mStreamBuffer[index].panel->canvas.x * mStreamBuffer[index].panel->width);
        mStreamBuffer[index].transform->position.z += mStreamBuffer[index].panel->width;
        
        // Anchor CENTER horizontally
        if (mStreamBuffer[index].panel->canvas.anchorCenterHorz) 
            mStreamBuffer[index].transform->position.z = (Renderer.viewport.w / 2) + (mStreamBuffer[index].panel->canvas.x * mStreamBuffer[index].panel->width);
        
    }
    
    // Anchor TOP
    if (mStreamBuffer[index].panel->canvas.anchorTop) {
        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
        
        topAnchorTotal += (mStreamBuffer[index].panel->height * mStreamBuffer[index].panel->height) / 2;
        topAnchorTotal += mStreamBuffer[index].panel->height * mStreamBuffer[index].panel->canvas.y;
        
        mStreamBuffer[index].transform->position.y = topAnchorTotal;
        
    } else {
        
        // Anchor BOTTOM
        mStreamBuffer[index].transform->position.y  = Renderer.displaySize.y - mStreamBuffer[index].panel->height;
        mStreamBuffer[index].transform->position.y -= mStreamBuffer[index].panel->height * -(mStreamBuffer[index].panel->canvas.y);
        
        // Anchor CENTER vertically
        if (mStreamBuffer[index].panel->canvas.anchorCenterVert) {
            int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
            
            topAnchorTotal += (mStreamBuffer[index].panel->height * mStreamBuffer[index].panel->height) / 2;
            topAnchorTotal += (mStreamBuffer[index].panel->height * mStreamBuffer[index].panel->canvas.y) - (mStreamBuffer[index].panel->height * 2);
            
            mStreamBuffer[index].transform->position.y = topAnchorTotal;
        }
        
    }
    
    return;
}

