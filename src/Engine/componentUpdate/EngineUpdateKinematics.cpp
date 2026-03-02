#include <GameEngineFramework/Engine/EngineSystems.h>
#include <mutex>

void EngineSystemManager::UpdateKinematics(unsigned int index) {
    unsigned int numberOfActors = AI.GetNumberOfActors();
    
    // Animation states
    
    for (unsigned int i=0; i < numberOfActors; i++) {
        Actor* actor = AI.GetActor(i);
        
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
    
    // Process dead renderers into physical objects
    mux.unlock();
    
    while (AI.GetNumberOfDeadRenderers() > 0) {
        MeshRenderer* deadRenderer = AI.RemoveDeadRenderer(0);
        
        GameObject* gameObject = Create<GameObject>();
        gameObject->AddComponent( CreateComponentFromObject<MeshRenderer>(deadRenderer) );
        gameObject->AddComponent( CreateComponent<rp3d::RigidBody>() );
        
        Transform* transform = gameObject->GetComponent<Transform>();
        
        transform->scale.x = glm::length(glm::vec3(deadRenderer->transform.matrix[0]));
        transform->scale.y = glm::length(glm::vec3(deadRenderer->transform.matrix[1]));
        transform->scale.z = glm::length(glm::vec3(deadRenderer->transform.matrix[2]));
        
        rp3d::RigidBody* rigidBody = gameObject->GetComponent<rp3d::RigidBody>();
        rp3d::Collider* collider = rigidBody->addCollider(Physics.GetColliderBox(transform->scale / 2.0f), rp3d::Transform::identity());
        rp3d::Transform bodyTransform = rigidBody->getTransform();
        
        bodyTransform.setFromOpenGL( &deadRenderer->transform.matrix[0][0] );
        
        collider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
        
        rigidBody->setTransform(bodyTransform);
        rigidBody->enableGravity(true);
        rigidBody->updateMassPropertiesFromColliders();
    }
    mux.lock();
}
