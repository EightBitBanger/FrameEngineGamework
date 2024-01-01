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



void EngineSystemManager::Update(void) {
    
    // Update player/camera position in the AI simulation
    if (sceneMain != nullptr) {
        
        Camera* activeCamera = sceneMain->camera;
        
        if (activeCamera != nullptr) 
            AI.SetPlayerWorldPosition( activeCamera->transform.position );
        
    }
    
    // Check to update the data stream
    if (doUpdateDataStream) {
        
        doUpdateDataStream = true;
        
        streamSize = mGameObjects.Size();
        
        for (unsigned int i=0; i < streamSize; i++ ) {
            
            streamBuffer[i].gameObject    = mGameObjects[i];
            
            streamBuffer[i].light         = mGameObjects[i]->mLightCache;
            streamBuffer[i].actor         = mGameObjects[i]->mActorCache;
            streamBuffer[i].camera        = mGameObjects[i]->mCameraCache;
            streamBuffer[i].rigidBody     = mGameObjects[i]->mRigidBodyCache;
            streamBuffer[i].meshRenderer  = mGameObjects[i]->mMeshRendererCache;
            
            streamBuffer[i].text          = mGameObjects[i]->mTextCache;
            streamBuffer[i].panel         = mGameObjects[i]->mPanelCache;
            
            continue;
        }
        
    }
    
    
    // Run the parent matrix transform chains
    
    UpdateTransformationChains();
    
    
    // Update UI elements
    
    Engine.UpdateUI();
    
    
    // Run the game object list
    
    for (unsigned int i=0; i < streamSize; i++ ) {
        
        if (!streamBuffer[i].gameObject->isActive) 
            continue;
        
        // Rigid bodies
        if (streamBuffer[i].rigidBody != nullptr) 
            UpdateRigidBody(i);
        
        // Mesh renderers
        if (streamBuffer[i].meshRenderer != nullptr) 
            UpdateMeshRenderer(i);
        
        // Cameras
        if (streamBuffer[i].camera != nullptr) 
            UpdateCamera(i);
        
        // Actors
        if (streamBuffer[i].actor != nullptr) 
            if (streamBuffer[i].actor->mIsActive) 
                UpdateActor(i);
        
        // Lights
        if (streamBuffer[i].light != nullptr) 
            UpdateLight(i);
        
        // Panel canvases
        if (streamBuffer[i].panel != nullptr) 
            if (streamBuffer[i].meshRenderer != nullptr) 
                UpdatePanelUI(i);
        
        // Text elements
        if (streamBuffer[i].text != nullptr) 
            UpdateTextUI(i);
        
        continue;
    }
    
    return;
}
