#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateActorPhysics(unsigned int index) {
    
    if (mStreamBuffer[index].rigidBody == nullptr) 
        return;
    
    glm::vec3 actorPosition = mStreamBuffer[index].transform->position;
    glm::vec3 actorRotation = mStreamBuffer[index].actor->mRotation;
    glm::vec3 actorVelocity = mStreamBuffer[index].actor->mVelocity;
    
    // Ray cast here
    glm::vec3 from      = actorPosition;
    glm::vec3 direction = glm::vec3(0, -1, 0);
    from.y = 900;
    
    Hit hit;
    
    float distance = 1000;
    
    // Move the actor out of the way since we cant cast a ray from inside the collider...
    rp3d::Transform transform = mStreamBuffer[index].rigidBody->getTransform();
    rp3d::Vector3 currentPosition = transform.getPosition();
    currentPosition.y += 1000;
    transform.setPosition(currentPosition);
    mStreamBuffer[index].rigidBody->setTransform(transform);
    
    // Check not on ground
    if (!Physics.Raycast(from, direction, distance, hit, LayerMask::Ground)) {
        
        // Apply some falling action
        //actorVelocity.y -= 0.01;
        
        // Terminal velocity
        //if (actorVelocity.y < -1) 
        //    actorVelocity.y = -1;
        
        actorVelocity = glm::vec3(0, 0, 0);
        
        /*
        
        unsigned int numberOfRenderers = mStreamBuffer[index].actor->GetNumberOfMeshRenderers();
        for (unsigned int i=0; i < numberOfRenderers; i++) {
            
            Material* actorMaterial = mStreamBuffer[index].actor->GetMeshRendererAtIndex(i)->material;
            
            actorMaterial->ambient = Colors.black;
            
        }
        
        */
        
    } else {
        
        unsigned int numberOfRenderers = mStreamBuffer[index].actor->GetNumberOfMeshRenderers();
        for (unsigned int i=0; i < numberOfRenderers; i++) {
            
            Material* actorMaterial = mStreamBuffer[index].actor->GetMeshRendererAtIndex(i)->material;
            
            actorMaterial->ambient = Colors.white;
        }
        
        // Standing on ground
        actorPosition.y   = hit.point.y;
        currentPosition.y = hit.point.y + 1000;
        
        // Set current chunk
        GameObject* gameObject = (GameObject*)hit.gameObject;
        
        mStreamBuffer[index].actor->mUserDataA = gameObject->GetUserData();
        
        actorVelocity.y = 0;
        
    }
    
    // Move the actor back into position since we are finished casting rays...
    currentPosition.y -= 1000;
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

