#include <GameEngineFramework/Engine/EngineSystems.h>
#include <mutex>

void EngineSystemManager::UpdateKinematics(unsigned int index) {
    unsigned int numberOfActors = AI.GetNumberOfActors();
    
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
        
        // Check not on ground
        if (Physics.Raycast(actorPosition, glm::vec3(0, -1, 0), 2000, hit, LayerMask::Ground)) 
            actorPosition.y = hit.point.y;
        
        // Resolve velocity
        actorPosition += actorVelocity * 2.0f;
        
        // Sync actor position
        actor->navigation.mPosition = actorPosition;
    }
    
    // Process dead renderers into physical objects
    if (AI.GetNumberOfDeadRenderers() > 0) {
        
        std::vector<MeshRenderer*> deadRendererList;
        AI.SwapDeadRendererList(deadRendererList);
        
        for (unsigned int r=0; r < deadRendererList.size(); r++) {
            MeshRenderer* deadRenderer = deadRendererList[r];
            
            sceneMain->RemoveMeshRendererFromSceneRoot(deadRenderer, RENDER_QUEUE_GEOMETRY);
            deadRenderer->isActive = false;
            
            Renderer.DestroyMeshRenderer(deadRenderer);
            
            /*
            
            GameObject* gameObject = Create<GameObject>();
            gameObject->AddComponent( CreateComponentFromObject<MeshRenderer>(deadRenderer) );
            gameObject->AddComponent( CreateComponent<rp3d::RigidBody>() );
            
            Transform* transform = gameObject->GetComponent<Transform>();
            transform->position = deadRenderer->transform.position;
            transform->rotation = deadRenderer->transform.rotation;
            
            transform->scale.x = glm::length(glm::vec3(deadRenderer->transform.matrix[0]));
            transform->scale.y = glm::length(glm::vec3(deadRenderer->transform.matrix[1]));
            transform->scale.z = glm::length(glm::vec3(deadRenderer->transform.matrix[2]));
            transform->UpdateMatrix();
            
            rp3d::RigidBody* rigidBody = gameObject->GetComponent<rp3d::RigidBody>();
            rp3d::Collider* collider = rigidBody->addCollider(Physics.GetColliderBox(transform->scale / 2.0f), rp3d::Transform::identity());
            rp3d::Transform bodyTransform = rigidBody->getTransform();
            
            bodyTransform.setFromOpenGL( &deadRenderer->transform.matrix[0][0] );
            
            collider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
            
            rigidBody->setTransform(bodyTransform);
            rigidBody->enableGravity(true);
            rigidBody->updateMassPropertiesFromColliders();
            
            */
            
        }
    }
}
