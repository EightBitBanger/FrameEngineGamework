//
// Update text UI components

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


void EngineSystemManager::UpdateTextUI(unsigned int index) {
    
    if (streamBuffer[index].meshRenderer == nullptr) 
        return;
    
    
    //
    // Anchor RIGHT
    
    if (streamBuffer[index].text->canvas.anchorRight) {
        streamBuffer[index].gameObject->mTransformCache->position.z = Renderer.viewport.w + 
                                                                  streamBuffer[index].text->size * 
                                                                  streamBuffer[index].text->canvas.x;
        
        // Keep text on screen when anchored right
        streamBuffer[index].gameObject->mTransformCache->position.z -= streamBuffer[index].text->text.size() * // length of string
                                                                   streamBuffer[index].text->size;         // Size of font text
        
    } else {
        
        // Anchor LEFT by default
        streamBuffer[index].gameObject->mTransformCache->position.z  = (streamBuffer[index].text->canvas.x * streamBuffer[index].text->size);
        streamBuffer[index].gameObject->mTransformCache->position.z += streamBuffer[index].text->size;
        
        // Anchor CENTER horizontally
        if (streamBuffer[index].text->canvas.anchorCenterHorz) 
            streamBuffer[index].gameObject->mTransformCache->position.z = (Renderer.viewport.w / 2) + (streamBuffer[index].text->canvas.x * streamBuffer[index].text->size);
        
    }
    
    //
    // Anchor TOP
    
    if (streamBuffer[index].text->canvas.anchorTop) {
        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
        
        topAnchorTotal += (streamBuffer[index].text->size * streamBuffer[index].text->size) / 2;
        topAnchorTotal += streamBuffer[index].text->size * streamBuffer[index].text->canvas.y;
        
        streamBuffer[index].gameObject->mTransformCache->position.y = topAnchorTotal;
    } else {
        
        // Anchor BOTTOM by default
        streamBuffer[index].gameObject->mTransformCache->position.y  = Renderer.displaySize.y - streamBuffer[index].text->size;
        streamBuffer[index].gameObject->mTransformCache->position.y -= streamBuffer[index].text->size * -(streamBuffer[index].text->canvas.y);
        
        // Anchor CENTER vertically
        if (streamBuffer[index].text->canvas.anchorCenterVert) {
            int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
            
            topAnchorTotal += (streamBuffer[index].text->size * streamBuffer[index].text->size) / 2;
            topAnchorTotal += (streamBuffer[index].text->size * streamBuffer[index].text->canvas.y) - (streamBuffer[index].text->size * 2);
            
            streamBuffer[index].gameObject->mTransformCache->position.y = topAnchorTotal;
        }
        
    }
    
    // Flip height and width
    float textGlyphWidth  = streamBuffer[index].text->glyphHeight;
    float textGlyphHeight = streamBuffer[index].text->glyphWidth;
    
    // Check to refresh the vertex buffer
    if (streamBuffer[index].text->mCurrentText != streamBuffer[index].text->text) {
        streamBuffer[index].text->mCurrentText = streamBuffer[index].text->text;
        
        // Clear the text mesh
        streamBuffer[index].meshRenderer->mesh->ClearSubMeshes();
        
        // Update the text string characters 
        Engine.AddMeshText(streamBuffer[index].gameObject, 0, 0, textGlyphWidth, textGlyphHeight, streamBuffer[index].text->text, streamBuffer[index].text->color);
        
    }
    
    return;
}

