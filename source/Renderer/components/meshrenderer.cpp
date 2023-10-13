#include "meshrenderer.h"


MeshRenderer::MeshRenderer() : 
    
    mMesh(nullptr),
    mMaterial(nullptr)
{
}

void MeshRenderer::SetMesh(Mesh* meshPtr) {
    mMesh = meshPtr;
    return;
}

Mesh* MeshRenderer::GetMesh(void) {
    return mMesh;
}


void MeshRenderer::SetMaterial(Material* materialPtr) {
    mMaterial = materialPtr;
    return;
}

Material* MeshRenderer::GetMaterial(void) {
    return mMaterial;
}
