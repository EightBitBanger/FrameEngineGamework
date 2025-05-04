#include <GameEngineFramework/Engine/EngineSystems.h>

// Terminal velocity -9.81 * 2.0 * 2.0
#define TERMINAL_VELOCITY  39.24f


void EngineSystemManager::UpdateKinematics(unsigned int index) {
    if (mStreamBuffer[index].rigidBody == nullptr) 
        return;
    
    // Check if the actor needs a collider generated on thread main
    GenerateCollider(index);
    
    glm::vec3 actorPosition = mStreamBuffer[index].transform->position;
    glm::vec3 actorRotation = mStreamBuffer[index].actor->navigation.mRotation;
    glm::vec3 actorVelocity = mStreamBuffer[index].actor->navigation.mVelocity;
    glm::vec3 actorTarget = mStreamBuffer[index].actor->navigation.mTargetPoint;
    
    rp3d::Transform transform = mStreamBuffer[index].rigidBody->getTransform();
    rp3d::Vector3 currentPosition = transform.getPosition();
    
    // Check not on ground
    Hit hit;
    if (Physics.Raycast(actorPosition, glm::vec3(0, -1, 0), 1000, hit, LayerMask::Ground)) {
        
        // Standing on ground
        if (actorPosition.y > hit.point.y) {
            
            // Apply some falling action
            actorVelocity.y -= 1.3f;
            
            // Check terminal velocity
            if (actorVelocity.y < -TERMINAL_VELOCITY) 
                actorVelocity.y = -TERMINAL_VELOCITY;
            
        } else {
            
            actorPosition.y = hit.point.y;
            currentPosition.y = hit.point.y;
        }
        
        // Ground detected under actor - Clear color indicator
        unsigned int numberOfRenderers = mStreamBuffer[index].actor->genetics.GetNumberOfMeshRenderers();
        for (unsigned int i=0; i < numberOfRenderers; i++) {
            MeshRenderer* geneRenderer = mStreamBuffer[index].actor->genetics.GetMeshRendererAtIndex(i);
            if (geneRenderer != nullptr) {
                Material* actorMaterial = geneRenderer->material;
                if (actorMaterial !=nullptr) 
                    actorMaterial->ambient = Colors.white;
            }
        }
        
    } else {
        
        // No ground detected under actor - Apply dark color indicator
        actorVelocity = glm::vec3(0, 0, 0);
        unsigned int numberOfRenderers = mStreamBuffer[index].actor->genetics.GetNumberOfMeshRenderers();
        for (unsigned int i=0; i < numberOfRenderers; i++) {
            MeshRenderer* geneRenderer = mStreamBuffer[index].actor->genetics.GetMeshRendererAtIndex(i);
            if (geneRenderer == nullptr) 
                continue;
            
            Material* actorMaterial = geneRenderer->material;
            if (actorMaterial !=nullptr) 
                actorMaterial->ambient = Colors.black;
            
        }
        
        
    }
    
    // Get actor target height
    actorTarget.y = 1000;
    
    if (Physics.Raycast(actorTarget, glm::vec3(0, -1, 0), 2000, hit, LayerMask::Ground)) {
        
        actorTarget.y = hit.point.y;
        
    } else {
        
        actorTarget.y = 0.0f;
    }
    
    // Move the actor into position
    transform.setPosition(currentPosition);
    mStreamBuffer[index].rigidBody->setTransform(transform);
    
    // Factor in youth speed multiplier
    if (mStreamBuffer[index].actor->physical.mAge < 1000) 
        actorVelocity *= mStreamBuffer[index].actor->physical.mSpeedYouth;
    
    // Check max velocity
    actorVelocity.x = glm::clamp(actorVelocity.x, -1.0f, 1.0f);
    actorVelocity.y = glm::clamp(actorVelocity.y, -1.0f, 1.0f);
    actorVelocity.z = glm::clamp(actorVelocity.z, -1.0f, 1.0f);
    
    // Apply force velocity
    mStreamBuffer[index].rigidBody->applyLocalForceAtCenterOfMass( rp3d::Vector3(actorVelocity.x, 
                                                                                 actorVelocity.y, 
                                                                                 actorVelocity.z) );
    
    // Sync actor position
    mStreamBuffer[index].actor->navigation.mPosition = actorPosition;
    mStreamBuffer[index].actor->navigation.mRotation = actorRotation;
    mStreamBuffer[index].actor->navigation.mVelocity = actorVelocity;
    mStreamBuffer[index].actor->navigation.mTargetPoint = actorTarget;
    
    return;
}

// Function to generate collider
void EngineSystemManager::GenerateCollider(unsigned int index) {
    
    rp3d::RigidBody* rigidBody = mStreamBuffer[index].rigidBody;
    
    unsigned int numberOfColliders = rigidBody->getNbColliders();
    
    if (numberOfColliders == 0) {
        
        rp3d::Transform colliderOffset(rp3d::Transform::identity());
        
        colliderOffset.setPosition(rp3d::Vector3(0.0f, 1.0f, 0.0f));
        rp3d::Collider* collider = rigidBody->addCollider(colliders.box, colliderOffset);
        
        collider->setCollisionCategoryBits(static_cast<unsigned short>(LayerMask::Actor));
        collider->setCollideWithMaskBits(static_cast<unsigned short>(CollisionMask::Entity));
        
        collider->setUserData(static_cast<void*>(mStreamBuffer[index].gameObject));
        rigidBody->setUserData(static_cast<void*>(mStreamBuffer[index].gameObject));
    }
    
    return;
}

