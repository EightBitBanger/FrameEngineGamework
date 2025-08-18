#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateMeshRenderer(unsigned int index) {
    if (mStreamBuffer[index].components[EngineComponents::MeshRenderer] == nullptr) 
        return;
    
    ((MeshRenderer*)mStreamBuffer[index].components[EngineComponents::MeshRenderer])->transform.position = 
    ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->position;
    
    ((MeshRenderer*)mStreamBuffer[index].components[EngineComponents::MeshRenderer])->transform.rotation = 
    ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->rotation;
    
    ((MeshRenderer*)mStreamBuffer[index].components[EngineComponents::MeshRenderer])->transform.scale = 
    ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->scale;
    
    ((MeshRenderer*)mStreamBuffer[index].components[EngineComponents::MeshRenderer])->transform.matrix = 
    ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->matrix;
}
