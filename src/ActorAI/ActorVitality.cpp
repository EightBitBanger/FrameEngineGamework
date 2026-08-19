#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

bool ActorSystem::HandleVitality(Actor* actor) {
    
    // Natural expiration 
    if (actor->physical.mAge > (actor->physical.mAgeSenior + actor->physical.mAgeAdult)) {
        // TODO weaken/slow down as well?
        actor->biological.health -= 1.0f;
    }
    
    actor->biological.mHungerCounter++;
    if (actor->biological.mHungerCounter > 1024) {
        actor->biological.mHungerCounter = 0;
        
        // Apply hunger
        if (actor->biological.saturation < 0.2f) {
            
            if (actor->biological.hunger < 1.0f) {
                actor->biological.hunger += 0.014f;
                
                if (actor->biological.hunger > 1.0f) 
                    actor->biological.hunger = 1.0f;
            }
        }
        
        // Degrade saturation
        actor->biological.saturation *= 0.9f;
        if (actor->biological.saturation < 0.0f) 
            actor->biological.saturation = 0.0f;
        
    }
    
    // TODO remove size check when reproduction VS death is balanced
    //
    //if (actor->biological.hunger > 0.5f) 
    //    return false;
    //
    // Prevent actors from going extinct by starvation
    if (mActors.Size() < 50) 
        actor->biological.hunger = 0.0f;
    
    // Check if the target is.. no longer with us
    if (actor->biological.health <= 0.0f) {
        
        // Finish off the genetic renderers
        for (unsigned int i = 0; i < actor->genetics.mGeneticRenderers.size(); i++) {
            MeshRenderer* actorRenderer = actor->genetics.mGeneticRenderers[i];
            actorRenderer->isActive = true;
            mDeadActorRenderers.push_back(actorRenderer);
        }
        actor->genetics.mGeneticRenderers.clear();
        
        // Drop items from the inventory
        if (mWorldPlaceCallback != nullptr) {
            float dropRadius = 2.0f;
            for (const std::string& itemClass : actor->inventory.itemClassList) {
                // Calculate a random offset position around the actor
                glm::vec3 dropPos = actor->navigation.mPosition;
                dropPos.x += Random.Range(-dropRadius, dropRadius);
                dropPos.y += 0.5f; // TESTING
                dropPos.z += Random.Range(-dropRadius, dropRadius);
                
                glm::vec3 randomRotation(0.0f, Random.Range(0.0f, 360.0f), 0.0f);
                
                // Place the item in the world
                mWorldPlaceCallback(itemClass, dropPos, randomRotation);
            }
        }
        
        // Apply death to the actor
        actor->navigation.mTargetActor = nullptr;
        
        if (actor->inventory.holdingRenderer != nullptr) 
            actor->inventory.holdingRenderer->isActive = false;
        
        actor->isActive = false;
        actor->isGarbage = true;
        return true;
    }
    return false;
}
