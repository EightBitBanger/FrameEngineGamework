#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/EngineSystems.h>


void EngineSystemManager::UpdateComponentStream(void) {
    unsigned int numberOfGameObjects = mGameObjects.Size();
    if (numberOfGameObjects == 0) return;
    
    mDataStreamIndex = 0;
    std::vector<GameObject*> garbageObjects;
    
    for (unsigned int i = 0; i < numberOfGameObjects; i++) {
        mObjectIndex = i;
        GameObject* gameObject = mGameObjects[mObjectIndex];
        
        // Guard transform before use
        bool doUpdate = true;
        Transform* transformCache = (Transform*)gameObject->mComponents[EngineComponent::Transform];
        if (sceneMain && sceneMain->camera && gameObject->renderDistance > 0 && transformCache) {
            if (glm::distance(transformCache->position, sceneMain->camera->transform.position) > gameObject->renderDistance) {
                doUpdate = false;
            }
        }
        
        MeshRenderer*    meshRendererCache = (MeshRenderer*)gameObject->mComponents[EngineComponent::MeshRenderer];
        Light*           lightCache        = (Light*)gameObject->mComponents[EngineComponent::Light];
        rp3d::RigidBody* rigidBodyCache    = (rp3d::RigidBody*)gameObject->mComponents[EngineComponent::RigidBody];
        
        bool activeState = gameObject->isActive && doUpdate;
        if (meshRendererCache)  meshRendererCache->isActive = activeState;
        if (lightCache)         lightCache->isActive = activeState;
        if (rigidBodyCache)     rigidBodyCache->setIsActive(activeState);
        
        if (gameObject->isGarbage) garbageObjects.push_back(gameObject);
        if (!gameObject->isActive || !doUpdate) continue;
        
        // Grow the stream buffer
        if (mDataStreamIndex >= mStreamBuffer.size()) 
            mStreamBuffer.emplace_back();
        
        // Set buffer stream object and components
        auto& slot = mStreamBuffer[mDataStreamIndex];
        
        // Clear old pointers
        slot.gameObject = nullptr;
        for (unsigned int c = 0; c < EngineComponent::NumberOfComponents; ++c) {
            slot.components[c] = nullptr;
        }
        
        slot.gameObject = gameObject;
        for (unsigned int c = 0; c < EngineComponent::NumberOfComponents; ++c) {
            if (gameObject->mComponents[c]) slot.components[c] = gameObject->mComponents[c];
        }
        
        mDataStreamIndex++;
        
        // Preallocate some space
        if (mStreamBuffer.capacity() <= mDataStreamIndex) {
            size_t newCap = mStreamBuffer.capacity() ? mStreamBuffer.capacity() * 2 : 64;
            mStreamBuffer.reserve(newCap);
        }
    }
    
    // Finalize stream size for this tick
    mStreamSize = mDataStreamIndex;
    
    // Destroy garbage objects
    unsigned int numberOfObjectsOnDeathRow = garbageObjects.size();
    for (unsigned int i=0; i < numberOfObjectsOnDeathRow; i++) {
        GameObject* gameObject = garbageObjects[i];
        
        for (std::vector<GameObject*>::iterator it = mGameObjectActive.begin(); it != mGameObjectActive.end(); ++it) {
            if (gameObject == *it) { mGameObjectActive.erase(it); break; }
        }
        
        gameObject->Deactivate();
        
        unsigned int numberOfComponents = gameObject->GetComponentCount();
        for (unsigned int j=0; j < numberOfComponents; j++) {
            Component* componentPtr = gameObject->GetComponentIndex(j);
            DestroyComponent(componentPtr);
            mComponents.Destroy(componentPtr);
        }
        
        mGameObjects.Destroy(gameObject);
    }
}
