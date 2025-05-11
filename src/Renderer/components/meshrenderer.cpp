#include <GameEngineFramework/Renderer/components/meshrenderer.h>


MeshRenderer::MeshRenderer() : 
    isActive(false),
    mesh(nullptr),
    material(nullptr),
    distance(0),
    mDoCulling(false),
    mBoundingBoxMin(glm::vec3(-1.0f, -1.0f, -1.0f)),
    mBoundingBoxMax(glm::vec3(1.0f, 1.0f, 1.0f))
{
}

void MeshRenderer::EnableFrustumCulling(void) {
    mDoCulling = true;
    return;
}

void MeshRenderer::DisableFrustumCulling(void) {
    mDoCulling = false;
    return;
}

void MeshRenderer::SetBoundingBoxMin(glm::vec3 min) {
    mBoundingBoxMin = min;
    return;
}

void MeshRenderer::SetBoundingBoxMax(glm::vec3 max) {
    mBoundingBoxMax = max;
    return;
}

glm::vec3 MeshRenderer::GetBoundingBoxMin(void) {
    
    return mBoundingBoxMin;
}

glm::vec3 MeshRenderer::GetBoundingBoxMax(void) {
    
    return mBoundingBoxMax;
}

