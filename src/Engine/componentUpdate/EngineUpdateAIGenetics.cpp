#include <GameEngineFramework/Engine/EngineSystems.h>

#include <GameEngineFramework/Engine/EngineSystems.h>

// Function to clear old genetic renderers
void EngineSystemManager::ClearOldGeneticRenderers(unsigned int index) {
    
    for (unsigned int a = 0; a < mStreamBuffer[index].actor->genetics.mGeneticRenderers.size(); a++) {
        
        MeshRenderer* geneRenderer = mStreamBuffer[index].actor->genetics.mGeneticRenderers[a];
        
        sceneMain->RemoveMeshRendererFromSceneRoot(geneRenderer, RENDER_QUEUE_GEOMETRY);
        
        Renderer.DestroyMeshRenderer(geneRenderer);
    }
    
    mStreamBuffer[index].actor->genetics.mGeneticRenderers.clear();
    mStreamBuffer[index].actor->state.mAnimation.clear();
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
    
    newRenderer->material->ambient = Colors.white;
    newRenderer->material->shader = shaders.color;
    
    newRenderer->material->DisableBlending();
    newRenderer->material->EnableCulling();
    newRenderer->material->EnableDepthTest();
    newRenderer->material->DisableShadowVolumePass();
    
    newRenderer->transform.position = mStreamBuffer[index].actor->navigation.mPosition;
    
    newRenderer->transform.scale = glm::vec3(mStreamBuffer[index].actor->genetics.mGenes[geneIndex].scale.x,
                                             mStreamBuffer[index].actor->genetics.mGenes[geneIndex].scale.y,
                                             mStreamBuffer[index].actor->genetics.mGenes[geneIndex].scale.z);
    
    return newRenderer;
}


// Initiate an actors genetic expression by rebuilding the render components
void EngineSystemManager::UpdateActorGenetics(unsigned int index) {
    
    if (!mStreamBuffer[index].actor->genetics.mDoUpdateGenetics)
        return;
    
    mStreamBuffer[index].actor->genetics.mDoUpdateGenetics = false;
    
    ClearOldGeneticRenderers(index);
    GenerateCollider(index);
    
    unsigned int numberOfGenes = mStreamBuffer[index].actor->genetics.mGenes.size();
    bool sexualOrientation = mStreamBuffer[index].actor->physical.GetSexualOrientation();
    
    for (unsigned int a = 0; a < numberOfGenes; a++) {
        
        // Check should express
        if (!mStreamBuffer[index].actor->genetics.mGenes[a].doExpress)
            continue;
        
        MeshRenderer* newRenderer = CreateMeshRendererForGene(index, a, meshes.cube);
        
        // Check offset attachment
        if (mStreamBuffer[index].actor->genetics.mGenes[a].attachmentIndex != 0) {
            unsigned int attachmentIndex = mStreamBuffer[index].actor->genetics.mGenes[a].attachmentIndex - 1;
            mStreamBuffer[index].actor->genetics.mGenes[a].offset = mStreamBuffer[index].actor->genetics.mGenes[attachmentIndex].offset;
        }
        
        glm::vec4 orientation = glm::vec4(Transform().rotation.w, Transform().rotation.x, Transform().rotation.y, Transform().rotation.z);
        mStreamBuffer[index].actor->genetics.mGeneticRenderers.push_back(newRenderer);
        mStreamBuffer[index].actor->state.mAnimation.push_back(orientation);
        
        sceneMain->AddMeshRendererToSceneRoot(newRenderer, RENDER_QUEUE_GEOMETRY);
    }
    
    // Generate and push back blank phenotypes and biologicals for each gene
    Phen dummyPhenotype;
    if (mStreamBuffer[index].actor->genetics.mPhen.size() == 0) {
        for (unsigned int a = 0; a < numberOfGenes; a++) 
            mStreamBuffer[index].actor->genetics.mPhen.push_back(dummyPhenotype);
    }
    
    return;
}

// Express an actors genetics by calculating the phenotypic expression from its genome as well as its environment and state
void EngineSystemManager::ExpressActorGenetics(unsigned int index) {
    
    if (!mStreamBuffer[index].actor->genetics.mDoReexpressGenetics)
        return;
    
    mStreamBuffer[index].actor->genetics.mDoReexpressGenetics = false;
    
    unsigned int numberOfGenes = mStreamBuffer[index].actor->genetics.mGenes.size();
    unsigned int numberOfRenderers = mStreamBuffer[index].actor->genetics.mGeneticRenderers.size();
    
    // Trigger a full rebuild if any genes where added to or
    // removed from the genome
    if (numberOfRenderers != numberOfGenes) {
        mStreamBuffer[index].actor->genetics.mDoUpdateGenetics = true;
        mStreamBuffer[index].actor->genetics.mDoReexpressGenetics = true;
        return;
    }
    
    for (unsigned int a = 0; a < numberOfRenderers; a++) {
        // Check should express
        if (!mStreamBuffer[index].actor->genetics.mGenes[a].doExpress)
            continue;
        
        MeshRenderer* meshRenderer = mStreamBuffer[index].actor->genetics.mGeneticRenderers[a];
        
        // Genetic color
        meshRenderer->material->diffuse = Color(mStreamBuffer[index].actor->genetics.mGenes[a].color.x,
                                                mStreamBuffer[index].actor->genetics.mGenes[a].color.y,
                                                mStreamBuffer[index].actor->genetics.mGenes[a].color.z);
        
        meshRenderer->transform.position = glm::vec3(mStreamBuffer[index].actor->navigation.mPosition.x,
                                                     mStreamBuffer[index].actor->navigation.mPosition.y,
                                                     mStreamBuffer[index].actor->navigation.mPosition.z);
        
        // Check scale inheritance
        if (mStreamBuffer[index].actor->genetics.mGenes[a].scaleIndex == 0) {
            
            // Default scale
            meshRenderer->transform.scale = glm::clamp(glm::vec3(mStreamBuffer[index].actor->genetics.mGenes[a].scale.x,
                                                                 mStreamBuffer[index].actor->genetics.mGenes[a].scale.y,
                                                                 mStreamBuffer[index].actor->genetics.mGenes[a].scale.z), 0.0f, 2.0f);
            
        } else {
            
            // Inherit scale from another gene
            unsigned int scaleIndex = mStreamBuffer[index].actor->genetics.mGenes[a].scaleIndex - 1;
            meshRenderer->transform.scale = glm::clamp(glm::vec3(mStreamBuffer[index].actor->genetics.mGenes[scaleIndex].scale.x,
                                                                 mStreamBuffer[index].actor->genetics.mGenes[scaleIndex].scale.y,
                                                                 mStreamBuffer[index].actor->genetics.mGenes[scaleIndex].scale.z), 0.0f, 2.0f);
            
        }
        
        // Check custom expression
        if (mStreamBuffer[index].actor->genetics.mGenes[a].type != EXPRESSION_TYPE_BASE) {
            
            bool sexualOrientation = mStreamBuffer[index].actor->physical.GetSexualOrientation();
            
            // Check to disable genes that do not apply to this actor
            if ((mStreamBuffer[index].actor->genetics.mGenes[a].type == EXPRESSION_TYPE_MALE && sexualOrientation == false) || 
                (mStreamBuffer[index].actor->genetics.mGenes[a].type == EXPRESSION_TYPE_FEMALE && sexualOrientation == true)) {
                
                meshRenderer->isActive = false;
            }
            
            // Process age dependent genes
            if (mStreamBuffer[index].actor->physical.mAge > mStreamBuffer[index].actor->genetics.mGenes[a].expressionBegin && 
                mStreamBuffer[index].actor->physical.mAge < mStreamBuffer[index].actor->genetics.mGenes[a].expressionEnd) {
                
                // Express only genes that apply to this actor
                if ((mStreamBuffer[index].actor->genetics.mGenes[a].type == EXPRESSION_TYPE_MALE && sexualOrientation == true) || 
                    (mStreamBuffer[index].actor->genetics.mGenes[a].type == EXPRESSION_TYPE_FEMALE && sexualOrientation == false)) {
                    
                    float expressionFactor = mStreamBuffer[index].actor->genetics.mGenes[a].expressionFactor;
                    
                    // Modify phenotypic expression
                    mStreamBuffer[index].actor->genetics.mPhen[a].scale.x += expressionFactor;
                    mStreamBuffer[index].actor->genetics.mPhen[a].scale.y += expressionFactor;
                    mStreamBuffer[index].actor->genetics.mPhen[a].scale.z += expressionFactor;
                    
                }
                
            }
            
            float maxUniformScale = mStreamBuffer[index].actor->genetics.mGenes[a].expressionMax;
            
            // Apply phenotypic expression
            meshRenderer->transform.scale *= glm::clamp(glm::vec3(mStreamBuffer[index].actor->genetics.mPhen[a].scale.x, 
                                                                  mStreamBuffer[index].actor->genetics.mPhen[a].scale.y, 
                                                                  mStreamBuffer[index].actor->genetics.mPhen[a].scale.z), 
                                                                  0.0f, maxUniformScale);
            
        }
        
        // Apply offset positioning
        if (mStreamBuffer[index].actor->genetics.mGenes[a].attachmentIndex > 0) {
            
            unsigned int attachmentIndex = mStreamBuffer[index].actor->genetics.mGenes[a].attachmentIndex - 1;
            
            mStreamBuffer[index].actor->genetics.mGenes[a].offset = mStreamBuffer[index].actor->genetics.mGenes[attachmentIndex].offset;
        }
        
        // Genetic orientation
        meshRenderer->transform.RotateAxis(mStreamBuffer[index].actor->genetics.mGenes[a].rotation.x, glm::vec3(1, 0, 0));
        meshRenderer->transform.RotateAxis(mStreamBuffer[index].actor->genetics.mGenes[a].rotation.y, glm::vec3(0, 1, 0));
        meshRenderer->transform.RotateAxis(mStreamBuffer[index].actor->genetics.mGenes[a].rotation.z, glm::vec3(0, 0, 1));
        
        // TODO: Apply biological effectors
        
        continue;
    }
    
    return;
}



