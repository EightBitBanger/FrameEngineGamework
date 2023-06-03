#include <iostream>
#include <string>

#include "../framework.h"
#include "../../source/Engine/Engine.h"
extern EngineSystemManager  Engine;


void ApplicationTest::TestGameObject(void) {
    if (hasTestFailed) return;
    
    std::cout << "Game objects............ ";
    
    GameObject* gameObject = Engine.CreateGameObject();
    
    Component* componentEntity    = Engine.CreateComponent(ComponentType::Renderer);
    Component* componentRigidBody = Engine.CreateComponent(ComponentType::RigidBody);
    Component* componentCamera    = Engine.CreateComponent(ComponentType::Camera);
    Component* componentScript    = Engine.CreateComponent(ComponentType::Script);
    Component* componentLight     = Engine.CreateComponent(ComponentType::Light);
    
    // Check object creation
    if (gameObject == nullptr) mLogString += msgFailedObjectCreate;
    
    // Check component creation
    if (componentEntity    == nullptr) mLogString += msgFailedObjectCreate;
    if (componentRigidBody == nullptr) mLogString += msgFailedObjectCreate;
    if (componentCamera    == nullptr) mLogString += msgFailedObjectCreate;
    if (componentScript    == nullptr) mLogString += msgFailedObjectCreate;
    if (componentLight     == nullptr) mLogString += msgFailedObjectCreate;
    
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
    
    Entity*          getRenderer  = (Entity*)         componentEntity->GetComponent();
    rp3d::RigidBody* getRigidBody = (rp3d::RigidBody*)componentRigidBody->GetComponent();
    Camera*          getCamera    = (Camera*)         componentCamera->GetComponent();
    Script*          getScript    = (Script*)         componentScript->GetComponent();
    Light*           getLight     = (Light*)          componentLight->GetComponent();
    
    // Check component attachment
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
    
    return;
}

