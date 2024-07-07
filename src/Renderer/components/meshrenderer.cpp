#include <GameEngineFramework/Renderer/components/meshrenderer.h>


MeshRenderer::MeshRenderer() : 
    isActive(true),
    mesh(nullptr),
    material(nullptr),
    mDoCulling(false)
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
