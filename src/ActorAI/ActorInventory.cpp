#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Math/Random.h>

bool ActorSystem::UpdateActorInventory(Actor* actor) {
    
    // Only deactivate the held item if the actor is NOT attacking
    if (actor->state.mode != ActorState::Mode::MoveAttack && 
        actor->emotions.current.comfort > 0.5f && 
        actor->inventory.holdingRenderer != nullptr) {
        actor->inventory.holdingRenderer->isActive = false;
    }
    
    // Check in hand item rebuild
    if (actor->inventory.doBuildHandMesh) {
        actor->inventory.doBuildHandMesh = false;
        actor->inventory.BuildHandMesh(actor->inventory.inHandItemClass);
    }
    
    return true;
}
