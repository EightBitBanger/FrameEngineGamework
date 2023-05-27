#include <iostream>
#include <string>


#include "../source/Engine/Engine.h"
#include "../source/Application/ApplicationLayer.h"
extern EngineSystemManager  Engine;
extern ApplicationLayer     Application;

#include "applicationTest.h"

ApplicationTest::ApplicationTest() : 
    mFailureCount(0)
{}

void ApplicationTest::Initiate(void) {
    std::cout << "Running tests" << std::endl << std::endl;
    
}

void ApplicationTest::Complete(void) {
    std::cout << std::endl << "Complete" << std::endl << std::endl;
}

int ApplicationTest::GetFailureCount(void) {
    return mFailureCount;
}

void ApplicationTest::LogFail(std::string message) {
    std::cout << "failed :: " << message << std::endl;
    mFailureCount++;
}

void ApplicationTest::TestGameObject(void) {
    
    std::string failMessage="";
    int mFailureCount=0;
    
    std::string msgFailedToCreateObject    = "object creation";
    std::string msgFailedToAttachComponent = "component attachment";
    std::string msgFailedToDetachComponent = "component detachment";
    std::string msgFailedToCreateComponent = "component creation";
    
    std::cout << "Game object construction... " << std::endl;
    
    GameObject* gameObject = Engine.CreateGameObject();
    
    Component* componentEntity    = Engine.CreateComponent(ComponentType::Renderer);
    Component* componentRigidBody = Engine.CreateComponent(ComponentType::RigidBody);
    Component* componentCamera    = Engine.CreateComponent(ComponentType::Camera);
    Component* componentScript    = Engine.CreateComponent(ComponentType::Script);
    Component* componentLight     = Engine.CreateComponent(ComponentType::Light);
    
    // Check object creation
    if (gameObject == nullptr) LogFail(msgFailedToCreateObject);
    
    // Check component creation
    if (componentEntity    == nullptr) LogFail(msgFailedToCreateComponent);
    if (componentRigidBody == nullptr) LogFail(msgFailedToCreateComponent);
    if (componentCamera    == nullptr) LogFail(msgFailedToCreateComponent);
    if (componentScript    == nullptr) LogFail(msgFailedToCreateComponent);
    if (componentLight     == nullptr) LogFail(msgFailedToCreateComponent);
    
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
    
    if (getRenderer  == nullptr) LogFail(msgFailedToAttachComponent);
    if (getRigidBody == nullptr) LogFail(msgFailedToAttachComponent);
    if (getCamera    == nullptr) LogFail(msgFailedToAttachComponent);
    if (getScript    == nullptr) LogFail(msgFailedToAttachComponent);
    if (getLight     == nullptr) LogFail(msgFailedToAttachComponent);
    
    gameObject->RemoveComponent(componentEntity);
    gameObject->RemoveComponent(componentRigidBody);
    gameObject->RemoveComponent(componentCamera);
    gameObject->RemoveComponent(componentScript);
    gameObject->RemoveComponent(componentLight);
    
    // Check component detachment
    if (gameObject->FindComponent(ComponentType::Renderer)  != nullptr) LogFail(msgFailedToDetachComponent);
    if (gameObject->FindComponent(ComponentType::RigidBody) != nullptr) LogFail(msgFailedToDetachComponent);
    if (gameObject->FindComponent(ComponentType::Camera)    != nullptr) LogFail(msgFailedToDetachComponent);
    if (gameObject->FindComponent(ComponentType::Script)    != nullptr) LogFail(msgFailedToDetachComponent);
    if (gameObject->FindComponent(ComponentType::Light)     != nullptr) LogFail(msgFailedToDetachComponent);
    
    // Destroy components
    Engine.DestroyComponent(componentEntity);
    Engine.DestroyComponent(componentRigidBody);
    Engine.DestroyComponent(componentCamera);
    Engine.DestroyComponent(componentScript);
    Engine.DestroyComponent(componentLight);
    
    Engine.DestroyGameObject(gameObject);
    
    if (GetFailureCount() ) {
        
    }
    
    return;
}

