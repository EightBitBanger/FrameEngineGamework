#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateTextUI(unsigned int index) {
    
    if (mStreamBuffer[index].meshRenderer == nullptr) 
        return;
    
    if (mStreamBuffer[index].gameObject->isActive) {
        
        mStreamBuffer[index].meshRenderer->isActive = true;
        
    } else {
        
        mStreamBuffer[index].meshRenderer->isActive = false;
        
    }
    
    
    //
    // Anchor RIGHT
    
    if (mStreamBuffer[index].text->canvas.anchorRight) {
        mStreamBuffer[index].transform->position.z = Renderer.viewport.w + 
                                                     mStreamBuffer[index].text->size * 
                                                     mStreamBuffer[index].text->canvas.x;
        
        // Keep text on screen when anchored right
        mStreamBuffer[index].transform->position.z -= mStreamBuffer[index].text->text.size() * // length of string
                                                                   mStreamBuffer[index].text->size;         // Size of font text
        
    } else {
        
        // Anchor LEFT by default
        mStreamBuffer[index].transform->position.z  = (mStreamBuffer[index].text->canvas.x * mStreamBuffer[index].text->size);
        mStreamBuffer[index].transform->position.z += mStreamBuffer[index].text->size;
        
        // Anchor CENTER horizontally
        if (mStreamBuffer[index].text->canvas.anchorCenterHorz) 
            mStreamBuffer[index].transform->position.z = (Renderer.viewport.w / 2) + (mStreamBuffer[index].text->canvas.x * mStreamBuffer[index].text->size);
        
    }
    
    //
    // Anchor TOP
    
    if (mStreamBuffer[index].text->canvas.anchorTop) {
        int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h;
        
        topAnchorTotal += (mStreamBuffer[index].text->size * mStreamBuffer[index].text->size) / 2;
        topAnchorTotal += mStreamBuffer[index].text->size * mStreamBuffer[index].text->canvas.y;
        
        mStreamBuffer[index].transform->position.y = topAnchorTotal;
    } else {
        
        // Anchor BOTTOM by default
        mStreamBuffer[index].transform->position.y  = Renderer.displaySize.y - mStreamBuffer[index].text->size;
        mStreamBuffer[index].transform->position.y -= mStreamBuffer[index].text->size * -(mStreamBuffer[index].text->canvas.y);
        
        // Anchor CENTER vertically
        if (mStreamBuffer[index].text->canvas.anchorCenterVert) {
            int topAnchorTotal = Renderer.displaySize.y - Renderer.viewport.h / 2;
            
            topAnchorTotal += (mStreamBuffer[index].text->size * mStreamBuffer[index].text->size) / 2;
            topAnchorTotal += (mStreamBuffer[index].text->size * mStreamBuffer[index].text->canvas.y) - (mStreamBuffer[index].text->size * 2);
            
            mStreamBuffer[index].transform->position.y = topAnchorTotal;
        }
        
    }
    
    // Flip height and width
    float textGlyphWidth  = mStreamBuffer[index].text->glyphHeight;
    float textGlyphHeight = mStreamBuffer[index].text->glyphWidth;
    
    // Check to refresh the vertex buffer
    if (mStreamBuffer[index].text->mCurrentText != mStreamBuffer[index].text->text) {
        mStreamBuffer[index].text->mCurrentText = mStreamBuffer[index].text->text;
        
        // Clear the text mesh
        mStreamBuffer[index].meshRenderer->mesh->ClearSubMeshes();
        
        // Update the text string characters 
        AddMeshText(mStreamBuffer[index].gameObject, 0, 0, textGlyphWidth, textGlyphHeight, mStreamBuffer[index].text->text, mStreamBuffer[index].text->color);
        
    }
    
    return;
}

