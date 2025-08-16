#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateAudio(unsigned int index) {
    if (mStreamBuffer[index].components[EngineComponents::Sound] == nullptr) 
        return;
    
    glm::vec3 position = ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->position;
    
    ((Sound*)mStreamBuffer[index].components[EngineComponents::Sound])->SetPosition( position );
}

