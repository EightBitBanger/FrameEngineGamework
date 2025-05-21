#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>
#include <algorithm>


Mesh* RenderSystem::LevelOfDetailPass(MeshRenderer* currentEntity, glm::vec3& eye) {
    if (currentEntity->mLods.size() == 0)
        return currentEntity->mesh;
    
    float distanceToEye = glm::distance(currentEntity->transform.position, eye);
    
    for (unsigned int i = 0; i < currentEntity->mLods.size(); i++) {
        LevelOfDetail& lod = currentEntity->mLods[i];
        
        if (distanceToEye > lod.distance) 
            continue;
        
        //currentEntity->transform.position += lod.offset;
        
        //currentEntity->transform.matrix = glm::translate(currentEntity->transform.matrix, lod.offset);
        
        return lod.mesh;
    }
    
    // If we’re further than all distances, use the last LOD
    
    //currentEntity->transform.position += currentEntity->mLods.back().offset;
    
    //currentEntity->transform.matrix = glm::translate(currentEntity->transform.matrix, currentEntity->mLods.back().offset);
    
    return currentEntity->mLods.back().mesh;
}
