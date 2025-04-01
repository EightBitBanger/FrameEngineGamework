#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>
#include <algorithm>

Mesh* RenderSystem::LevelOfDetailPass(MeshRenderer* currentEntity, glm::vec3& eye) {
    
    if (currentEntity->distance == 0.0f) 
        return currentEntity->mesh;
    
    std::vector<Mesh*>& levelOfDetailList = currentEntity->lods;
    
    // Check no lods
    if (levelOfDetailList.size() == 0) 
        return currentEntity->mesh;
    
    float distanceToEye = glm::distance(currentEntity->transform.position, eye);
    
    // Array of distance levels
    float lodDistances[] = { currentEntity->distance, currentEntity->distance * 1.3f, currentEntity->distance * 1.8f };
    
    if (distanceToEye < currentEntity->distance * 0.7f) 
        return currentEntity->mesh;
    
    // Find the appropriate level of detail using binary search
    size_t lodIndex = std::lower_bound(std::begin(lodDistances), std::end(lodDistances), distanceToEye) - std::begin(lodDistances);
    
    if (lodIndex < levelOfDetailList.size()) 
        return levelOfDetailList[lodIndex];
    
    // Return full quality mesh if within the closest level of detail range
    return levelOfDetailList[ levelOfDetailList.size()-1 ];
}

