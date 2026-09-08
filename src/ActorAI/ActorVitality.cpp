#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>

extern UniversalConstants UniversalConst;
extern EngineSystemManager Engine;

bool ActorSystem::HandleVitality(Actor* actor) {
    // Natural expiration post senior age
    if (actor->physical.mAge > (actor->physical.mAgeSenior + actor->physical.mAgeAdult)) {
        actor->biological.health -= Random.Range(0.1f, 0.8f);
    }
    
    // Food Consumption Logic
    const float hungerThreshold      = 0.2f;
    const float saturationThreshold  = 0.8f;
    
    bool needsFood = (actor->biological.hunger > hungerThreshold || 
                      actor->biological.health < actor->biological.healthMax);
    if (needsFood && !actor->inventory.itemClassList.empty()) {
        bool isFarmer = false;
        const std::string& trade = actor->memories.Get("trade");
        if (!trade.empty() && trade.rfind("farmer:", 0) == 0) {
            isFarmer = true;
        }
        
        int cropCount = 0;
        if (isFarmer) {
            for (const std::string& itemStr : actor->inventory.itemClassList) {
                if (itemStr.find("crop") != std::string::npos) {
                    cropCount++;
                }
            }
        }
        
        for (unsigned int i = 0; i < actor->inventory.itemClassList.size(); ++i) {
            const std::string& item = actor->inventory.itemClassList[i];
            
            if (item.find("saturation") == std::string::npos) 
                continue;
            
            // Farmers reserve at least 2 crops as seed reserves unless starving
            if (isFarmer && item.find("crop") != std::string::npos && cropCount <= 2 && actor->biological.hunger < 0.85f) {
                continue;
            }
            
            float saturationValue = 0.0f;
            bool hasSaturation = false;
            
            // Explode key-value pairs separated by semicolons
            std::vector<std::string> itemTokens = String.Explode(item, ';');
            for (unsigned int j = 0; j < itemTokens.size(); ++j) {
                String.RemoveWhiteSpace(itemTokens[j]);
                
                std::vector<std::string> kvPair = String.Explode(itemTokens[j], ':');
                if (kvPair.size() >= 2) {
                    String.RemoveWhiteSpace(kvPair[0]);
                    if (kvPair[0] == "saturation") {
                        String.RemoveWhiteSpace(kvPair[1]);
                        saturationValue = String.ToFloat(kvPair[1]);
                        hasSaturation = true;
                        break;
                    }
                }
            }
            
            if (hasSaturation && saturationValue > 0.0f) {
                actor->inventory.RemoveItem(i);
                
                // Replenish saturation and lower hunger dynamically
                actor->biological.saturation += saturationValue;
                actor->biological.hunger     -= saturationValue;
                
                // Heal health and boost comfort if damaged
                const float health  = 2.0f;
                const float comfort = 0.3f;
                
                if (actor->biological.health < actor->biological.healthMax) {
                    actor->biological.health += health * 10.0f;
                    actor->emotions.AddComfort(comfort);
                }
                
                break;
            }
        }
    }
    
    actor->biological.mHungerCounter++;
    if (actor->biological.mHungerCounter > 30) {
        actor->biological.mHungerCounter = 0;
        
        // Apply hunger
        if (actor->biological.saturation < 0.2f) {
            if (actor->biological.hunger < 1.0f) 
                actor->biological.hunger += 0.01f;
        }
        
        // Degrade saturation
        actor->biological.saturation -= 0.00127f;
        if (actor->biological.saturation < 0.0f) 
            actor->biological.saturation = 0.0f;
    }
    
    actor->biological.saturation = glm::clamp(actor->biological.saturation, 0.0f, 1.0f);
    actor->biological.hunger     = glm::clamp(actor->biological.hunger,     0.0f, 1.0f);
    actor->biological.health     = glm::clamp(actor->biological.health,     0.0f, actor->biological.healthMax);
    
    // Check if the target has died
    if (actor->biological.health <= 0.0f) {
        std::string cause = "Trauma";
        if (actor->physical.mAge > (actor->physical.mAgeSenior + actor->physical.mAgeAdult)) {
            cause = "Age";
        } else if (actor->biological.hunger >= 1.0f) {
            cause = "Starvation";
        }
        
        RecordDeath(actor, cause);
        
        // Finish off the genetic renderers
        for (unsigned int i = 0; i < actor->genetics.mGeneticRenderers.size(); i++) {
            MeshRenderer* actorRenderer = actor->genetics.mGeneticRenderers[i];
            actorRenderer->isActive = true;
            mDeadActorRenderers.push_back(actorRenderer);
        }
        actor->genetics.mGeneticRenderers.clear();
        
        // Drop items from the inventory
        if (mWorldPickupPlaceCallback != nullptr) {
            float dropRadius = 1.5f;
            for (const std::string& itemClass : actor->inventory.itemClassList) {
                glm::vec3 dropPos = actor->navigation.mPosition;
                dropPos.x += Random.Range(-dropRadius, dropRadius);
                dropPos.y += 0.2f;
                dropPos.z += Random.Range(-dropRadius, dropRadius);
                
                glm::vec3 randomRotation(0.0f, 90.0f, 0.0f);
                mWorldPickupPlaceCallback(itemClass, dropPos, randomRotation);
            }
        }
        
        const std::string& family = actor->memories.Get("name");
        
        actor->navigation.mTargetActor = nullptr;
        
        if (actor->inventory.holdingRenderer != nullptr) 
            actor->inventory.holdingRenderer->isActive = false;
        
        actor->isActive = false;
        actor->isGarbage = true;
        
        Engine.console.Print(family + " has died by " + cause);
        return true;
    }
    
    return false;
}
