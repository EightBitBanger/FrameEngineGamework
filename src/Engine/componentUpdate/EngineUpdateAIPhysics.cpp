#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateActorPhysics(unsigned int index) {
    
    if (mStreamBuffer[index].rigidBody == nullptr) 
        return;
    
    glm::vec3 actorPosition = mStreamBuffer[index].transform->position;
    glm::vec3 actorRotation = mStreamBuffer[index].actor->mRotation;
    glm::vec3 actorVelocity = mStreamBuffer[index].actor->mVelocity;
    
    
    rp3d::Transform transform = mStreamBuffer[index].rigidBody->getTransform();
    rp3d::Vector3 currentPosition = transform.getPosition();
    mStreamBuffer[index].rigidBody->setTransform(transform);
    
    // Check not on ground
    Hit hit;
    
    if (Physics.Raycast(actorPosition, glm::vec3(0, -1, 0), 1000, hit, LayerMask::Ground)) {
        
        // Standing on ground
        if (actorPosition.y > hit.point.y) {
            
            // Apply some falling action
            actorVelocity.y -= 1.3f;
            
            // Terminal velocity
            float terminalVelocity = -9.81f * 2.0f;
            
            if (actorVelocity.y < terminalVelocity) 
                actorVelocity.y = terminalVelocity;
            
        } else {
            
            actorPosition.y   = hit.point.y;
            currentPosition.y = hit.point.y;
            
        }
        
        
        unsigned int numberOfRenderers = mStreamBuffer[index].actor->GetNumberOfMeshRenderers();
        for (unsigned int i=0; i < numberOfRenderers; i++) {
            
            Material* actorMaterial = mStreamBuffer[index].actor->GetMeshRendererAtIndex(i)->material;
            
            actorMaterial->ambient = Colors.white;
        }
        
        
        // Set current chunk
        GameObject* gameObject = (GameObject*)hit.gameObject;
        
        mStreamBuffer[index].actor->mUserDataA = gameObject->GetUserData();
        
    } else {
        
        actorVelocity = glm::vec3(0, 0, 0);
        
        
        unsigned int numberOfRenderers = mStreamBuffer[index].actor->GetNumberOfMeshRenderers();
        for (unsigned int i=0; i < numberOfRenderers; i++) {
            
            Material* actorMaterial = mStreamBuffer[index].actor->GetMeshRendererAtIndex(i)->material;
            
            actorMaterial->ambient = Colors.black;
            
        }
        
        
    }
    
    // Move the actor into position
    transform.setPosition(currentPosition);
    mStreamBuffer[index].rigidBody->setTransform(transform);
    
    // Factor in youth speed multiplier
    if (mStreamBuffer[index].actor->mAge < 1000) 
        actorVelocity *= mStreamBuffer[index].actor->mSpeedYouth;
    
    // Apply force velocity
    mStreamBuffer[index].rigidBody->applyLocalForceAtCenterOfMass( rp3d::Vector3(actorVelocity.x, 
                                                                                 actorVelocity.y, 
                                                                                 actorVelocity.z) );
    
    // Sync actor position
    mStreamBuffer[index].actor->mPosition = actorPosition;
    mStreamBuffer[index].actor->mRotation = actorRotation;
    mStreamBuffer[index].actor->mVelocity = actorVelocity;
    
    return;
}

