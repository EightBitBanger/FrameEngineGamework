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
    
    for (unsigned int i=0; i < streamSize; i++ ) {
        
        // Current transform
        Transform currentTransform;
        currentTransform.position    = streamBuffer[i].gameObject->mTransformCache->position;
        currentTransform.orientation = streamBuffer[i].gameObject->mTransformCache->orientation;
        currentTransform.scale       = streamBuffer[i].gameObject->mTransformCache->scale;
        
        // Begin the parent chain
        GameObject* parent = streamBuffer[i].gameObject->parent;
        
        // Roll over the parent transform chain
        while (parent != nullptr) {
            
            currentTransform.position    += parent->mTransformCache->position;
            currentTransform.orientation *= parent->mTransformCache->orientation;
            currentTransform.scale       *= parent->mTransformCache->scale;
            
            parent = parent->parent;
        }
        
        glm::mat4 translation = glm::translate(glm::mat4(1), currentTransform.position);
        glm::mat4 rotation    = glm::toMat4(currentTransform.orientation);
        glm::mat4 scale       = glm::scale(glm::mat4(1), currentTransform.scale);
        
        streamBuffer[i].gameObject->mTransformCache->matrix = translation * rotation * scale;
        
        continue;
    }
    
    return;
}

