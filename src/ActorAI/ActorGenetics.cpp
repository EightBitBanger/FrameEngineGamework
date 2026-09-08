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
    for (unsigned int a = 0; a < numberOfGenes; a++) {
        // Check should express
        if (!actor->genetics.mGenes[a].doExpress)
            continue;
        
        // Attachment offset
        if (actor->genetics.mGenes[a].attachmentIndex != 0) {
            unsigned int attachmentIndex = actor->genetics.mGenes[a].attachmentIndex - 1;
            actor->genetics.mGenes[a].offset = actor->genetics.mGenes[attachmentIndex].offset;
        }
        
        MeshRenderer* renderer = CreateMeshRendererForGene(actor, a, baseMesh);
        
        glm::vec4 orientation = glm::vec4(Transform().rotation.w, Transform().rotation.x, Transform().rotation.y, Transform().rotation.z);
        actor->genetics.mGeneticRenderers.push_back(renderer);
        actor->animation.mAnimation.push_back(orientation);
        
        sceneMain->AddMeshRendererToSceneRoot(renderer, RENDER_QUEUE_GEOMETRY);
    }
    
    // Only push dummy entries if not already populated
    Phen dummyPhenotype;
    while (actor->genetics.mPhen.size() < numberOfGenes) {
        actor->genetics.mPhen.push_back(dummyPhenotype);
    }
    
    Bio dummyBio;
    while (actor->biological.mBiologics.size() < numberOfGenes) {
        actor->biological.mBiologics.push_back(dummyBio);
    }
}

MeshRenderer* ActorSystem::CreateMeshRendererForGene(Actor* actor, unsigned int geneIndex, Mesh* sourceMesh) {
    MeshRenderer* renderer = Renderer.CreateMeshRenderer();
    renderer->isActive = false;
    
    renderer->EnableFrustumCulling();
    renderer->SetBoundingBox({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
    
    renderer->mesh = sourceMesh;
    renderer->material = Renderer.CreateMaterial();
    renderer->material->isShared = false;
    
    renderer->material->ambient = Colors.white;
    renderer->material->shader = shader;
    
    renderer->material->DisableBlending();
    renderer->material->EnableCulling();
    renderer->material->EnableDepthTest();
    renderer->material->DisableShadowVolumePass();
    
    return renderer;
}

void ActorSystem::ClearOldGeneticRenderers(Actor* actor) {
    for (unsigned int a = 0; a < actor->genetics.mGeneticRenderers.size(); a++) {
        MeshRenderer* geneRenderer = actor->genetics.mGeneticRenderers[a];
        sceneMain->RemoveMeshRendererFromSceneRoot(geneRenderer, RENDER_QUEUE_GEOMETRY);
        Renderer.DestroyMeshRenderer(geneRenderer);
    }
    actor->genetics.mGeneticRenderers.clear();
    actor->animation.mAnimation.clear();
}

// Express an actors genetics by calculating the phenotypic expression from its genome as well as its environment and state
void ActorSystem::ExpressActorGenetics(Actor* actor) {
    if (!actor->genetics.mDoReexpressGenetics)
        return;
    
    actor->genetics.mDoReexpressGenetics = false;
    
    unsigned int numberOfGenes = actor->genetics.mGenes.size();
    unsigned int numberOfRenderers = actor->genetics.mGeneticRenderers.size();
    
    // Trigger a full rebuild if any genes where added to or removed from the genome
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
        meshRenderer->transform.position = actor->navigation.mPosition;
        
        // Color inheritance
        Color targetColor;
        if (actor->genetics.mGenes[a].colorIndex == 0) {
            targetColor = actor->genetics.mGenes[a].color.ToVec3();
        } else {
            // Inherit color from another gene
            unsigned int colorIndex = actor->genetics.mGenes[a].colorIndex - 1;
            targetColor = actor->genetics.mGenes[colorIndex].color.ToVec3();
        }
        
        // Phenotypic color modifier
        if (a < actor->genetics.mPhen.size()) {
            targetColor += actor->genetics.mPhen[a].color.ToVec3();
        }
        
        // Biological tint effector
        if (a < actor->biological.mBiologics.size()) {
            targetColor += actor->biological.mBiologics[a].tint.ToVec3();
        }
        
        meshRenderer->material->diffuse = Colors.Clamp(targetColor, 0.0f, 1.0f);
        
        // Scale inheritance
        glm::vec3 targetScale;
        if (actor->genetics.mGenes[a].scaleIndex == 0) {
            targetScale = actor->genetics.mGenes[a].scale.ToVec3();
        } else {
            // Inherit scale from another gene
            unsigned int scaleIndex = actor->genetics.mGenes[a].scaleIndex - 1;
            targetScale = actor->genetics.mGenes[scaleIndex].scale.ToVec3();
        }
        meshRenderer->transform.scale = glm::clamp(targetScale, 0.0f, 2.0f);
        
        // Check custom expression
        if (actor->genetics.mGenes[a].form != ActorState::Genetic::Base) {
            
            bool sexualOrientation = actor->physical.GetSexualOrientation();
            
            // Check to disable genes that do not apply to this actor
            if ((actor->genetics.mGenes[a].form == ActorState::Genetic::Male && sexualOrientation == false) || 
                (actor->genetics.mGenes[a].form == ActorState::Genetic::Female && sexualOrientation == true)) {
                
                meshRenderer->isActive = false;
            }
            
            // Resolve effective gene index for expression thresholds/factors when scaleIndex > 0
            unsigned int expGeneIndex = a;
            if (actor->genetics.mGenes[a].scaleIndex > 0) {
                unsigned int indexed = actor->genetics.mGenes[a].scaleIndex - 1;
                if (indexed < numberOfGenes) {
                    expGeneIndex = indexed;
                }
            }
            
            // Process age dependent genes
            if (actor->physical.mAge >= actor->genetics.mGenes[expGeneIndex].expressionAge) {
                
                // Express only genes that apply to this actor
                if ((actor->genetics.mGenes[a].form == ActorState::Genetic::Male && sexualOrientation == true) || 
                    (actor->genetics.mGenes[a].form == ActorState::Genetic::Female && sexualOrientation == false)) {
                    
                    float expressionFactor = actor->genetics.mGenes[expGeneIndex].expressionFactor;
                    
                    // Modify phenotypic expression
                    actor->genetics.mPhen[a].scale.x = expressionFactor;
                    actor->genetics.mPhen[a].scale.y = expressionFactor;
                    actor->genetics.mPhen[a].scale.z = expressionFactor;
                }
            }
            
            float maxUniformScale = actor->genetics.mGenes[expGeneIndex].expressionMax;
            
            // Apply phenotypic expression
            meshRenderer->transform.scale *= glm::clamp(glm::vec3(actor->genetics.mPhen[a].scale.x, 
                                                                  actor->genetics.mPhen[a].scale.y, 
                                                                  actor->genetics.mPhen[a].scale.z), 
                                                                  0.0f, maxUniformScale);
            
        }
        
        // Biological inflammation effector (uniform scale offset)
        if (a < actor->biological.mBiologics.size()) {
            meshRenderer->transform.scale += glm::vec3(actor->biological.mBiologics[a].inflammation);
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
        
        continue;
    }
}

void ActorSystem::CombineParentMemories(Actor* parentA, Actor* parentB, Actor* child) {
    if (parentA == nullptr || parentB == nullptr || child == nullptr) return;
    child->memories.Clear();
    
    // Determine parent roles (mSexualOrientation: true = male, false = female)
    Actor* father = parentA->physical.mSexualOrientation ? parentA : parentB;
    Actor* mother = parentA->physical.mSexualOrientation ? parentB : parentA;
    
    // Pick father if child is male, or mother if female
    const Actor* chosenParent = child->physical.mSexualOrientation ? father : mother;
    
    // Inherit specific memories from the chosen parent
    static const std::string targetKeys[] = { "behavior", "sentience", "trade", "culture", "religion" };
    for (const std::string& key : targetKeys) {
        std::unordered_map<std::string, std::string>::const_iterator it = chosenParent->memories.mMemories.find(key);
        if (it != chosenParent->memories.mMemories.end() && !it->second.empty()) {
            child->memories.Add(key, it->second);
        }
    }
    
    // Always inherit family name from the father
    std::string familyName = father->memories.Get("family");
    if (!familyName.empty()) {
        child->memories.Add("family", familyName);
    }
    
    child->memories.mDoUpdateMemories = true;
}
