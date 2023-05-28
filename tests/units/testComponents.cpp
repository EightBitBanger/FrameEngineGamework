#include <iostream>
#include <string>

#include "../unitTest.h"
#include "../../source/Engine/Engine.h"
extern EngineSystemManager  Engine;


void ApplicationTest::TestComponentObject(void) {
    if (hasTestFailed) return;
    
    std::string msgFailedNullptr = "component internal pointer is null\n";
    
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
    if (entityObject    == nullptr) mLogString += msgFailedNullptr;
    if (rigidBodyObject == nullptr) mLogString += msgFailedNullptr;
    if (cameraObject    == nullptr) mLogString += msgFailedNullptr;
    if (scriptObject    == nullptr) mLogString += msgFailedNullptr;
    if (lightObject     == nullptr) mLogString += msgFailedNullptr;
    
    // Destroy components
    Engine.DestroyComponent(componentEntity);
    Engine.DestroyComponent(componentRigidBody);
    Engine.DestroyComponent(componentCamera);
    Engine.DestroyComponent(componentScript);
    Engine.DestroyComponent(componentLight);
    
    // Finalize the test
    if (mLogString != "") {
        std::cout  << msgFailed << std::endl;
        std::cout << mLogString << std::endl;
        mLogString="";
    } else {
        std::cout << msgPassed << std::endl;
    }
    
    return;
}

