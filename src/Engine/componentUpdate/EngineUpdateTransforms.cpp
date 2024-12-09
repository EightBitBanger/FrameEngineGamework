#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateTransformationChains(void) {
    
    for (unsigned int i=0; i < mStreamSize; i++ ) {
        
        Transform* current = mStreamBuffer[i].transform;
        Transform* parent  = mStreamBuffer[i].transform->parent;
        
        glm::vec3 currentPosition = mStreamBuffer[i].transform->position;
        glm::quat currentRotation = mStreamBuffer[i].transform->rotation;
        glm::vec3 currentScale    = mStreamBuffer[i].transform->scale;
        
        // Roll over the parent matrix transform chain
        
        // TODO: Rotations should be positioned locally from the
        //       previous transform rather than the base object.
        
        while (parent != nullptr) {
            
            currentPosition += parent->position;
            
            if (parent->inheritParentRotation) 
                currentRotation *= parent->rotation;
            
            if (parent->inheritParentScale) 
                currentScale *= parent->scale;
            
            parent = parent->parent;
            
            continue;
        }
        
        // Finalize the matrix
        if (current->inheritParentRotation) {
            
            current->matrix = glm::toMat4( currentRotation ) * 
                              glm::translate( glm::mat4(1), currentPosition ) * 
                              glm::scale( glm::mat4(1), currentScale );
        } else {
            
            current->matrix = glm::translate( glm::mat4(1), currentPosition ) * 
                              glm::toMat4( currentRotation ) * 
                              glm::scale( glm::mat4(1), currentScale );
        }
        
        continue;
    }
    
    return;
}

