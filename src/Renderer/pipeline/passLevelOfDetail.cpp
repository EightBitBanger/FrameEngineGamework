#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


Mesh* RenderSystem::LevelOfDetailPass(MeshRenderer* currentEntity, glm::vec3& eye) {
    
    if (currentEntity->distance == 0.0f) 
        return currentEntity->mesh;
    
    std::vector<Mesh*>& levelOfDetailList = currentEntity->lods;
    
    if (levelOfDetailList.size() == 0) 
        return currentEntity->mesh;
    
    // Return lower quality mesh
    if (glm::distance(currentEntity->transform.position, eye) > currentEntity->distance) 
        return levelOfDetailList[0];
    
    // Return full quality mesh
    return currentEntity->mesh;
}

