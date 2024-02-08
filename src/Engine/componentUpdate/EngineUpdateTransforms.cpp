//
// Update transform components

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern StringType        String;
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

ENGINE_API extern PlatformLayer     Platform;


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

