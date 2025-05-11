#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateTransformationChains(void) {
    for (unsigned int i = 0; i < mStreamSize; i++) {
        
        Transform* current = mStreamBuffer[i].transform;
        Transform* parent = current->parent;
        
        glm::vec3 currentPosition = current->position;
        glm::quat currentRotation = current->rotation;
        glm::vec3 currentScale = current->scale;
        
        // Roll over the parent matrix transform chain
        while (parent != nullptr) {
            
            currentPosition = parent->position + currentPosition * parent->rotation;
            
            if (parent->inheritParentRotation) 
                currentRotation = parent->rotation * currentRotation;
            
            if (parent->inheritParentScale) 
                currentScale *= parent->scale;
            
            parent = parent->parent;
        }
        
        // Finalize the matrix
        current->matrix = glm::translate(glm::mat4(1), currentPosition) *
                          glm::toMat4(currentRotation) *
                          glm::scale(glm::mat4(1), currentScale);
    }
    return;
}
