#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateMeshRenderer(unsigned int index) {
    if (mStreamBuffer[index].components[EngineComponent::MeshRenderer] == nullptr) 
        return;
    
    ((MeshRenderer*)mStreamBuffer[index].components[EngineComponent::MeshRenderer])->transform.position = 
    ((Transform*)mStreamBuffer[index].components[EngineComponent::Transform])->position;
    
    ((MeshRenderer*)mStreamBuffer[index].components[EngineComponent::MeshRenderer])->transform.rotation = 
    ((Transform*)mStreamBuffer[index].components[EngineComponent::Transform])->rotation;
    
    ((MeshRenderer*)mStreamBuffer[index].components[EngineComponent::MeshRenderer])->transform.scale = 
    ((Transform*)mStreamBuffer[index].components[EngineComponent::Transform])->scale;
    
    ((MeshRenderer*)mStreamBuffer[index].components[EngineComponent::MeshRenderer])->transform.matrix = 
    ((Transform*)mStreamBuffer[index].components[EngineComponent::Transform])->matrix;
}
