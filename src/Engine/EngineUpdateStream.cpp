#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateComponentStream(void) {
    
    unsigned int numberOfGameObjects = mGameObjects.Size();
    
    if (numberOfGameObjects == 0) 
        return;
    
    unsigned int objectsPerTick = glm::max((unsigned int)1, numberOfGameObjects / 8);

    for (unsigned int i = 0; i < objectsPerTick; i++) {
        
        mObjectIndex = (mObjectIndex + 1) % numberOfGameObjects;
        
        GameObject* gameObject = mGameObjects[mObjectIndex];
        
        // Check game object render distance
        bool shouldRender = true;
        
        if (gameObject->renderDistance > 0) {
            if (glm::distance(gameObject->mTransformCache->position, sceneMain->camera->transform.position) > gameObject->renderDistance) 
                shouldRender = false;
        }
        
        // Update the state of associated components
        bool activeState = gameObject->isActive && shouldRender;
        
        if (gameObject->mActorCache != nullptr) 
            gameObject->mActorCache->SetActive(activeState);
        
        if (gameObject->mMeshRendererCache != nullptr) 
            gameObject->mMeshRendererCache->isActive = activeState;
        
        if (gameObject->mLightCache != nullptr) 
            gameObject->mLightCache->isActive = activeState;
        
        if (gameObject->mRigidBodyCache != nullptr) 
            gameObject->mRigidBodyCache->setIsActive(activeState);
        
        // Check last object
        if (mObjectIndex == numberOfGameObjects - 1) {
            mStreamSize = mDataStreamIndex;
            mDataStreamIndex = 0;
        }
        
        // UI elements should always be added to the stream buffer
        bool isUIElement = gameObject->mTextCache != nullptr || gameObject->mPanelCache != nullptr;
        
        // Check garbage objects
        if (gameObject->isGarbage) {
            
            // Remove the game object from the active list
            for (std::vector<GameObject*>::iterator it = mGameObjectActive.begin(); it != mGameObjectActive.end(); ++it) {
                
                if (gameObject != *it) 
                    continue;
                
                mGameObjectActive.erase(it);
                
                break;
            }

            gameObject->Deactivate();
            
            // Destroy the components
            unsigned int numberOfComponents = gameObject->GetComponentCount();
            for (unsigned int i = 0; i < numberOfComponents; i++) {
                
                DestroyComponent(gameObject->GetComponentIndex(i));
                
                mComponents.Destroy(gameObject->GetComponentIndex(i));
            }
            
            mGameObjects.Destroy(gameObject);
            continue;
        }
        
        // Set buffer stream objects and components
        if ((!gameObject->isActive || !shouldRender) && !isUIElement) 
            continue;
        
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
        mStreamSize = std::max(mStreamSize, mDataStreamIndex);
    }
}
