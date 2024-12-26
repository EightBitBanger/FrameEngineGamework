#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateActorBreeding(unsigned int index) {
    
    if (!mStreamBuffer[index].actor->mIsActive) 
        return;
    
    if (mStreamBuffer[index].actor->mBreedWithActor == nullptr) 
        return;
    
    return;
}

