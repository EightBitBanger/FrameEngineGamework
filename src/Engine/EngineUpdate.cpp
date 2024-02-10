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



void EngineSystemManager::Update(void) {
    
    glm::vec3 eye = sceneMain->camera->transform.position;
    
    // Update player/camera position in the AI simulation
    if (sceneMain != nullptr) {
        
        Camera* activeCamera = sceneMain->camera;
        
        if (activeCamera != nullptr) 
            AI.SetPlayerWorldPosition( activeCamera->transform.position );
        
    }
    
    if (mGameObjects.Size() == 0) 
        return;
    
    unsigned int objectsPerTick = mGameObjects.Size() / 8;
    
    for (unsigned int i=0; i < objectsPerTick; i++) {
        
        mObjectIndex++;
        
        if (mObjectIndex >= mGameObjects.Size()) 
            mObjectIndex = 0;
        
        GameObject* gameObject = mGameObjects[mObjectIndex];
        
        // Check game object render distance
        bool shoudRender = true;
        
        if (gameObject->renderDistance > 0) {
            
            if (glm::distance(gameObject->mTransformCache->position, eye) > gameObject->renderDistance) 
                shoudRender = false;
            
        }
        
        
        // Check garbage collection
        
        if (gameObject->mIsGarbage) {
            
            mGarbageObjects.push_back( gameObject );
            
            // Set to inactivate components in the next step
            gameObject->isActive = false;
            
        }
        
        
        // Update the state of associated components
        
        bool activeState = true;
        
        if ((!gameObject->isActive) | (!shoudRender))
            activeState = false;
        
        if (gameObject->mActorCache) 
            gameObject->mActorCache->SetActive( activeState );
        
        if (gameObject->mMeshRendererCache) 
            gameObject->mMeshRendererCache->isActive = activeState;
        
        if (gameObject->mLightCache) 
            gameObject->mLightCache->isActive = activeState;
        
        if (gameObject->mRigidBodyCache) 
            gameObject->mRigidBodyCache->setIsActive( activeState );
        
        
        // Check last object
        if (mObjectIndex == mGameObjects.Size() - 1) {
            
            mStreamSize = mDataStreamIndex;
            
            mDataStreamIndex = 0;
        }
        
        // UI elements should always be added to the stream buffer
        bool isUIElement = false;
        
        if (gameObject->mTextCache != nullptr)  isUIElement = true;
        if (gameObject->mPanelCache != nullptr) isUIElement = true;
        
        
        //
        // Final check before added to list
        
        if (((!gameObject->isActive) | (!shoudRender)) & (!isUIElement)) 
            continue;
        
        //
        // Set buffer stream objects and components
        //
        
        mStreamBuffer[mDataStreamIndex].gameObject    = gameObject;
        mStreamBuffer[mDataStreamIndex].transform     = gameObject->mTransformCache;
        
        mStreamBuffer[mDataStreamIndex].light         = gameObject->mLightCache;
        mStreamBuffer[mDataStreamIndex].actor         = gameObject->mActorCache;
        mStreamBuffer[mDataStreamIndex].camera        = gameObject->mCameraCache;
        mStreamBuffer[mDataStreamIndex].rigidBody     = gameObject->mRigidBodyCache;
        mStreamBuffer[mDataStreamIndex].meshRenderer  = gameObject->mMeshRendererCache;
        
        mStreamBuffer[mDataStreamIndex].text          = gameObject->mTextCache;
        mStreamBuffer[mDataStreamIndex].panel         = gameObject->mPanelCache;
        
        mDataStreamIndex++;
        
        if (mStreamSize < mDataStreamIndex)
            mStreamSize++;
        
        continue;
    }
    
    
    // Run the parent matrix transform chains
    
    UpdateTransformationChains();
    
    
    // Update UI elements
    
    UpdateUI();
    
    UpdateConsole();
    
    
    //
    // Update attached components
    //
    
    for (unsigned int i=0; i < mStreamSize; i++ ) {
        
        if (mStreamBuffer[i].rigidBody != nullptr)       UpdateRigidBody(i);
        if (mStreamBuffer[i].meshRenderer != nullptr)    UpdateMeshRenderer(i);
        if (mStreamBuffer[i].camera != nullptr)          UpdateCamera(i);
        if (mStreamBuffer[i].actor != nullptr)           UpdateActor(i);
        if (mStreamBuffer[i].light != nullptr)           UpdateLight(i);
        if (mStreamBuffer[i].text != nullptr)            UpdateTextUI(i);
        
        if ((mStreamBuffer[i].panel != nullptr) & 
            (mStreamBuffer[i].meshRenderer != nullptr))  UpdatePanelUI(i);
        
        continue;
    }
    
    
    //
    // Process garbage objects
    //
    
    ProcessDeferredDeletion();
    
    
    return;
}





