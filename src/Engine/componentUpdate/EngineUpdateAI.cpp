#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateActor(unsigned int index) {
    
    UpdateActorGenetics(index);
    ExpressActorGenetics(index);
    UpdateActorPhysics(index);
    
    return;
}

