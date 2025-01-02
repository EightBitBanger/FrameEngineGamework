#include <GameEngineFramework/Engine/EngineSystems.h>

#include <GameEngineFramework/Engine/EngineSystems.h>

// Function to clear old genetic renderers
void EngineSystemManager::ClearOldGeneticRenderers(unsigned int index) {
    
    for (unsigned int a = 0; a < mStreamBuffer[index].actor->mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        
        sceneMain->RemoveMeshRendererFromSceneRoot(geneRenderer, RENDER_QUEUE_GEOMETRY);
        
        Renderer.DestroyMeshRenderer(geneRenderer);
    }
    
    mStreamBuffer[index].actor->mGeneticRenderers.clear();
    mStreamBuffer[index].actor->mAnimationStates.clear();
}

// Function to generate collider
void EngineSystemManager::GenerateCollider(unsigned int index) {
    
    if (mStreamBuffer[index].rigidBody == nullptr) 
        return;
    
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

// Function to create a new mesh renderer for a gene
MeshRenderer* EngineSystemManager::CreateMeshRendererForGene(unsigned int index, unsigned int geneIndex, Mesh* sourceMesh) {
    
    MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
    newRenderer->isActive = false;
    newRenderer->EnableFrustumCulling();
    
    newRenderer->mesh = sourceMesh;
    newRenderer->material = Renderer.CreateMaterial();
    newRenderer->material->isShared = false;
    
    newRenderer->material->diffuse.r = mStreamBuffer[index].actor->mGenes[geneIndex].color.x;
    newRenderer->material->diffuse.g = mStreamBuffer[index].actor->mGenes[geneIndex].color.y;
    newRenderer->material->diffuse.b = mStreamBuffer[index].actor->mGenes[geneIndex].color.z;
    
    newRenderer->material->ambient = Colors.white;
    newRenderer->material->shader = shaders.color;
    
    newRenderer->material->DisableBlending();
    newRenderer->material->EnableCulling();
    newRenderer->material->EnableDepthTest();
    newRenderer->material->DisableShadowVolumePass();
    
    newRenderer->transform.position = mStreamBuffer[index].actor->mPosition;
    newRenderer->transform.scale = glm::vec3(mStreamBuffer[index].actor->mGenes[geneIndex].scale.x,
                                             mStreamBuffer[index].actor->mGenes[geneIndex].scale.y,
                                             mStreamBuffer[index].actor->mGenes[geneIndex].scale.z);
    
    newRenderer->transform.RotateAxis(mStreamBuffer[index].actor->mGenes[geneIndex].rotation.x, glm::vec3(1, 0, 0));
    newRenderer->transform.RotateAxis(mStreamBuffer[index].actor->mGenes[geneIndex].rotation.y, glm::vec3(0, 1, 0));
    newRenderer->transform.RotateAxis(mStreamBuffer[index].actor->mGenes[geneIndex].rotation.z, glm::vec3(0, 0, 1));
    
    return newRenderer;
}

// Function to update genetic attributes
void EngineSystemManager::UpdateGeneticAttributes(unsigned int index, unsigned int geneIndex, unsigned int attachmentIndex) {
    
    mStreamBuffer[index].actor->mGenes[geneIndex].offset = mStreamBuffer[index].actor->mGenes[attachmentIndex].offset;
    mStreamBuffer[index].actor->mGenes[geneIndex].color = mStreamBuffer[index].actor->mGenes[attachmentIndex].color;
    
    mStreamBuffer[index].actor->mGenes[geneIndex].animationAxis = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationAxis;
    mStreamBuffer[index].actor->mGenes[geneIndex].animationRange = mStreamBuffer[index].actor->mGenes[attachmentIndex].animationRange;
    
    mStreamBuffer[index].actor->mGenes[geneIndex].doAnimationCycle = mStreamBuffer[index].actor->mGenes[attachmentIndex].doAnimationCycle;
    mStreamBuffer[index].actor->mGenes[geneIndex].doInverseAnimation = mStreamBuffer[index].actor->mGenes[attachmentIndex].doInverseAnimation;
    
    return;
}

// Function to update actor genetics
void EngineSystemManager::UpdateActorGenetics(unsigned int index) {
    
    if (!mStreamBuffer[index].actor->mDoUpdateGenetics)
        return;
    
    ClearOldGeneticRenderers(index);
    GenerateCollider(index);
    
    unsigned int numberOfGenes = mStreamBuffer[index].actor->mGenes.size();
    
    for (unsigned int a = 0; a < numberOfGenes; a++) {
        
        if (!mStreamBuffer[index].actor->mGenes[a].doExpress)
            continue;
        
        MeshRenderer* newRenderer = CreateMeshRendererForGene(index, a, meshes.cube);
        
        if (mStreamBuffer[index].actor->mGenes[a].attachmentIndex > 0) {
            
            unsigned int attachmentIndex = mStreamBuffer[index].actor->mGenes[a].attachmentIndex - 1;
            
            UpdateGeneticAttributes(index, a, attachmentIndex);
        }
        
        glm::vec4 orientation = glm::vec4(Transform().rotation.w, Transform().rotation.x, Transform().rotation.y, Transform().rotation.z);
        mStreamBuffer[index].actor->mGeneticRenderers.push_back(newRenderer);
        mStreamBuffer[index].actor->mAnimationStates.push_back(orientation);
        
        sceneMain->AddMeshRendererToSceneRoot(newRenderer, RENDER_QUEUE_GEOMETRY);
    }
    
    mStreamBuffer[index].actor->mDoUpdateGenetics = false;
    
    return;
}

// Function to express actor genetics
void EngineSystemManager::ExpressActorGenetics(unsigned int index) {
    
    if (!mStreamBuffer[index].actor->mDoReexpressGenetics)
        return;
    
    unsigned int numberOfGenes = mStreamBuffer[index].actor->mGenes.size();
    unsigned int numberOfRenderers = mStreamBuffer[index].actor->mGeneticRenderers.size();
    
    if (numberOfRenderers != numberOfGenes) {
        mStreamBuffer[index].actor->mDoUpdateGenetics = true;
        UpdateActorGenetics(index);
        return;
    }
    
    for (unsigned int a = 0; a < numberOfRenderers; a++) {
        if (!mStreamBuffer[index].actor->mGenes[a].doExpress)
            continue;
        
        MeshRenderer* meshRenderer = mStreamBuffer[index].actor->mGeneticRenderers[a];
        meshRenderer->material->ambient = Colors.white;
        meshRenderer->material->diffuse = Color(mStreamBuffer[index].actor->mGenes[a].color.x,
                                                mStreamBuffer[index].actor->mGenes[a].color.y,
                                                mStreamBuffer[index].actor->mGenes[a].color.z);
        
        meshRenderer->transform.position = glm::vec3(mStreamBuffer[index].actor->mPosition.x,
                                                     mStreamBuffer[index].actor->mPosition.y,
                                                     mStreamBuffer[index].actor->mPosition.z);
        
        meshRenderer->transform.scale = glm::clamp(glm::vec3(mStreamBuffer[index].actor->mGenes[a].scale.x,
                                                             mStreamBuffer[index].actor->mGenes[a].scale.y,
                                                             mStreamBuffer[index].actor->mGenes[a].scale.z), 0.0f, 2.0f);
        
        if (mStreamBuffer[index].actor->mGenes[a].attachmentIndex > 0) {
            unsigned int attachmentIndex = mStreamBuffer[index].actor->mGenes[a].attachmentIndex - 1;
            UpdateGeneticAttributes(index, a, attachmentIndex);
        }
        
        meshRenderer->transform.RotateAxis(mStreamBuffer[index].actor->mGenes[a].rotation.x, glm::vec3(1, 0, 0));
        meshRenderer->transform.RotateAxis(mStreamBuffer[index].actor->mGenes[a].rotation.y, glm::vec3(0, 1, 0));
        meshRenderer->transform.RotateAxis(mStreamBuffer[index].actor->mGenes[a].rotation.z, glm::vec3(0, 0, 1));
    }
    
    mStreamBuffer[index].actor->mDoReexpressGenetics = false;
    
    return;
}

