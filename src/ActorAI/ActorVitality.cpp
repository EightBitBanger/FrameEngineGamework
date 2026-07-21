#include <GameEngineFramework/Engine/Engine.h>
extern EngineSystemManager Engine;
#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>


bool ActorSystem::HandleVitality(Actor* actor) {
    // Natural expiration 
    //if (actor->physical.mAge > actor->physical.mAgeSenior) 
    //    actor->biological.health -= 1.0f;
    
    // Check if the target is.. no longer with us
    if (actor->biological.health > 0.0f) 
        return false;
    
    for (unsigned int i=0; i < actor->genetics.mGeneticRenderers.size(); i++) {
        MeshRenderer* actorRenderer = actor->genetics.mGeneticRenderers[i];
        actorRenderer->isActive = true;
        mDeadActorRenderers.push_back(actorRenderer);
    }
    actor->genetics.mGeneticRenderers.clear();
    
    // Apply death to the actor
    actor->navigation.mTargetActor = nullptr;
    
    if (actor->inventory.holdingRenderer != nullptr) 
        actor->inventory.holdingRenderer->isActive = false;
    
    actor->isActive = false;
    actor->isGarbage = true;
    return true;
}
