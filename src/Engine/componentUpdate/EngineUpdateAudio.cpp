#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateAudio(unsigned int index) {
    if (mStreamBuffer[index].components[EngineComponent::Sound] == nullptr) 
        return;
    
    glm::vec3 position = ((Transform*)mStreamBuffer[index].components[EngineComponent::Transform])->position;
    
    ((Sound*)mStreamBuffer[index].components[EngineComponent::Sound])->SetPosition( position );
}

