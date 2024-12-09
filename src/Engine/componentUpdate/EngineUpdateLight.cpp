#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateLight(unsigned int index) {
    
    mStreamBuffer[index].light->position  = mStreamBuffer[index].transform->position;
    mStreamBuffer[index].light->direction = mStreamBuffer[index].transform->EulerAngles();
    
    return;
}

