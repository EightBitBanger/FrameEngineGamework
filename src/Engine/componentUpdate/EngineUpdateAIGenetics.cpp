#include <GameEngineFramework/Engine/EngineSystems.h>


void EngineSystemManager::UpdateActorGenetics(unsigned int index) {
    
    if (!mStreamBuffer[index].actor->mDoUpdateGenetics) 
        return;
    
    // Destroy and clear old genetic renderers
    
    for (unsigned int a=0; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        
        sceneMain->RemoveMeshRendererFromSceneRoot( geneRenderer, RENDER_QUEUE_GEOMETRY );
        
        Renderer.DestroyMeshRenderer( geneRenderer );
        
        continue;
    }
    
    mStreamBuffer[index].actor->mGeneticRenderers.clear();
    mStreamBuffer[index].actor->mAnimationStates.clear();
    
    // Generate collider
    
    if (mStreamBuffer[index].rigidBody != nullptr) {
        
        rp3d::RigidBody* rigidBody = mStreamBuffer[index].rigidBody;
        unsigned int numberOfColliders = rigidBody->getNbColliders();
        
        if (numberOfColliders == 0) {
            
            rp3d::Transform colliderOffset(rp3d::Transform::identity());
            rp3d::Collider* collider = rigidBody->addCollider(colliders.box, colliderOffset);
            
            collider->setCollisionCategoryBits( (unsigned short)LayerMask::Actor );
            collider->setCollideWithMaskBits( (unsigned short)CollisionMask::Entity );
            
            collider->setUserData( (void*)mStreamBuffer[index].gameObject );
            rigidBody->setUserData( (void*)mStreamBuffer[index].gameObject );
            
        }
        
    }
    
    // Create and express genetic elements
    
    unsigned int numberOfGenes = mStreamBuffer[index].actor->mGenes.size();
    
    for (unsigned int a=0; a < numberOfGenes; a++) {
        
        if (!mStreamBuffer[index].actor->mGenes[a].doExpress) 
            continue;
        
        // Generate a new mesh renderer for the gene
        
        MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
        newRenderer->isActive = false;
        newRenderer->EnableFrustumCulling();
        
        // TODO: Allow different mesh types
        newRenderer->mesh     = meshes.cube;
        newRenderer->material = Renderer.CreateMaterial();
        
        newRenderer->material->isShared = false;
        
        newRenderer->material->diffuse.r = mStreamBuffer[index].actor->mGenes[a].color.x;
        newRenderer->material->diffuse.g = mStreamBuffer[index].actor->mGenes[a].color.y;
        newRenderer->material->diffuse.b = mStreamBuffer[index].actor->mGenes[a].color.z;
        newRenderer->material->ambient = Colors.white;
        
        newRenderer->material->shader = shaders.color;
        
        newRenderer->material->DisableBlending();
        newRenderer->material->EnableCulling();
        newRenderer->material->EnableDepthTest();
        
        newRenderer->material->DisableShadowVolumePass();
        
        // Position
        newRenderer->transform.position = mStreamBuffer[index].actor->mPosition;
        
        // Offset position
        glm::vec3 offset(0);
        glm::vec4 orientation(0);
        
        if (mStreamBuffer[index].actor->mGenes[a].attachmentIndex > 0) {
            
            unsigned int attachmentIndex = mStreamBuffer[index].actor->mGenes[a].attachmentIndex - 1;
            
            // Position
            mStreamBuffer[index].actor->mGenes[a].offset.x = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.x;
            mStreamBuffer[index].actor->mGenes[a].offset.y = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.y;
            mStreamBuffer[index].actor->mGenes[a].offset.z = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.z;
            
            // Color
            newRenderer->material->diffuse.r = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.x;
            newRenderer->material->diffuse.g = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.y;
            newRenderer->material->diffuse.b = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.z;
            
            // Animation state
            mStreamBuffer[index].actor->mGenes[a].animationAxis = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationAxis;
            mStreamBuffer[index].actor->mGenes[a].animationRange = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationRange;
            mStreamBuffer[index].actor->mGenes[a].doAnimationCycle = mStreamBuffer[index].actor->mGenes[attachmentIndex].doAnimationCycle;
            mStreamBuffer[index].actor->mGenes[a].doInverseAnimation = mStreamBuffer[index].actor->mGenes[attachmentIndex].doInverseAnimation;
            
        }
        
        // Scale
        newRenderer->transform.scale.x = mStreamBuffer[index].actor->mGenes[a].scale.x;
        newRenderer->transform.scale.y = mStreamBuffer[index].actor->mGenes[a].scale.y;
        newRenderer->transform.scale.z = mStreamBuffer[index].actor->mGenes[a].scale.z;
        
        // Clamp scale
        newRenderer->transform.scale = glm::clamp(newRenderer->transform.scale, 0.0f, 2.0f);
        
        // Rotation
        newRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.x, glm::vec3(1, 0, 0) );
        newRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.y, glm::vec3(0, 1, 0) );
        newRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.z, glm::vec3(0, 0, 1) );
        
        // Default animation orientation
        Transform transform;
        
        orientation = glm::vec4( transform.rotation.w, 
                                 transform.rotation.x, 
                                 transform.rotation.y, 
                                 transform.rotation.z );
        
        mStreamBuffer[index].actor->mGeneticRenderers.push_back( newRenderer );
        mStreamBuffer[index].actor->mAnimationStates .push_back( orientation );
        
        sceneMain->AddMeshRendererToSceneRoot( newRenderer, RENDER_QUEUE_GEOMETRY );
        
        continue;
    }
    
    mStreamBuffer[index].actor->mDoUpdateGenetics = false;
    
    return;
}


void EngineSystemManager::ExpressActorGenetics(unsigned int index) {
    
    if (!mStreamBuffer[index].actor->mDoReexpressGenetics) 
        return;
    
    unsigned int numberOfGenes = mStreamBuffer[index].actor->mGenes.size();
    
    unsigned int numberOfRenderers = mStreamBuffer[index].actor->mGeneticRenderers.size();
    
    // Generate mesh renderers for each gene
    
    if (numberOfRenderers != numberOfGenes) {
        
        mStreamBuffer[index].actor->mDoUpdateGenetics = true;
        
        UpdateActorGenetics(index);
        
        return;
    }
    
    
    // Express genetic elements
    
    for (unsigned int a=0; a < numberOfRenderers; a++) {
        
        if (!mStreamBuffer[index].actor->mGenes[a].doExpress) 
            continue;
        
        MeshRenderer* meshRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        
        // Set renderer color
        meshRenderer->material->ambient = Colors.white;
        
        meshRenderer->material->diffuse.r = mStreamBuffer[index].actor->mGenes[a].color.x;
        meshRenderer->material->diffuse.g = mStreamBuffer[index].actor->mGenes[a].color.y;
        meshRenderer->material->diffuse.b = mStreamBuffer[index].actor->mGenes[a].color.z;
        
        
        // Position
        meshRenderer->transform.position = mStreamBuffer[index].actor->mPosition;
        
        // Offset position
        glm::vec3 offset(0);
        
        if (mStreamBuffer[index].actor->mGenes[a].attachmentIndex > 0) {
            
            unsigned int attachmentIndex = mStreamBuffer[index].actor->mGenes[a].attachmentIndex - 1;
            
            // Position
            mStreamBuffer[index].actor->mGenes[a].offset.x = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.x;
            mStreamBuffer[index].actor->mGenes[a].offset.y = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.y;
            mStreamBuffer[index].actor->mGenes[a].offset.z = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.z;
            
            // Color
            meshRenderer->material->diffuse.r = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.x;
            meshRenderer->material->diffuse.g = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.y;
            meshRenderer->material->diffuse.b = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.z;
            
            // Animation state
            mStreamBuffer[index].actor->mGenes[a].animationAxis = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationAxis;
            mStreamBuffer[index].actor->mGenes[a].animationRange = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationRange;
            mStreamBuffer[index].actor->mGenes[a].doAnimationCycle = mStreamBuffer[index].actor->mGenes[attachmentIndex].doAnimationCycle;
            mStreamBuffer[index].actor->mGenes[a].doInverseAnimation = mStreamBuffer[index].actor->mGenes[attachmentIndex].doInverseAnimation;
            
        }
        
        // Scale
        meshRenderer->transform.scale.x = mStreamBuffer[index].actor->mGenes[a].scale.x;
        meshRenderer->transform.scale.y = mStreamBuffer[index].actor->mGenes[a].scale.y;
        meshRenderer->transform.scale.z = mStreamBuffer[index].actor->mGenes[a].scale.z;
        
        // Clamp scale
        meshRenderer->transform.scale = glm::clamp(meshRenderer->transform.scale, 0.0f, 2.0f);
        
        // Rotation
        meshRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.x, glm::vec3(1, 0, 0) );
        meshRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.y, glm::vec3(0, 1, 0) );
        meshRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.z, glm::vec3(0, 0, 1) );
        
        continue;
    }
    
    
    
    /*
    
    if (!mStreamBuffer[index].actor->mDoReexpressGenetics) 
        return;
    
    unsigned int numberOfGenes = mStreamBuffer[index].actor->mGenes.size();
    
    unsigned int numberOfRenderers = mStreamBuffer[index].actor->mGeneticRenderers.size();
    
    // Generate mesh renderers for each gene
    
    while (numberOfRenderers < numberOfGenes) {
        
        MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
        
        newRenderer->isActive = false;
        newRenderer->EnableFrustumCulling();
        
        newRenderer->material = Renderer.CreateMaterial();
        
        newRenderer->material->isShared = false;
        
        newRenderer->material->shader = shaders.color;
        
        newRenderer->material->DisableBlending();
        newRenderer->material->EnableCulling();
        newRenderer->material->EnableDepthTest();
        
        newRenderer->material->DisableShadowVolumePass();
        
        // TODO: Allow different mesh types
        newRenderer->mesh = meshes.cube;
        
        // Default animation state orientation
        Transform transform;
        glm::vec4 orientation(0);
        
        orientation = glm::vec4( transform.rotation.w, 
                                 transform.rotation.x, 
                                 transform.rotation.y, 
                                 transform.rotation.z );
        
        mStreamBuffer[index].actor->mGeneticRenderers.push_back( newRenderer );
        mStreamBuffer[index].actor->mAnimationStates .push_back( orientation );
        
        continue;
    }
    
    
    
    
    // Zero the animation states
    
    unsigned int numberOfAnimationStates = mStreamBuffer[index].actor->mAnimationStates.size();
    
    for (unsigned int i=0; i < numberOfAnimationStates; i++) {
        
        Transform transform;
        
        glm::vec4 orientation;
        orientation = glm::vec4( transform.rotation.w, 
                                 transform.rotation.x, 
                                 transform.rotation.y, 
                                 transform.rotation.z );
        
        mStreamBuffer[index].actor->mAnimationStates[i] = orientation;
        
        continue;
    }
    
    
    
    
    // Generate collider
    
    if (mStreamBuffer[index].rigidBody != nullptr) {
        
        rp3d::RigidBody* rigidBody = mStreamBuffer[index].rigidBody;
        unsigned int numberOfColliders = rigidBody->getNbColliders();
        
        if (numberOfColliders == 0) {
            
            rp3d::Transform colliderOffset(rp3d::Transform::identity());
            rp3d::Collider* collider = rigidBody->addCollider(colliders.box, colliderOffset);
            
            collider->setCollisionCategoryBits( (unsigned short)LayerMask::Actor );
            collider->setCollideWithMaskBits( (unsigned short)CollisionMask::Entity );
            
            collider->setUserData( (void*)mStreamBuffer[index].gameObject );
            rigidBody->setUserData( (void*)mStreamBuffer[index].gameObject );
            
        }
        
    }
    
    // Express genetic elements
    
    unsigned int numberOfMeshRenderers = mStreamBuffer[index].actor->mGeneticRenderers.size();
    
    for (unsigned int a=0; a < numberOfMeshRenderers; a++) {
        
        // Remove extra renderers
        if (a >= numberOfGenes) {
            
            sceneMain->RemoveMeshRendererFromSceneRoot( mStreamBuffer[index].actor->mGeneticRenderers[a], RENDER_QUEUE_GEOMETRY );
            
            continue;
        }
        
        if (!mStreamBuffer[index].actor->mGenes[a].doExpress) 
            continue;
        
        MeshRenderer* meshRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        
        sceneMain->AddMeshRendererToSceneRoot( meshRenderer, RENDER_QUEUE_GEOMETRY );
        
        // Set renderer color
        meshRenderer->material->ambient = Colors.white;
        
        meshRenderer->material->diffuse.r = mStreamBuffer[index].actor->mGenes[a].color.x;
        meshRenderer->material->diffuse.g = mStreamBuffer[index].actor->mGenes[a].color.y;
        meshRenderer->material->diffuse.b = mStreamBuffer[index].actor->mGenes[a].color.z;
        
        
        // Position
        meshRenderer->transform.position = mStreamBuffer[index].actor->mPosition;
        
        // Offset position
        glm::vec3 offset(0);
        
        if (mStreamBuffer[index].actor->mGenes[a].attachmentIndex > 0) {
            
            unsigned int attachmentIndex = mStreamBuffer[index].actor->mGenes[a].attachmentIndex - 1;
            
            // Position
            mStreamBuffer[index].actor->mGenes[a].offset.x = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.x;
            mStreamBuffer[index].actor->mGenes[a].offset.y = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.y;
            mStreamBuffer[index].actor->mGenes[a].offset.z = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset.z;
            
            // Color
            meshRenderer->material->diffuse.r = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.x;
            meshRenderer->material->diffuse.g = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.y;
            meshRenderer->material->diffuse.b = mStreamBuffer[index].actor->mGenes[attachmentIndex].color.z;
            
            // Animation state
            mStreamBuffer[index].actor->mGenes[a].animationAxis = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationAxis;
            mStreamBuffer[index].actor->mGenes[a].animationRange = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationRange;
            mStreamBuffer[index].actor->mGenes[a].doAnimationCycle = mStreamBuffer[index].actor->mGenes[attachmentIndex].doAnimationCycle;
            mStreamBuffer[index].actor->mGenes[a].doInverseAnimation = mStreamBuffer[index].actor->mGenes[attachmentIndex].doInverseAnimation;
            
        }
        
        // Scale
        meshRenderer->transform.scale.x = mStreamBuffer[index].actor->mGenes[a].scale.x;
        meshRenderer->transform.scale.y = mStreamBuffer[index].actor->mGenes[a].scale.y;
        meshRenderer->transform.scale.z = mStreamBuffer[index].actor->mGenes[a].scale.z;
        
        // Clamp scale
        meshRenderer->transform.scale = glm::clamp(meshRenderer->transform.scale, 0.0f, 2.0f);
        
        // Rotation
        meshRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.x, glm::vec3(1, 0, 0) );
        meshRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.y, glm::vec3(0, 1, 0) );
        meshRenderer->transform.RotateAxis( mStreamBuffer[index].actor->mGenes[a].rotation.z, glm::vec3(0, 0, 1) );
        
        continue;
    }
    
    */
    
    mStreamBuffer[index].actor->mDoReexpressGenetics = false;
    
    return;
}



