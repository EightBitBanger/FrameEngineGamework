#include <GameEngineFramework/Engine/EngineSystems.h>
#include <mutex>

// Terminal velocity -9.81 * 2.0 * 2.0
#define TERMINAL_VELOCITY  39.24f


void EngineSystemManager::UpdateKinematics(void) {
    unsigned int numberOfActors = AI.GetNumberOfActors();
    for (unsigned int i=0; i < numberOfActors; i++) {
        Actor* actor = AI.GetActor(i);
        std::lock_guard<std::mutex> (actor->navigation.mux);
        
        if (actor->physical.mColliderBody != nullptr && actor->isGarbage) {
            if (actor->physical.mColliderBody != nullptr) 
                Physics.DestroyCollisionBody(actor->physical.mColliderBody);
            
            actor->physical.mColliderBody = nullptr;
        }
        
        // Check to generate a collider
        if (actor->physical.mDoUpdateCollider) {
            actor->physical.mDoUpdateCollider = false;
            
            if (actor->physical.mColliderBody != nullptr) 
                Physics.DestroyCollisionBody(actor->physical.mColliderBody);
            
            actor->physical.mColliderBody = Physics.CreateCollisionBody(actor->navigation.mPosition.x, 
                                                                        actor->navigation.mPosition.y, 
                                                                        actor->navigation.mPosition.z);
            
            rp3d::Transform colliderTransform;
            colliderTransform.setPosition( rp3d::Vector3(actor->physical.mColliderOffset.x, actor->physical.mColliderOffset.y, actor->physical.mColliderOffset.z) );
            rp3d::Collider* coll = actor->physical.mColliderBody->addCollider( GetColliderBox(actor->physical.mColliderScale), colliderTransform );
            
            coll->setCollisionCategoryBits((unsigned short)LayerMask::Actor);
            coll->setCollideWithMaskBits((unsigned short)CollisionMask::Entity);
            
            coll->setUserData( (void*)actor );
        }
        
        // Check query points
        Hit hit;
        unsigned int numberOfPoints = actor->navigation.mQueryPoints.size();
        for (unsigned int i=0; i < numberOfPoints; i++) {
            glm::vec3 queryPoint = actor->navigation.mQueryPoints[i];
            
            // Get point height
            if (Physics.Raycast(queryPoint, glm::vec3(0, -1, 0), 2000, hit, LayerMask::Ground)) 
                actor->navigation.mQueryPoints[i].y = hit.point.y;
        }
        
        glm::vec3 actorPosition = actor->navigation.mPosition;
        glm::vec3 actorRotation = actor->navigation.mRotation;
        glm::vec3 actorVelocity = actor->navigation.mVelocity;
        glm::vec3 actorTarget   = actor->navigation.mTargetPoint;
        
        // Update collision body
        if (actor->physical.mColliderBody != nullptr) {
            rp3d::Transform transform = actor->physical.mColliderBody->getTransform();
            transform.setPosition(rp3d::Vector3(actorPosition.x, actorPosition.y, actorPosition.z));
            actor->physical.mColliderBody->setTransform(transform);
        }
        
        actorPosition.y += 500.0f;
        actorTarget.y += 500.0f;
        
        // Check not on ground
        if (Physics.Raycast(actorPosition, glm::vec3(0, -1, 0), 2000, hit, LayerMask::Ground)) 
            actorPosition.y = hit.point.y;
        
        // Get height at the target point
        if (Physics.Raycast(actorTarget, glm::vec3(0, -1, 0), 2000, hit, LayerMask::Ground)) 
            actorTarget.y = hit.point.y;
        
        // Resolve velocity
        actorPosition += (actorVelocity * 2.0f);
        
        // Sync actor position
        actor->navigation.mPosition = actorPosition;
        actor->navigation.mRotation = actorRotation;
        actor->navigation.mVelocity = actorVelocity;
        actor->navigation.mTargetPoint = actorTarget;
    }
    
    return;
}


void EngineSystemManager::GenerateCollider(Actor* actor) {
    /*
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
    */
    return;
}

