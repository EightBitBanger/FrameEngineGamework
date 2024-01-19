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
    if (mDoUpdateDataStream) {
        
        mDoUpdateDataStream = false;
        
        mStreamSize = mGameObjects.Size();
        
        for (unsigned int i=0; i < mStreamSize; i++ ) {
            
            mStreamBuffer[i].gameObject    = mGameObjects[i];
            mStreamBuffer[i].transform     = mGameObjects[i]->mTransformCache;
            
            mStreamBuffer[i].light         = mGameObjects[i]->mLightCache;
            mStreamBuffer[i].actor         = mGameObjects[i]->mActorCache;
            mStreamBuffer[i].camera        = mGameObjects[i]->mCameraCache;
            mStreamBuffer[i].rigidBody     = mGameObjects[i]->mRigidBodyCache;
            mStreamBuffer[i].meshRenderer  = mGameObjects[i]->mMeshRendererCache;
            
            mStreamBuffer[i].text          = mGameObjects[i]->mTextCache;
            mStreamBuffer[i].panel         = mGameObjects[i]->mPanelCache;
            
            continue;
        }
        
    }
    
    
    // Run the parent matrix transform chains
    
    UpdateTransformationChains();
    
    
    // Update UI elements
    
    UpdateUI();
    
    
    // Run the game object list
    
    for (unsigned int i=0; i < mStreamSize; i++ ) {
        
        if (!mStreamBuffer[i].gameObject->isActive) 
            continue;
        
        // Rigid bodies
        if (mStreamBuffer[i].rigidBody != nullptr) 
            UpdateRigidBody(i);
        
        // Mesh renderers
        if (mStreamBuffer[i].meshRenderer != nullptr) 
            UpdateMeshRenderer(i);
        
        // Cameras
        if (mStreamBuffer[i].camera != nullptr) 
            UpdateCamera(i);
        
        // Actors
        if (mStreamBuffer[i].actor != nullptr) 
            UpdateActor(i);
        
        // Lights
        if (mStreamBuffer[i].light != nullptr) 
            UpdateLight(i);
        
        // Panel canvases
        if (mStreamBuffer[i].panel != nullptr) 
            if (mStreamBuffer[i].meshRenderer != nullptr) 
                UpdatePanelUI(i);
        
        // Text elements
        if (mStreamBuffer[i].text != nullptr) 
            UpdateTextUI(i);
        
        continue;
    }
    
    return;
}
