#include <iostream>
#include <string>


#include "../source/Engine/Engine.h"
extern EngineSystemManager  Engine;

#include "applicationTest.h"
ApplicationTest Test;


void ApplicationTest::Initiate(void) {
    std::cout << "Running tests" << std::endl << std::endl;
    
    
}

void ApplicationTest::TestGameObject(void) {
    int failState=0;
    std::cout << "Game object construction...";
    
    GameObject* gameObject = Engine.CreateGameObject();
    Component* componentEntity = Engine.CreateComponent(ComponentType::Renderer);
    Component* componentRigidBody = Engine.CreateComponent(ComponentType::RigidBody);
    Component* componentCamera = Engine.CreateComponent(ComponentType::Camera);
    
    gameObject->AddComponent(componentEntity);
    gameObject->AddComponent(componentRigidBody);
    gameObject->AddComponent(componentCamera);
    
    componentEntity    = gameObject->FindComponent(ComponentType::Renderer);
    componentRigidBody = gameObject->FindComponent(ComponentType::RigidBody);
    componentCamera    = gameObject->FindComponent(ComponentType::Camera);
    
    Entity*    getRenderer  = (Entity*)componentEntity->GetComponent();
    rp3d::RigidBody* getRigidBody = (rp3d::RigidBody*)componentRigidBody->GetComponent();
    Camera*    getCamera    = (Camera*)componentCamera->GetComponent();
    
    if (getRenderer == nullptr)  failState++;
    if (getRigidBody == nullptr) failState++;
    if (getCamera == nullptr)    failState++;
    
    Engine.DestroyGameObject(gameObject);
    if (failState == 0) {
        std::cout << "pass" << std::endl;
    } else {
        std::cout << "fail" << std::endl;
    }
    return;
}

