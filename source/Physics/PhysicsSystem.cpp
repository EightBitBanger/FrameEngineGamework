#include <ReactPhysics3d/ReactPhysics3d.h>
#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem() {}

void PhysicsSystem::Initiate(void) {
    
    world = common.createPhysicsWorld();
    return;
}

void PhysicsSystem::SetWorldGravity(float x, float y, float z) {world->setGravity(rp3d::Vector3(x, y, z));}


rp3d::RigidBody* PhysicsSystem::CreateRigidBody(float x, float y, float z) {
    
    rp3d::Vector3 position = rp3d::Vector3(x, y, z);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform physicsTransform = rp3d::Transform(position, orientation);
    
    rp3d::RigidBody* body = world->createRigidBody(physicsTransform);
    
    return body;
}

void PhysicsSystem::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    assert(rigidBodyPtr != nullptr);
    world->destroyRigidBody(rigidBodyPtr);
}



rp3d::BoxShape* PhysicsSystem::CreateBoxShape(float xscale, float yscale, float zscale) {
    rp3d::Vector3 scale(xscale, yscale, zscale);
    return common.createBoxShape(scale);
}

