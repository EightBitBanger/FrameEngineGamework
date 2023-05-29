#include <iostream>
#include <string>

#include "../unitTest.h"
#include "../../source/Renderer/RenderSystem.h"
extern RenderSystem Renderer;


void ApplicationTest::TestRenderSystem(void) {
    if (hasTestFailed) return;
    
    std::cout << "Render system........... ";
    
    Entity* entityPtr = Renderer.CreateEntity();
    Mesh* meshPtr = Renderer.CreateMesh();
    Material* materialPtr = Renderer.CreateMaterial();
    
    // Check render objects where created
    if (entityPtr == nullptr)   mLogString += msgFailedObjectCreation;
    if (meshPtr == nullptr)     mLogString += msgFailedObjectCreation;
    if (materialPtr == nullptr) mLogString += msgFailedObjectCreation;
    
    entityPtr->AttachMesh(meshPtr);
    entityPtr->AttachMaterial(materialPtr);
    
    Mesh*     checkMeshPtr     = entityPtr->GetAttachedMesh();
    Material* checkMaterialPtr = entityPtr->GetAttachedMaterial();
    
    // Check object attachment
    if (checkMeshPtr == nullptr)     mLogString += msgFailedToAttachComponent;
    if (checkMaterialPtr == nullptr) mLogString += msgFailedToAttachComponent;
    
    // Check render objects where destroyed
    if (!Renderer.DestroyMaterial(materialPtr)) mLogString += msgFailedObjectDestruction;
    if (!Renderer.DestroyMesh(meshPtr))         mLogString += msgFailedObjectDestruction;
    if (!Renderer.DestroyEntity(entityPtr))     mLogString += msgFailedObjectDestruction;
    
    // Check entity was not left over
    if (Renderer.GetEntityCount() > 0) mLogString += msgFailedObjectAllocator;
    
    return;
}

