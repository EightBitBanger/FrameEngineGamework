//
// Update actor components

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


void EngineSystemManager::UpdateActor(unsigned int index) {
    
    UpdateActorGenetics(index);
    
    UpdateActorTargetRotation(index);
    
    UpdateActorAnimation(index);
    
    UpdateActorPhysics(index);
    
    //
    // Optimize out actor meshes at distance
    
    // Remove from scene
    if (mStreamBuffer[index].actor->mDistance > AI.GetActorDetailDistance() * 1.4f) {
        
        if (mStreamBuffer[index].actor->mIsActorActiveInScene) {
            
            mStreamBuffer[index].actor->mIsActorActiveInScene = false;
            
            mStreamBuffer[index].actor->SetUpdateGeneticsFlag();
            
            for (unsigned int a=1; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
                
                MeshRenderer* meshRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
                
                sceneMain->RemoveMeshRendererFromSceneRoot( meshRenderer, RENDER_QUEUE_GEOMETRY );
                
            }
            
        }
        
    }
    
    // Add to scene
    if (mStreamBuffer[index].actor->mDistance < AI.GetActorDetailDistance()) {
        
        if (!mStreamBuffer[index].actor->mIsActorActiveInScene) {
            
            mStreamBuffer[index].actor->mIsActorActiveInScene = true;
            
            for (unsigned int a=1; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
                
                MeshRenderer* meshRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
                
                sceneMain->AddMeshRendererToSceneRoot( meshRenderer, RENDER_QUEUE_GEOMETRY );
                
            }
            
        }
        
    }
    
    
    
    // Check walking state
    if (mStreamBuffer[index].actor->mIsWalking) {
        
        // Apply forward velocity
        glm::vec3 forward;
        
        forward.x = cos( glm::radians( -(mStreamBuffer[index].actor->mRotation.y - 90.0f) ) );
        // TODO: Actors should fly???
        //forward.y = tan( glm::radians( -(mStreamBuffer[index].actor->mRotation.x - 90) ) );
        forward.z = sin( glm::radians( -(mStreamBuffer[index].actor->mRotation.y - 90) ) );
        
        float actorSpeed = mStreamBuffer[index].actor->mSpeed;
        
        glm::vec3 actorVelocity = forward * (actorSpeed * 0.1f) * 0.1f;
        
        mStreamBuffer[index].actor->mVelocity.x = actorVelocity.x;
        mStreamBuffer[index].actor->mVelocity.z = actorVelocity.z;
        
        
        // Check running speed multiplier
        if (mStreamBuffer[index].actor->mIsRunning) {
            mStreamBuffer[index].actor->mVelocity.x *= mStreamBuffer[index].actor->mSpeedMul;
            mStreamBuffer[index].actor->mVelocity.z *= mStreamBuffer[index].actor->mSpeedMul;
        }
        
        // Get distance to target
        float targetDistance = glm::distance( mStreamBuffer[index].actor->mTargetPoint, 
                                              mStreamBuffer[index].actor->mPosition );
        
        // Check arrived at target point
        if (targetDistance < 1.5) 
            mStreamBuffer[index].actor->mIsWalking = false;
        
    } else {
        
        // Stop moving but keep falling
        mStreamBuffer[index].actor->mVelocity *= glm::vec3(0, 1, 0);
        
    }
    
    return;
}

