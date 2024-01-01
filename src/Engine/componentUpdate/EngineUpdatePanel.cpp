//
// Update panel allignment UI components

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
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


void EngineSystemManager::UpdatePanelUI(unsigned int index) {
    
    // Anchor RIGHT
    
    if (streamBuffer[index].panel->canvas.anchorRight) {
        streamBuffer[index].gameObject->mTransformCache->position.z = Renderer.viewport.w + 
                                                                  streamBuffer[index].panel->width * 
                                                                  streamBuffer[index].panel->canvas.y;
        
    } else {
        
        //
        // Anchor LEFT by default
        //
        
        streamBuffer[index].gameObject->mTransformCache->position.z  = (streamBuffer[index].panel->canvas.y * streamBuffer[index].panel->width);
        streamBuffer[index].gameObject->mTransformCache->position.z += streamBuffer[index].panel->width;
        
        //
        // Anchor CENTER horizontally
        //
        
        if (streamBuffer[index].panel->canvas.anchorCenterHorz) {
            
            streamBuffer[index].gameObject->mTransformCache->position.z = (Renderer.viewport.w / 2) + (streamBuffer[index].panel->canvas.y * streamBuffer[index].panel->width);
            
        }
        
    }
    
    //
    // Anchor TOP
    //
    
    if (streamBuffer[index].panel->canvas.anchorTop) {
        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
        
        topAnchorTotal += (streamBuffer[index].panel->height * streamBuffer[index].panel->height) / 2;
        topAnchorTotal += streamBuffer[index].panel->height * streamBuffer[index].panel->canvas.x;
        
        streamBuffer[index].gameObject->mTransformCache->position.y = topAnchorTotal;
    } else {
        
        //
        // Anchor BOTTOM by default
        //
        
        streamBuffer[index].gameObject->mTransformCache->position.y  = Renderer.displaySize.y - streamBuffer[index].panel->height;
        streamBuffer[index].gameObject->mTransformCache->position.y -= streamBuffer[index].panel->height * -(streamBuffer[index].panel->canvas.x);
        
        //
        // Anchor CENTER vertically
        //
        
        if (streamBuffer[index].panel->canvas.anchorCenterVert) {
            int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
            
            topAnchorTotal += (streamBuffer[index].panel->height * streamBuffer[index].panel->height) / 2;
            topAnchorTotal += (streamBuffer[index].panel->height * streamBuffer[index].panel->canvas.x) - (streamBuffer[index].panel->height * 2);
            
            streamBuffer[index].gameObject->mTransformCache->position.y = topAnchorTotal;
        }
        
    }
    
    return;
}

