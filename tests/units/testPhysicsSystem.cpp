#include <iostream>
#include <string>

#include "../framework.h"
#include "../../src/Physics/PhysicsSystem.h"
extern PhysicsSystem Physics;


void TestFramework::TestPhysicsSystem(void) {
    if (hasTestFailed) return;
    
    std::cout << "Physics system.......... ";
    
    // Clear the physics world
    Physics.common.destroyPhysicsWorld(Physics.world);
    Physics.world = Physics.common.createPhysicsWorld();
    
    // Test set/get on create
    rp3d::RigidBody* rigidBody = Physics.CreateRigidBody(2.0, 4.0, 8.0);
    if (rigidBody->getTransform().getPosition() != rp3d::Vector3(2.0, 4.0, 8.0)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    Physics.DestroyRigidBody(rigidBody);
    
    // Test set/get physics world gravity
    Physics.SetWorldGravity(20.0, 40.0, 100.0);
    if (Physics.world->getGravity() != rp3d::Vector3(20.0, 40.0, 100.0)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    // Test collision objects
    rp3d::BoxShape*     boxShape = Physics.CreateColliderBox(10.0, 20.0, 30.0);
    rp3d::SphereShape*  sphereShape = Physics.CreateColliderSphere(20.0);
    rp3d::CapsuleShape* capsuleShape = Physics.CreateColliderCapsule(40.0, 60.0);
    if (boxShape == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (sphereShape == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (capsuleShape == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    
    // Test collider shapes
    if (boxShape->getHalfExtents() != rp3d::Vector3(10.0, 20.0, 30.0)) Throw(msgFailedSetGet, __FILE__, __LINE__);
    if (sphereShape->getRadius() != 20.0) Throw(msgFailedSetGet, __FILE__, __LINE__);
    if (capsuleShape->getHeight() != 60.0) Throw(msgFailedSetGet, __FILE__, __LINE__);
    if (capsuleShape->getRadius() != 40.0) Throw(msgFailedSetGet, __FILE__, __LINE__);
    
    // Clear the physics world
    Physics.common.destroyPhysicsWorld(Physics.world);
    Physics.world = Physics.common.createPhysicsWorld();
    
    return;
}

