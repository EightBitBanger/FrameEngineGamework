#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/EngineSystems.h>


void EngineSystemManager::UpdateComponentStream(void) {
    std::lock_guard<std::mutex> lock(mux);
    
    unsigned int numberOfGameObjects = mGameObjects.Size();
    if (numberOfGameObjects == 0) 
        return;
    
    std::vector<GameObject*> garbageObjects;
    
    unsigned int objectsPerTick = glm::max((unsigned int)1, numberOfGameObjects / 8);
    for (unsigned int i = 0; i < numberOfGameObjects; i++) {
        mObjectIndex = (mObjectIndex + 1) % numberOfGameObjects;
        GameObject* gameObject = mGameObjects[mObjectIndex];
        
        // Check game object render distance
        bool doUpdate = true;
        Transform* transformCache = (Transform*)gameObject->mComponents[EngineComponents::Transform];
        if (sceneMain != nullptr) 
            if (sceneMain->camera != nullptr) 
                if (gameObject->renderDistance > 0) 
                    if (glm::distance(transformCache->position, sceneMain->camera->transform.position) > gameObject->renderDistance) 
                        doUpdate = false;
        
        // Update the active state of associated components
        MeshRenderer*    meshRendererCache = (MeshRenderer*)gameObject->mComponents[EngineComponents::MeshRenderer];
        Light*           lightCache        = (Light*)gameObject->mComponents[EngineComponents::Light];
        rp3d::RigidBody* rigidBodyCache    = (rp3d::RigidBody*)gameObject->mComponents[EngineComponents::RigidBody];
        
        bool activeState = gameObject->isActive && doUpdate;
        if (meshRendererCache != nullptr)  meshRendererCache->isActive = activeState;
        if (lightCache != nullptr)         lightCache->isActive = activeState;
        if (rigidBodyCache != nullptr)     rigidBodyCache->setIsActive(activeState);
        
        // Check last object
        if (mObjectIndex == numberOfGameObjects - 1) {
            mStreamSize = mDataStreamIndex;
            mDataStreamIndex = 0;
        }
        
        // Check garbage collection
        if (gameObject->isGarbage) 
            garbageObjects.push_back(gameObject);
        
        if (!gameObject->isActive || !doUpdate) 
            continue;
        
        // Set buffer stream object and components
        mStreamBuffer[mDataStreamIndex].gameObject = gameObject;
        for (unsigned int i=0; i < EngineComponents::NumberOfComponents; i++) 
            if (gameObject->mComponents[i] != nullptr) 
                mStreamBuffer[mDataStreamIndex].components[i] = gameObject->mComponents[i];
        
        // Extend the stream buffer if needed
        mDataStreamIndex++;
        if (mStreamBuffer.capacity() <= mDataStreamIndex) 
            mStreamBuffer.reserve(mStreamBuffer.capacity() * 2);
        
        mStreamSize = std::max(mStreamSize, mDataStreamIndex);
    }
    
    // Destroy garbage objects
    unsigned int numberOfObjectsOnDeathRow = garbageObjects.size();
    for (unsigned int i=0; i < numberOfObjectsOnDeathRow; i++) {
        GameObject* gameObject = garbageObjects[i];
        
        // Remove the game object from the active list
        for (std::vector<GameObject*>::iterator it = mGameObjectActive.begin(); it != mGameObjectActive.end(); ++it) {
            if (gameObject != *it) 
                continue;
            
            mGameObjectActive.erase(it);
            break;
        }
        
        gameObject->Deactivate();
        
        unsigned int numberOfComponents = gameObject->GetComponentCount();
        for (unsigned int i=0; i < numberOfComponents; i++) {
            Component* componentPtr = gameObject->GetComponentIndex(i);
            DestroyComponent( componentPtr );
            mComponents.Destroy( componentPtr );
        }
        
        mGameObjects.Destroy(gameObject);
    }
    
}

