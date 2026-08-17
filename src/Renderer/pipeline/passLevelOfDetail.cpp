#include <GameEngineFramework/Renderer/RenderSystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/Types.h>
#include <algorithm>


Mesh* RenderSystem::LevelOfDetailPass(MeshRenderer* currentEntity, glm::vec3& eye) {
    if (currentEntity->mLods.empty())
        return currentEntity->mesh;
    
    float distanceToEye = glm::distance(currentEntity->transform.position, eye);
    
    // Render full detail base mesh if within first LOD distance
    if (distanceToEye <= currentEntity->mLods[0].distance)
        return currentEntity->mesh;
    
    for (size_t i = 0; i < currentEntity->mLods.size(); i++) {
        if (distanceToEye <= currentEntity->mLods[i].distance)
            return currentEntity->mLods[i].mesh;
    }
    
    return currentEntity->mLods.back().mesh;
}
