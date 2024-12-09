#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateMeshRenderer(unsigned int index) {
    
    mStreamBuffer[index].meshRenderer->transform.position  = mStreamBuffer[index].transform->position;
    mStreamBuffer[index].meshRenderer->transform.rotation  = mStreamBuffer[index].transform->rotation;
    mStreamBuffer[index].meshRenderer->transform.scale     = mStreamBuffer[index].transform->scale;
    
    mStreamBuffer[index].meshRenderer->transform.matrix = mStreamBuffer[index].transform->matrix;
    
    return;
}

