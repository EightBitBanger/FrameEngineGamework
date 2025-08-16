#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateLight(unsigned int index) {
    if (mStreamBuffer[index].components[EngineComponents::Light] == nullptr) 
        return;
    
    ((Light*)mStreamBuffer[index].components[EngineComponents::Light])->position  = ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->position;
    ((Light*)mStreamBuffer[index].components[EngineComponents::Light])->direction = ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->EulerAngles();
    
    return;
}

