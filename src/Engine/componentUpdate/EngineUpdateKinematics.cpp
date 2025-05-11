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
    rp3d::Vector3 physicsPosition = transform.getPosition();
    
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
            physicsPosition.y = hit.point.y;
        }
        
    }
    
    // Get height at the target point
    if (actorTarget.y == 1000.0f) {
        actorTarget.y = 1000;
        if (Physics.Raycast(actorTarget, glm::vec3(0, -1, 0), 2000, hit, LayerMask::Ground)) 
            actorTarget.y = hit.point.y;
    }
    
    // Move the actor into position
    transform.setPosition(physicsPosition);
    mStreamBuffer[index].rigidBody->setTransform(transform);
    
    // Check max velocity
    actorVelocity = glm::clamp(actorVelocity, -1.0f, 1.0f);
    
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

