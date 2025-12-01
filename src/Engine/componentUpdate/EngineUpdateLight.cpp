#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateLight(unsigned int index) {
    if (mStreamBuffer[index].components[EngineComponent::Light] == nullptr) 
        return;
    
    ((Light*)mStreamBuffer[index].components[EngineComponent::Light])->position  = ((Transform*)mStreamBuffer[index].components[EngineComponent::Transform])->position;
    ((Light*)mStreamBuffer[index].components[EngineComponent::Light])->direction = ((Transform*)mStreamBuffer[index].components[EngineComponent::Transform])->EulerAngles();
    
    return;
}

