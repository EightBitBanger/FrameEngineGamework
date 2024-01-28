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

ENGINE_API extern PlatformLayer         Platform;
ENGINE_API extern EngineSystemManager   Engine;


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
        
        // Check game object render distance
        if (mGameObjects[mObjectIndex]->renderDistance > 0) {
            
            if (glm::distance(mGameObjects[mObjectIndex]->mTransformCache->position, eye) < mGameObjects[mObjectIndex]->renderDistance) {
                
                mGameObjects[mObjectIndex]->isActive = true;
                
            } else {
                
                mGameObjects[mObjectIndex]->isActive = false;
                
            }
            
            // Set the state of associated components
            
            bool activeState = mGameObjects[mObjectIndex]->isActive;
            
            if (mGameObjects[mObjectIndex]->mActorCache) 
                mGameObjects[mObjectIndex]->mActorCache->SetActive( activeState );
            
            if (mGameObjects[mObjectIndex]->mMeshRendererCache) 
                mGameObjects[mObjectIndex]->mMeshRendererCache->isActive = activeState;
            
            if (mGameObjects[mObjectIndex]->mLightCache) 
                mGameObjects[mObjectIndex]->mLightCache->isActive = activeState;
            
            if (mGameObjects[mObjectIndex]->mRigidBodyCache) 
                mGameObjects[mObjectIndex]->mRigidBodyCache->setIsActive( activeState );
            
        }
        
        // Check last object
        if (mObjectIndex == mGameObjects.Size() - 1) {
            
            mStreamSize = mDataStreamIndex;
            
            mDataStreamIndex = 0;
        }
        
        if (!mGameObjects[mObjectIndex]->isActive) 
            continue;
        
        // Add the object to the object buffer stream
        mStreamBuffer[mDataStreamIndex].gameObject    = mGameObjects[mObjectIndex];
        mStreamBuffer[mDataStreamIndex].transform     = mGameObjects[mObjectIndex]->mTransformCache;
        
        mStreamBuffer[mDataStreamIndex].light         = mGameObjects[mObjectIndex]->mLightCache;
        mStreamBuffer[mDataStreamIndex].actor         = mGameObjects[mObjectIndex]->mActorCache;
        mStreamBuffer[mDataStreamIndex].camera        = mGameObjects[mObjectIndex]->mCameraCache;
        mStreamBuffer[mDataStreamIndex].rigidBody     = mGameObjects[mObjectIndex]->mRigidBodyCache;
        mStreamBuffer[mDataStreamIndex].meshRenderer  = mGameObjects[mObjectIndex]->mMeshRendererCache;
        
        mStreamBuffer[mDataStreamIndex].text          = mGameObjects[mObjectIndex]->mTextCache;
        mStreamBuffer[mDataStreamIndex].panel         = mGameObjects[mObjectIndex]->mPanelCache;
        
        mDataStreamIndex++;
        
        if (mStreamSize < mDataStreamIndex)
            mStreamSize++;
        
        continue;
    }
    
    
    // Run the parent matrix transform chains
    
    UpdateTransformationChains();
    
    
    // Update UI elements
    
    UpdateUI();
    
    
    // Run the game object list
    
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
    
    
    return;
}
