#include <iostream>
#include <string>

#include "../framework.h"
#include "../../source/Renderer/RenderSystem.h"
extern RenderSystem Renderer;


void TestFramework::TestRenderSystem(void) {
    if (hasTestFailed) return;
    
    std::cout << "Render system........... ";
    
    MeshRenderer* meshRendererPtr = Renderer.CreateMeshRenderer();
    Mesh* meshPtr = Renderer.CreateMesh();
    Material* materialPtr = Renderer.CreateMaterial();
    
    // Check render objects where created
    if (meshRendererPtr == nullptr)   Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (meshPtr == nullptr)     Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (materialPtr == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    
    meshRendererPtr->SetMesh(meshPtr);
    meshRendererPtr->SetMaterial(materialPtr);
    
    Mesh*     checkMeshPtr     = meshRendererPtr->GetMesh();
    Material* checkMaterialPtr = meshRendererPtr->GetMaterial();
    
    // Check object attachment
    if (checkMeshPtr == nullptr)     Throw(msgFailedToAttachComponent, __FILE__, __LINE__);
    if (checkMaterialPtr == nullptr) Throw(msgFailedToAttachComponent, __FILE__, __LINE__);
    
    // Check render objects where destroyed
    if (!Renderer.DestroyMaterial(materialPtr))   Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    if (!Renderer.DestroyMesh(meshPtr))           Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    if (!Renderer.DestroyMeshRenderer(meshRendererPtr)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    
    // Check entity was not left over
    if (Renderer.GetMeshRendererCount() > 0) Throw(msgFailedAllocatorNotZero, __FILE__, __LINE__);
    
    // Check cameras
    Camera* cameraPtr = Renderer.CreateCamera();
    if (cameraPtr == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (!Renderer.DestroyCamera(cameraPtr)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    
    // Check lights
    Light* lightPtr = Renderer.CreateLight();
    if (lightPtr == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (!Renderer.DestroyLight(lightPtr)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    
    // Check scene
    Scene* scenePtr = Renderer.CreateScene();
    if (scenePtr == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (!Renderer.DestroyScene(scenePtr)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    
    // Check shader
    Shader* shaderPtr = Renderer.CreateShader();
    if (shaderPtr == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (!Renderer.DestroyShader(shaderPtr)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    
    return;
}

