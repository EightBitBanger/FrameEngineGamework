#include <GameEngineFramework/Renderer/components/meshrenderer.h>


MeshRenderer::MeshRenderer() : 
    isActive(false),
    mesh(nullptr),
    material(nullptr),
    mDoCulling(false),
    mBoundingBoxMin(glm::vec3(-1.0f, -1.0f, -1.0f)),
    mBoundingBoxMax(glm::vec3(1.0f, 1.0f, 1.0f))
{
}

void MeshRenderer::EnableFrustumCulling(void) {
    mDoCulling = true;
}

void MeshRenderer::DisableFrustumCulling(void) {
    mDoCulling = false;
}

void MeshRenderer::SetBoundingBox(glm::vec3 min, glm::vec3 max) {
    mBoundingBoxMin = min;
    mBoundingBoxMax = max;
}

glm::vec3 MeshRenderer::GetBoundingBoxMin(void) {
    return mBoundingBoxMin;
}

glm::vec3 MeshRenderer::GetBoundingBoxMax(void) {
    return mBoundingBoxMax;
}

void MeshRenderer::AddLevelOfDetail(LevelOfDetail lod) {
    mLods.push_back( lod );
}

bool MeshRenderer::RemoveLevelOfDetail(unsigned int index) {
    if (index >= mLods.size()) 
        return false;
    mLods.erase(mLods.begin() + index);
    return true;
}

unsigned int MeshRenderer::GetNumberOfLevelsOfDetail(void) {
    return mLods.size();
}
