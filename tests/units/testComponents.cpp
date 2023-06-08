#include <iostream>
#include <string>

#include "../framework.h"
#include "../../source/Engine/Engine.h"

extern EngineSystemManager  Engine;

void TestFramework::TestComponentObject(void) {
    if (hasTestFailed) return;
    
    std::cout << "Component objects....... ";
    
    // Create components
    Component* componentEntity    = Engine.CreateComponent(ComponentType::Renderer);
    Component* componentRigidBody = Engine.CreateComponent(ComponentType::RigidBody);
    Component* componentCamera    = Engine.CreateComponent(ComponentType::Camera);
    Component* componentScript    = Engine.CreateComponent(ComponentType::Script);
    Component* componentLight     = Engine.CreateComponent(ComponentType::Light);
    
    void* entityObject    = componentEntity->GetComponent();
    void* rigidBodyObject = componentRigidBody->GetComponent();
    void* cameraObject    = componentCamera->GetComponent();
    void* scriptObject    = componentScript->GetComponent();
    void* lightObject     = componentLight->GetComponent();
    
    // Check components
    if (entityObject    == nullptr) Throw(msgFailedNullptr, __FILE__, __LINE__);
    if (rigidBodyObject == nullptr) Throw(msgFailedNullptr, __FILE__, __LINE__);
    if (cameraObject    == nullptr) Throw(msgFailedNullptr, __FILE__, __LINE__);
    if (scriptObject    == nullptr) Throw(msgFailedNullptr, __FILE__, __LINE__);
    if (lightObject     == nullptr) Throw(msgFailedNullptr, __FILE__, __LINE__);
    
    // Destroy components
    Engine.DestroyComponent(componentEntity);
    Engine.DestroyComponent(componentRigidBody);
    Engine.DestroyComponent(componentCamera);
    Engine.DestroyComponent(componentScript);
    Engine.DestroyComponent(componentLight);
    
    if (Engine.GetComponentCount() > 0) Throw(msgFailedAllocatorNotZero, __FILE__, __LINE__);
    
    return;
}

