#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

void ActorSystem::UpdateActorGenetics(Actor* actor) {
    if (!actor->genetics.mDoUpdateGenetics)
        return;
    actor->genetics.mDoUpdateGenetics = false;
    
    ClearOldGeneticRenderers(actor);
    
    unsigned int numberOfGenes = actor->genetics.mGenes.size();
    bool sexualOrientation = actor->physical.GetSexualOrientation();
    
    for (unsigned int a = 0; a < numberOfGenes; a++) {
        // Check should express
        if (!actor->genetics.mGenes[a].doExpress)
            continue;
        
        // Check offset attachment
        if (actor->genetics.mGenes[a].attachmentIndex != 0) {
            unsigned int attachmentIndex = actor->genetics.mGenes[a].attachmentIndex - 1;
            actor->genetics.mGenes[a].offset = actor->genetics.mGenes[attachmentIndex].offset;
        }
        
        MeshRenderer* newRenderer = CreateMeshRendererForGene(actor, a, baseMesh);
        newRenderer->isActive = true;
        
        glm::vec4 orientation = glm::vec4(Transform().rotation.w, Transform().rotation.x, Transform().rotation.y, Transform().rotation.z);
        actor->genetics.mGeneticRenderers.push_back(newRenderer);
        actor->state.mAnimation.push_back(orientation);
        
        sceneMain->AddMeshRendererToSceneRoot(newRenderer, RENDER_QUEUE_GEOMETRY);
    }
    
    // Generate and push back blank phenotypes and biologicals for each gene
    Phen dummyPhenotype;
    if (actor->genetics.mPhen.size() == 0) {
        for (unsigned int a = 0; a < numberOfGenes; a++) 
            actor->genetics.mPhen.push_back(dummyPhenotype);
    }
    
    return;
}

MeshRenderer* ActorSystem::CreateMeshRendererForGene(Actor* actor, unsigned int geneIndex, Mesh* sourceMesh) {
    MeshRenderer* newRenderer = Renderer.CreateMeshRenderer();
    newRenderer->isActive = false;
    
    newRenderer->mesh = sourceMesh;
    newRenderer->material = Renderer.CreateMaterial();
    newRenderer->material->isShared = false;
    
    newRenderer->material->ambient = Colors.white;
    newRenderer->material->shader = shader;
    
    newRenderer->material->DisableBlending();
    newRenderer->material->EnableCulling();
    newRenderer->material->EnableDepthTest();
    newRenderer->material->DisableShadowVolumePass();
    
    newRenderer->transform.position = actor->navigation.mPosition;
    
    newRenderer->transform.scale = glm::vec3(actor->genetics.mGenes[geneIndex].scale.x,
                                             actor->genetics.mGenes[geneIndex].scale.y,
                                             actor->genetics.mGenes[geneIndex].scale.z);
    
    return newRenderer;
}

void ActorSystem::ClearOldGeneticRenderers(Actor* actor) {
    for (unsigned int a = 0; a < actor->genetics.mGeneticRenderers.size(); a++) {
        MeshRenderer* geneRenderer = actor->genetics.mGeneticRenderers[a];
        sceneMain->RemoveMeshRendererFromSceneRoot(geneRenderer, RENDER_QUEUE_GEOMETRY);
        Renderer.DestroyMeshRenderer(geneRenderer);
    }
    
    actor->genetics.mGeneticRenderers.clear();
    actor->genetics.mPhen.clear();
    actor->state.mAnimation.clear();
    actor->biological.mBiologics.clear();
    return;
}


// Express an actors genetics by calculating the phenotypic expression from its genome as well as its environment and state
void ActorSystem::ExpressActorGenetics(Actor* actor) {
    if (!actor->genetics.mDoReexpressGenetics)
        return;
    actor->genetics.mDoReexpressGenetics = false;
    
    unsigned int numberOfGenes = actor->genetics.mGenes.size();
    unsigned int numberOfRenderers = actor->genetics.mGeneticRenderers.size();
    
    // Trigger a full rebuild if any genes where added to or
    // removed from the genome
    if (numberOfRenderers != numberOfGenes) {
        actor->genetics.mDoUpdateGenetics = true;
        actor->genetics.mDoReexpressGenetics = true;
        return;
    }
    
    for (unsigned int a = 0; a < numberOfRenderers; a++) {
        // Check should express
        if (!actor->genetics.mGenes[a].doExpress)
            continue;
        
        MeshRenderer* meshRenderer = actor->genetics.mGeneticRenderers[a];
        
        // Genetic color
        meshRenderer->material->diffuse = Color(actor->genetics.mGenes[a].color.x,
                                                actor->genetics.mGenes[a].color.y,
                                                actor->genetics.mGenes[a].color.z);
        
        meshRenderer->transform.position = glm::vec3(actor->navigation.mPosition.x,
                                                     actor->navigation.mPosition.y,
                                                     actor->navigation.mPosition.z);
        
        // Check scale inheritance
        if (actor->genetics.mGenes[a].scaleIndex == 0) {
            
            // Default scale
            meshRenderer->transform.scale = glm::clamp(glm::vec3(actor->genetics.mGenes[a].scale.x,
                                                                 actor->genetics.mGenes[a].scale.y,
                                                                 actor->genetics.mGenes[a].scale.z), 0.0f, 2.0f);
            
        } else {
            
            // Inherit scale from another gene
            unsigned int scaleIndex = actor->genetics.mGenes[a].scaleIndex - 1;
            meshRenderer->transform.scale = glm::clamp(glm::vec3(actor->genetics.mGenes[scaleIndex].scale.x,
                                                                 actor->genetics.mGenes[scaleIndex].scale.y,
                                                                 actor->genetics.mGenes[scaleIndex].scale.z), 0.0f, 2.0f);
            
        }
        
        // Check custom expression
        if (actor->genetics.mGenes[a].type != EXPRESSION_TYPE_BASE) {
            
            bool sexualOrientation = actor->physical.GetSexualOrientation();
            
            // Check to disable genes that do not apply to this actor
            if ((actor->genetics.mGenes[a].type == EXPRESSION_TYPE_MALE && sexualOrientation == false) || 
                (actor->genetics.mGenes[a].type == EXPRESSION_TYPE_FEMALE && sexualOrientation == true)) {
                
                meshRenderer->isActive = false;
            }
            
            // Process age dependent genes
            if (actor->physical.mAge > actor->genetics.mGenes[a].expressionAge) {
                
                // Express only genes that apply to this actor
                if ((actor->genetics.mGenes[a].type == EXPRESSION_TYPE_MALE && sexualOrientation == true) || 
                    (actor->genetics.mGenes[a].type == EXPRESSION_TYPE_FEMALE && sexualOrientation == false)) {
                    
                    float expressionFactor = actor->genetics.mGenes[a].expressionFactor;
                    
                    // Modify phenotypic expression
                    actor->genetics.mPhen[a].scale.x += expressionFactor;
                    actor->genetics.mPhen[a].scale.y += expressionFactor;
                    actor->genetics.mPhen[a].scale.z += expressionFactor;
                    
                }
                
            }
            
            float maxUniformScale = actor->genetics.mGenes[a].expressionMax;
            
            // Apply phenotypic expression
            meshRenderer->transform.scale *= glm::clamp(glm::vec3(actor->genetics.mPhen[a].scale.x, 
                                                                  actor->genetics.mPhen[a].scale.y, 
                                                                  actor->genetics.mPhen[a].scale.z), 
                                                                  0.0f, maxUniformScale);
            
        }
        
        // Apply offset positioning
        if (actor->genetics.mGenes[a].attachmentIndex > 0) {
            
            unsigned int attachmentIndex = actor->genetics.mGenes[a].attachmentIndex - 1;
            
            actor->genetics.mGenes[a].offset = actor->genetics.mGenes[attachmentIndex].offset;
        }
        
        // Genetic orientation
        meshRenderer->transform.RotateAxis(actor->genetics.mGenes[a].rotation.x, glm::vec3(1, 0, 0));
        meshRenderer->transform.RotateAxis(actor->genetics.mGenes[a].rotation.y, glm::vec3(0, 1, 0));
        meshRenderer->transform.RotateAxis(actor->genetics.mGenes[a].rotation.z, glm::vec3(0, 0, 1));
        
        // TODO: Apply biological effectors
        
        continue;
    }
    
    return;
}

