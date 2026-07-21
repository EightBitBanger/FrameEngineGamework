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
    
    // Generate and push back blank phenotypes and biologicals for each gene
    Phen dummyPhenotype;
    if (actor->genetics.mPhen.size() == 0) {
        for (unsigned int a = 0; a < numberOfGenes; a++) 
            actor->genetics.mPhen.push_back(dummyPhenotype);
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
    actor->genetics.mPhen.clear();
    actor->animation.mAnimation.clear();
    actor->biological.mBiologics.clear();
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
            // Inherit scale from another gene
            unsigned int colorIndex = actor->genetics.mGenes[a].colorIndex - 1;
            targetColor = actor->genetics.mGenes[colorIndex].color.ToVec3();
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
            
            // Process age dependent genes
            if (actor->physical.mAge >= actor->genetics.mGenes[a].expressionAge) {
                
                // Express only genes that apply to this actor
                if ((actor->genetics.mGenes[a].form == ActorState::Genetic::Male && sexualOrientation == true) || 
                    (actor->genetics.mGenes[a].form == ActorState::Genetic::Female && sexualOrientation == false)) {
                    
                    float expressionFactor = actor->genetics.mGenes[a].expressionFactor;
                    
                    // Modify phenotypic expression
                    actor->genetics.mPhen[a].scale.x = expressionFactor;
                    actor->genetics.mPhen[a].scale.y = expressionFactor;
                    actor->genetics.mPhen[a].scale.z = expressionFactor;
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
        
        // TODO Apply biological effectors
        
        continue;
    }
}

void ActorSystem::CombineParentMemories(const Actor* parentA, const Actor* parentB, Actor* child) {
    if (parentA == nullptr || parentB == nullptr || child == nullptr) return;
    child->memories.Clear();
    
    // First, discover every unique memory category across both parents
    std::vector<std::string> allCategories;
    
    for (std::unordered_map<std::string, std::string>::const_iterator it = parentA->memories.mMemories.begin();
         it != parentA->memories.mMemories.end(); ++it) {
        allCategories.push_back(it->first);
    }
    
    for (std::unordered_map<std::string, std::string>::const_iterator it = parentB->memories.mMemories.begin();
         it != parentB->memories.mMemories.end(); ++it) {
        
        bool alreadyExists = false;
        for (std::vector<std::string>::const_iterator catIt = allCategories.begin(); catIt != allCategories.end(); ++catIt) {
            if (*catIt == it->first) {
                alreadyExists = true;
                break;
            }
        }
        if (!alreadyExists) {
            allCategories.push_back(it->first);
        }
    }
    
    // Process each category dynamically
    for (std::vector<std::string>::const_iterator catIt = allCategories.begin(); catIt != allCategories.end(); ++catIt) {
        const std::string& category = *catIt;
        
        std::unordered_map<std::string, std::vector<MemoryTrigger>>::const_iterator mapItA = 
            parentA->memories.mMemoryTriggers.find(category);
        std::unordered_map<std::string, std::vector<MemoryTrigger>>::const_iterator mapItB = 
            parentB->memories.mMemoryTriggers.find(category);
        
        bool hasTriggersA = (mapItA != parentA->memories.mMemoryTriggers.end() && !mapItA->second.empty());
        bool hasTriggersB = (mapItB != parentB->memories.mMemoryTriggers.end() && !mapItB->second.empty());
        
        // This category contains parsed numeric MemoryTriggers that can be blended
        if (hasTriggersA || hasTriggersB) {
            std::vector<MemoryTrigger> blendedTriggers;
            
            if (hasTriggersA && hasTriggersB) {
                const std::vector<MemoryTrigger>& triggersA = mapItA->second;
                const std::vector<MemoryTrigger>& triggersB = mapItB->second;
                
                // Match and blend combinations from Parent A
                for (std::vector<MemoryTrigger>::const_iterator aIt = triggersA.begin(); aIt != triggersA.end(); ++aIt) {
                    bool foundMatch = false;
                    for (std::vector<MemoryTrigger>::const_iterator bIt = triggersB.begin(); bIt != triggersB.end(); ++bIt) {
                        if (aIt->name == bIt->name) {
                            MemoryTrigger blended;
                            blended.name = aIt->name;
                            blended.value = (aIt->value + bIt->value) * 0.5f;
                            blendedTriggers.push_back(blended);
                            foundMatch = true;
                            break;
                        }
                    }
                    if (!foundMatch) {
                        blendedTriggers.push_back(*aIt);
                    }
                }
                
                // Append entirely unique sub-traits found only in Parent B
                for (std::vector<MemoryTrigger>::const_iterator bIt = triggersB.begin(); bIt != triggersB.end(); ++bIt) {
                    bool foundMatch = false;
                    for (std::vector<MemoryTrigger>::const_iterator aIt = triggersA.begin(); aIt != triggersA.end(); ++aIt) {
                        if (bIt->name == aIt->name) {
                            foundMatch = true;
                            break;
                        }
                    }
                    if (!foundMatch) {
                        blendedTriggers.push_back(*bIt);
                    }
                }
            } 
            else if (hasTriggersA) {
                blendedTriggers = mapItA->second;
            } 
            else {
                blendedTriggers = mapItB->second;
            }
            
            // Serialize the dynamically generated collection back into the string structure
            if (!blendedTriggers.empty()) {
                std::string serializedString = "";
                for (std::vector<MemoryTrigger>::const_iterator tIt = blendedTriggers.begin(); tIt != blendedTriggers.end(); ++tIt) {
                    if (tIt != blendedTriggers.begin()) {
                        serializedString += ",";
                    }
                    serializedString += tIt->name + ":" + std::to_string(tIt->value);
                }
                child->memories.mMemories[category] = serializedString;
            }
        }
        
        // Plain-text identifier value string
        else {
            std::unordered_map<std::string, std::string>::const_iterator rawItA = parentA->memories.mMemories.find(category);
            std::unordered_map<std::string, std::string>::const_iterator rawItB = parentB->memories.mMemories.find(category);
            
            if (rawItA != parentA->memories.mMemories.end() && rawItB != parentB->memories.mMemories.end()) {
                // If both parents have different raw text values, fallback safely (e.g., inherit from Parent A)
                child->memories.mMemories[category] = rawItA->second;
            }
            else if (rawItA != parentA->memories.mMemories.end()) {
                child->memories.mMemories[category] = rawItA->second;
            }
            else if (rawItB != parentB->memories.mMemories.end()) {
                child->memories.mMemories[category] = rawItB->second;
            }
        }
    }
    
    child->memories.mDoUpdateMemories = true;
}
