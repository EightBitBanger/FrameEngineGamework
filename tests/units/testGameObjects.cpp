#include <iostream>
#include <string>

#include "../framework.h"
#include <GameEngineFramework/Engine/Engine.h>

extern EngineSystemManager  Engine;
extern EngineComponents     Components;


void TestFramework::TestGameObject(void) {
    if (hasTestFailed) return;
    
    std::cout << "Game objects............ ";
    
    GameObject* gameObject1     = Engine.CreateGameObject();
    Component*  componentEntity = Engine.CreateComponent<MeshRenderer>();
    
    // Check object creation
    if (gameObject1 == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    
    // Test component attachment
    gameObject1->AddComponent(componentEntity);
    MeshRenderer* getRenderer = gameObject1->GetComponent<MeshRenderer>();
    
    if (getRenderer  == nullptr) Throw(msgFailedToAttachComponent, __FILE__, __LINE__);
    
    // Test component detachment
    gameObject1->RemoveComponent(componentEntity);
    if (gameObject1->GetComponent<MeshRenderer>() != nullptr) Throw(msgFailedToDetachComponent, __FILE__, __LINE__);
    
    // Test game object should destroy its components
    gameObject1->AddComponent(componentEntity);
    Engine.DestroyGameObject(gameObject1);
    if (Engine.GetComponentCount() > 0) Throw(msgFailedToDestroyAttachment, __FILE__, __LINE__);
    
    // Check game object`s get and set functions
    GameObject* gameObject2     = Engine.CreateGameObject();
    Component*  componentRigidBody = Engine.CreateComponent<RigidBody>();
    gameObject2->AddComponent(componentRigidBody);
    rp3d::RigidBody* rigidBody = (rp3d::RigidBody*)componentRigidBody->GetComponent();
    
    // Test mass
    gameObject2->SetMass(2.0);
    if (rigidBody->getMass() != 2.0) Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    // Test damping
    gameObject2->SetLinearDamping(8.0);
    if (rigidBody->getLinearDamping() != 8.0) Throw(msgFailedSetGet, __FILE__, __LINE__);
    gameObject2->SetAngularDamping(4.0);
    if (rigidBody->getAngularDamping() != 4.0) Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    // Test axial locks
    gameObject2->SetLinearAxisLockFactor(1.0, 3.0, 5.0);
    if (rigidBody->getLinearLockAxisFactor() != rp3d::Vector3(1.0, 3.0, 5.0)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    gameObject2->SetAngularAxisLockFactor(2.0, 4.0, 6.0);
    if (rigidBody->getAngularLockAxisFactor() != rp3d::Vector3(2.0, 4.0, 6.0)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    Engine.DestroyGameObject(gameObject2);
    
    return;
}





