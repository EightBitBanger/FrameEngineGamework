#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Types/types.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>

struct Frustum {
    
    // Left, Right, Bottom, Top, Near, Far
    glm::vec4 planes[6];
    
};

// Extract frustum planes from view-projection matrix
Frustum ExtractFrustumPlanes(const glm::mat4& viewProjMatrix) {
    
    Frustum frustum;
    
    glm::mat4 inverseViewProjMatrix = glm::inverse( viewProjMatrix );
    
    // Left
    frustum.planes[0] = glm::vec4(
        inverseViewProjMatrix[0][3] + inverseViewProjMatrix[0][0],
        inverseViewProjMatrix[1][3] + inverseViewProjMatrix[1][0],
        inverseViewProjMatrix[2][3] + inverseViewProjMatrix[2][0],
        inverseViewProjMatrix[3][3] + inverseViewProjMatrix[3][0]
    );
    
    // Right
    frustum.planes[1] = glm::vec4(
        inverseViewProjMatrix[0][3] - inverseViewProjMatrix[0][0],
        inverseViewProjMatrix[1][3] - inverseViewProjMatrix[1][0],
        inverseViewProjMatrix[2][3] - inverseViewProjMatrix[2][0],
        inverseViewProjMatrix[3][3] - inverseViewProjMatrix[3][0]
    );
    
    // Bottom
    frustum.planes[2] = glm::vec4(
        inverseViewProjMatrix[0][3] + inverseViewProjMatrix[0][1],
        inverseViewProjMatrix[1][3] + inverseViewProjMatrix[1][1],
        inverseViewProjMatrix[2][3] + inverseViewProjMatrix[2][1],
        inverseViewProjMatrix[3][3] + inverseViewProjMatrix[3][1]
    );
    
    // Top
    frustum.planes[3] = glm::vec4(
        inverseViewProjMatrix[0][3] - inverseViewProjMatrix[0][1],
        inverseViewProjMatrix[1][3] - inverseViewProjMatrix[1][1],
        inverseViewProjMatrix[2][3] - inverseViewProjMatrix[2][1],
        inverseViewProjMatrix[3][3] - inverseViewProjMatrix[3][1]
    );
    
    // Near
    frustum.planes[4] = glm::vec4(
        inverseViewProjMatrix[0][3] + inverseViewProjMatrix[0][2],
        inverseViewProjMatrix[1][3] + inverseViewProjMatrix[1][2],
        inverseViewProjMatrix[2][3] + inverseViewProjMatrix[2][2],
        inverseViewProjMatrix[3][3] + inverseViewProjMatrix[3][2]
    );
    
    // Far
    frustum.planes[5] = glm::vec4(
        inverseViewProjMatrix[0][3] - inverseViewProjMatrix[0][2],
        inverseViewProjMatrix[1][3] - inverseViewProjMatrix[1][2],
        inverseViewProjMatrix[2][3] - inverseViewProjMatrix[2][2],
        inverseViewProjMatrix[3][3] - inverseViewProjMatrix[3][2]
    );
    
    // Normalize the planes
    for (int i = 0; i < 6; i++) {
        float length = glm::length(glm::vec3(frustum.planes[i]));
        frustum.planes[i] /= length;
    }
    
    return frustum;
}

// Check if a point is inside the frustum
bool IsPointInFrustum(const Frustum& frustum, const glm::vec3& point) {
    
    for (int i = 0; i < 6; i++) {
        
        if (glm::dot(glm::vec3(frustum.planes[i]), point) + frustum.planes[i].w < 0) 
            return false;
        
    }
    
    return true;
}

// Check if a bounding box is inside the frustum
bool IsAABBInFrustum(const Frustum& frustum, const glm::vec3& min, const glm::vec3& max) {
    for (int i = 0; i < 6; i++) {
        
        glm::vec3 positiveVertex = min;
        
        if (frustum.planes[i].x >= 0) positiveVertex.x = max.x;
        if (frustum.planes[i].y >= 0) positiveVertex.y = max.y;
        if (frustum.planes[i].z >= 0) positiveVertex.z = max.z;
        
        if (glm::dot(glm::vec3(frustum.planes[i]), positiveVertex) + frustum.planes[i].w < 0) 
            return false;
        
    }
    return true;
}

// Perform frustum culling
bool RenderSystem:: CullingPass(MeshRenderer* currentEntity, Camera* currentCamera, glm::mat4& viewProjection) {
    
    // Extract frustum planes
    Frustum frustum = ExtractFrustumPlanes(viewProjection);
    
    // Calculate frustum position
    glm::vec3 mBoundingAreaMax = currentEntity->transform.position + currentEntity->mBoundingBoxMax;
    glm::vec3 mBoundingAreaMin = currentEntity->transform.position - currentEntity->mBoundingBoxMin;
    
    // Perform frustum culling
    return IsAABBInFrustum(frustum, mBoundingAreaMin, mBoundingAreaMax);
}
