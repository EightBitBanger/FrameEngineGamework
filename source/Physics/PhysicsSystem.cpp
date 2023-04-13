#include "PhysicsSystem.h"
#include <ReactPhysics3d/ReactPhysics3d.h>

PhysicsSystem::PhysicsSystem() {
    
    world = common.createPhysicsWorld();
}


void PhysicsSystem::SetGravity(float x, float y, float z) {world->setGravity(rp3d::Vector3(x, y, z));}


rp3d::RigidBody* PhysicsSystem::CreateRigidBody(float x=0.0, float y=0.0, float z=0.0) {
    
    rp3d::Vector3 position = rp3d::Vector3(x, y, z);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform physicsTransform = rp3d::Transform(position, orientation);
    
    return world->createRigidBody(physicsTransform);
}

void PhysicsSystem::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    world->destroyRigidBody(rigidBodyPtr);
}

