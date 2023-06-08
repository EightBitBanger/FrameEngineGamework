#include <iostream>
#include <string>

#include "../framework.h"
#include "../../source/Engine/Engine.h"
extern EngineSystemManager  Engine;


void TestFramework::TestGameObject(void) {
    if (hasTestFailed) return;
    
    std::cout << "Game objects............ ";
    
    GameObject* gameObject1     = Engine.CreateGameObject();
    Component*  componentEntity = Engine.CreateComponent(ComponentType::Renderer);
    
    // Check object creation
    if (gameObject1 == nullptr) mLogString += msgFailedObjectCreate;
    
    // Test component attachment
    gameObject1->AddComponent(componentEntity);
    componentEntity = gameObject1->FindComponent(ComponentType::Renderer);
    Entity* getRenderer  = (Entity*)componentEntity->GetComponent();
    if (getRenderer  == nullptr) mLogString += msgFailedToAttachComponent;
    
    // Test component detachment
    gameObject1->RemoveComponent(componentEntity);
    if (gameObject1->FindComponent(ComponentType::Renderer)  != nullptr) mLogString += msgFailedToDetachComponent;
    
    // Test game object should destroy its components
    gameObject1->AddComponent(componentEntity);
    Engine.DestroyGameObject(gameObject1);
    if (Engine.GetComponentCount() > 0) mLogString += msgFailedToDestroyAttachment;
    
    // Check game object`s get and set functions
    GameObject* gameObject2     = Engine.CreateGameObject();
    Component*  componentRigidBody = Engine.CreateComponent(ComponentType::RigidBody);
    gameObject2->AddComponent(componentRigidBody);
    rp3d::RigidBody* rigidBody = (rp3d::RigidBody*)componentRigidBody->GetComponent();
    
    // Test mass
    gameObject2->SetMass(2.0);
    if (rigidBody->getMass() != 2.0) mLogString += msgFailedSetGet;
    
    // Test damping
    gameObject2->SetLinearDamping(8.0);
    if (rigidBody->getLinearDamping() != 8.0) mLogString += msgFailedSetGet;
    gameObject2->SetAngularDamping(4.0);
    if (rigidBody->getAngularDamping() != 4.0) mLogString += msgFailedSetGet;
    
    // Test axial locks
    gameObject2->SetLinearAxisLockFactor(1.0, 3.0, 5.0);
    if (rigidBody->getLinearLockAxisFactor() != rp3d::Vector3(1.0, 3.0, 5.0)) mLogString += msgFailedSetGet;
    gameObject2->SetAngularAxisLockFactor(2.0, 4.0, 6.0);
    if (rigidBody->getAngularLockAxisFactor() != rp3d::Vector3(2.0, 4.0, 6.0)) mLogString += msgFailedSetGet;
    
    Engine.DestroyGameObject(gameObject2);
    
    return;
}





