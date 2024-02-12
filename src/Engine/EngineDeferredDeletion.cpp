//
// Deferred deletion algorithm

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



void EngineSystemManager::ProcessDeferredDeletion(void) {
    
    for (unsigned int i=0; i < mGarbageObjects.size(); i++) {
        
        GameObject* objectPtr = mGarbageObjects[i];
        
        //
        // Clean up mesh renderers
        //
        
        if (objectPtr->mMeshRendererCache != nullptr) {
            
            for (unsigned int g=0; g < 5; g++) {
                
                int renderQueueIndex = RENDER_QUEUE_SKY + g;
                
                // Remove renderer from the render queue
                if (sceneMain->RemoveMeshRendererFromSceneRoot( objectPtr->mMeshRendererCache, renderQueueIndex )) 
                    break;
            }
            
        }
        
        //
        // Clean up actor mesh renderers
        //
        
        if (objectPtr->mActorCache != nullptr) {
            
            Actor* actorPtr = objectPtr->mActorCache;
            
            for (unsigned int i=0; i < actorPtr->mGeneticRenderers.size(); i++) {
                
                MeshRenderer* renderer = actorPtr->mGeneticRenderers[i];
                
                // Remove renderer from the render queue
                sceneMain->RemoveMeshRendererFromSceneRoot( renderer, RENDER_QUEUE_DEFAULT );
                
                Destroy<MeshRenderer>( renderer );
                
            }
            
        }
        
        DestroyGameObject( objectPtr );
        
        continue;
    }
    
    mGarbageObjects.clear();
    
    return;
}


