#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>


Mesh* RenderSystem::LevelOfDetailPass(MeshRenderer* currentEntity, glm::vec3& eye) {
    
    if (currentEntity->distance == 0.0f) 
        return currentEntity->mesh;
    
    // Return low quality mesh
    if (glm::distance(currentEntity->transform.position, eye) > currentEntity->distance) {
        
        if (currentEntity->meshLod != nullptr) 
            return currentEntity->meshLod;
        
    }
    
    return currentEntity->mesh;
}

