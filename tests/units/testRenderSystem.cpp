#include <iostream>
#include <string>

#include "../framework.h"
#include "../../source/Renderer/RenderSystem.h"
extern RenderSystem Renderer;


void TestFramework::TestRenderSystem(void) {
    if (hasTestFailed) return;
    
    std::cout << "Render system........... ";
    
    Entity* entityPtr = Renderer.CreateEntity();
    Mesh* meshPtr = Renderer.CreateMesh();
    Material* materialPtr = Renderer.CreateMaterial();
    
    // Check render objects where created
    if (entityPtr == nullptr)   mLogString += msgFailedObjectCreate;
    if (meshPtr == nullptr)     mLogString += msgFailedObjectCreate;
    if (materialPtr == nullptr) mLogString += msgFailedObjectCreate;
    
    entityPtr->AttachMesh(meshPtr);
    entityPtr->AttachMaterial(materialPtr);
    
    Mesh*     checkMeshPtr     = entityPtr->GetAttachedMesh();
    Material* checkMaterialPtr = entityPtr->GetAttachedMaterial();
    
    // Check object attachment
    if (checkMeshPtr == nullptr)     mLogString += msgFailedToAttachComponent;
    if (checkMaterialPtr == nullptr) mLogString += msgFailedToAttachComponent;
    
    // Check render objects where destroyed
    if (!Renderer.DestroyMaterial(materialPtr)) mLogString += msgFailedObjectDestroy;
    if (!Renderer.DestroyMesh(meshPtr))         mLogString += msgFailedObjectDestroy;
    if (!Renderer.DestroyEntity(entityPtr))     mLogString += msgFailedObjectDestroy;
    
    // Check entity was not left over
    if (Renderer.GetEntityCount() > 0) mLogString += msgFailedAllocatorNotZero;
    
    // Check cameras
    Camera* cameraPtr = Renderer.CreateCamera();
    if (cameraPtr == nullptr) mLogString += msgFailedObjectCreate;
    if (!Renderer.DestroyCamera(cameraPtr)) mLogString += msgFailedObjectDestroy;
    
    // Check lights
    Light* lightPtr = Renderer.CreateLight();
    if (lightPtr == nullptr) mLogString += msgFailedObjectCreate;
    if (!Renderer.DestroyLight(lightPtr)) mLogString += msgFailedObjectDestroy;
    
    // Check scene
    Scene* scenePtr = Renderer.CreateScene();
    if (scenePtr == nullptr) mLogString += msgFailedObjectCreate;
    if (!Renderer.DestroyScene(scenePtr)) mLogString += msgFailedObjectDestroy;
    
    // Check shader
    Shader* shaderPtr = Renderer.CreateShader();
    if (shaderPtr == nullptr) mLogString += msgFailedObjectCreate;
    if (!Renderer.DestroyShader(shaderPtr)) mLogString += msgFailedObjectDestroy;
    
    return;
}

