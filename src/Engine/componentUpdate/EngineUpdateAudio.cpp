#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateAudio(unsigned int index) {
    
    glm::vec3 position = mStreamBuffer[index].transform->position;
    
    mStreamBuffer[index].sound->SetPosition( position );
    
    return;
}

