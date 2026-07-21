#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateTransforms(unsigned int index) {
    Transform* current = (Transform*)mStreamBuffer[index].components[EngineComponent::Transform];
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
