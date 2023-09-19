#include "meshrenderer.h"


MeshRenderer::MeshRenderer() : 
    
    mMesh(nullptr),
    mMaterial(nullptr)
{
}

void MeshRenderer::AttachMesh(Mesh* meshPtr) {
    mMesh = meshPtr;
    return;
}

void MeshRenderer::DetachMesh(void) {
    mMesh = nullptr;
    return;
}

Mesh* MeshRenderer::GetAttachedMesh(void) {
    return mMesh;
}


void MeshRenderer::AttachMaterial(Material* materialPtr) {
    mMaterial = materialPtr;
    return;
}

void MeshRenderer::DetachMaterial(void) {
    mMaterial = nullptr;
    return;
}

Material* MeshRenderer::GetAttachedMaterial(void) {
    return mMaterial;
}
