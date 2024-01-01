//
// Update transform components

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


void EngineSystemManager::UpdateTransformationChains(void) {
    
    for (unsigned int i=0; i < mStreamSize; i++ ) {
        
        glm::vec3 currentPosition = mStreamBuffer[i].transform->position;
        glm::quat currentRotation = mStreamBuffer[i].transform->localRotation;
        glm::vec3 currentScale    = mStreamBuffer[i].transform->localScale;
        
        // Roll over the parent matrix transform chain
        
        Transform* parent = mStreamBuffer[i].transform->parent;
        
        while (parent != nullptr) {
            
            currentPosition += parent->position;
            currentRotation *= parent->localRotation;
            currentScale    *= parent->localScale;
            
            parent = parent->parent;
        }
        
        // Finalize the matrix
        
        mStreamBuffer[i].transform->matrix = glm::translate( glm::mat4(1), currentPosition ) * 
                                             glm::toMat4( currentRotation ) * 
                                             glm::scale( glm::mat4(1), currentScale );
        
        continue;
    }
    
    return;
}

