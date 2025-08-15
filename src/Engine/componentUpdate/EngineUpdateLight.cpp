#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateLight(unsigned int index) {
    
    ((Light*)mStreamBuffer[index].components[EngineComponents::Light])->position  = ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->position;
    ((Light*)mStreamBuffer[index].components[EngineComponents::Light])->direction = ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->EulerAngles();
    
    return;
}

