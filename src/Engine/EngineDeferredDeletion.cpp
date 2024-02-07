//
// Update console

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

ENGINE_API extern PlatformLayer         Platform;
ENGINE_API extern EngineSystemManager   Engine;

void EngineSystemManager::ProcessDeferredDeletion(void) {
    
    for (unsigned int i=0; i < mGarbageObjects.size(); i++) {
        
        GameObject* objectPtr = mGarbageObjects[i];
        
        // Make damn sure the mesh renderer is removed from the render queues before termination
        
        if (objectPtr->mMeshRendererCache != nullptr) {
            
            for (unsigned int g=0; g < 5; g++) {
                
                int renderQueueIndex = RENDER_QUEUE_SKY + g;
                
                if (sceneMain->RemoveMeshRendererFromSceneRoot( objectPtr->mMeshRendererCache, renderQueueIndex )) 
                    break;
            }
            
        }
        
        DestroyGameObject( objectPtr );
        
        continue;
    }
    
    mGarbageObjects.clear();
    
    return;
}


