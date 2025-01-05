#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Types/types.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

// Perform frustum culling
bool RenderSystem::CullingPass(MeshRenderer* currentEntity, Camera* currentCamera, glm::mat4& viewProjection, Frustum& frustum) {
    
    // Calculate frustum position
    glm::vec3 mBoundingAreaMax = currentEntity->transform.position + currentEntity->mBoundingBoxMax;
    glm::vec3 mBoundingAreaMin = currentEntity->transform.position - currentEntity->mBoundingBoxMin;
    
    // Perform frustum culling
    return FrustumCheckAABB(frustum, mBoundingAreaMin, mBoundingAreaMax);
}
