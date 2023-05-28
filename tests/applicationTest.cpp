#include <iostream>
#include <string>


#include "../source/Engine/Engine.h"
#include "../source/Application/ApplicationLayer.h"
#include "../source/Renderer/RenderSystem.h"
extern EngineSystemManager  Engine;
extern RenderSystem         Renderer;
extern ApplicationLayer     Application;

#include "applicationTest.h"


ApplicationTest::ApplicationTest() : 
    mLogString("")
{}

void ApplicationTest::Initiate(void) {
    std::cout << "Running unit tests" << std::endl;
}

void ApplicationTest::Complete(void) {
    std::cout << std::endl << "Complete" << std::endl << std::endl;
}



void ApplicationTest::TestEngineFunctionality(void) {
    
    std::string msgFailedObjectCreation = "engine-object creation\n";
    std::string msgFailedObjectDestruction = "engine-object destruction\n";
    
    std::cout << "Engine functionality.... ";
    
    // Create game object
    GameObject* gameObject = Engine.CreateGameObject();
    if (gameObject == nullptr) mLogString += msgFailedObjectCreation;
    Engine.DestroyGameObject(gameObject);
    if (Engine.GetGameObjectCount() > 0) mLogString += msgFailedObjectDestruction;
    
    // Create component
    Component* component = Engine.CreateComponent(ComponentType::Renderer);
    if (component == nullptr) mLogString += msgFailedObjectCreation;
    Engine.DestroyComponent(component);
    if (Engine.GetComponentCount() > 0) mLogString += msgFailedObjectDestruction;
    
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



void ApplicationTest::TestGameObject(void) {
    
    std::string msgFailedToCreateObject    = "object creation\n";
    std::string msgFailedToAttachComponent = "component attachment\n";
    std::string msgFailedToDetachComponent = "component detachment\n";
    std::string msgFailedToCreateComponent = "component creation\n";
    
    std::cout << "Game objects............ ";
    
    GameObject* gameObject = Engine.CreateGameObject();
    
    Component* componentEntity    = Engine.CreateComponent(ComponentType::Renderer);
    Component* componentRigidBody = Engine.CreateComponent(ComponentType::RigidBody);
    Component* componentCamera    = Engine.CreateComponent(ComponentType::Camera);
    Component* componentScript    = Engine.CreateComponent(ComponentType::Script);
    Component* componentLight     = Engine.CreateComponent(ComponentType::Light);
    
    // Check object creation
    if (gameObject == nullptr) mLogString += msgFailedToCreateObject;
    
    // Check component creation
    if (componentEntity    == nullptr) mLogString += msgFailedToCreateComponent;
    if (componentRigidBody == nullptr) mLogString += msgFailedToCreateComponent;
    if (componentCamera    == nullptr) mLogString += msgFailedToCreateComponent;
    if (componentScript    == nullptr) mLogString += msgFailedToCreateComponent;
    if (componentLight     == nullptr) mLogString += msgFailedToCreateComponent;
    
    gameObject->AddComponent(componentEntity);
    gameObject->AddComponent(componentRigidBody);
    gameObject->AddComponent(componentCamera);
    gameObject->AddComponent(componentScript);
    gameObject->AddComponent(componentLight);
    
    componentEntity    = gameObject->FindComponent(ComponentType::Renderer);
    componentRigidBody = gameObject->FindComponent(ComponentType::RigidBody);
    componentCamera    = gameObject->FindComponent(ComponentType::Camera);
    componentScript    = gameObject->FindComponent(ComponentType::Script);
    componentLight     = gameObject->FindComponent(ComponentType::Light);
    
    // Check component attachment
    Entity*          getRenderer  = (Entity*)         componentEntity->GetComponent();
    rp3d::RigidBody* getRigidBody = (rp3d::RigidBody*)componentRigidBody->GetComponent();
    Camera*          getCamera    = (Camera*)         componentCamera->GetComponent();
    Script*          getScript    = (Script*)         componentScript->GetComponent();
    Light*           getLight     = (Light*)          componentLight->GetComponent();
    
    if (getRenderer  == nullptr) mLogString += msgFailedToAttachComponent;
    if (getRigidBody == nullptr) mLogString += msgFailedToAttachComponent;
    if (getCamera    == nullptr) mLogString += msgFailedToAttachComponent;
    if (getScript    == nullptr) mLogString += msgFailedToAttachComponent;
    if (getLight     == nullptr) mLogString += msgFailedToAttachComponent;
    
    gameObject->RemoveComponent(componentEntity);
    gameObject->RemoveComponent(componentRigidBody);
    gameObject->RemoveComponent(componentCamera);
    gameObject->RemoveComponent(componentScript);
    gameObject->RemoveComponent(componentLight);
    
    // Check component detachment
    if (gameObject->FindComponent(ComponentType::Renderer)  != nullptr) mLogString += msgFailedToDetachComponent;
    if (gameObject->FindComponent(ComponentType::RigidBody) != nullptr) mLogString += msgFailedToDetachComponent;
    if (gameObject->FindComponent(ComponentType::Camera)    != nullptr) mLogString += msgFailedToDetachComponent;
    if (gameObject->FindComponent(ComponentType::Script)    != nullptr) mLogString += msgFailedToDetachComponent;
    if (gameObject->FindComponent(ComponentType::Light)     != nullptr) mLogString += msgFailedToDetachComponent;
    
    // Destroy components
    Engine.DestroyComponent(componentEntity);
    Engine.DestroyComponent(componentRigidBody);
    Engine.DestroyComponent(componentCamera);
    Engine.DestroyComponent(componentScript);
    Engine.DestroyComponent(componentLight);
    
    Engine.DestroyGameObject(gameObject);
    
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



void ApplicationTest::TestComponentObject(void) {
    
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


