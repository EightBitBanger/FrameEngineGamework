#include <ReactPhysics3d/ReactPhysics3d.h>
#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem() {}

void PhysicsSystem::Initiate(void) {
    
    world = common.createPhysicsWorld();
    
    SetWorldGravity(0, -9.98 * 2 * 2, 0);
    
    world->enableSleeping(true);
    
    world->setSleepLinearVelocity(0.8);
    world->setSleepAngularVelocity(0.8);
    
    return;
}

void PhysicsSystem::SetWorldGravity(float x, float y, float z) {world->setGravity(rp3d::Vector3(x, y, z));}


rp3d::RigidBody* PhysicsSystem::CreateRigidBody(float x, float y, float z) {
    
    rp3d::Vector3 position = rp3d::Vector3(x, y, z);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform physicsTransform = rp3d::Transform(position, orientation);
    
    rp3d::RigidBody* body = world->createRigidBody(physicsTransform);
    
    body->setMass(1);
    body->setLinearDamping(0);
    body->setAngularDamping(0.087);
    
    return body;
}

void PhysicsSystem::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    assert(rigidBodyPtr != nullptr);
    world->destroyRigidBody(rigidBodyPtr);
}


rp3d::BoxShape* PhysicsSystem::CreateColliderBox(float xscale, float yscale, float zscale) {
    rp3d::Vector3 scale(xscale, yscale, zscale);
    return common.createBoxShape(scale);
}

rp3d::SphereShape* PhysicsSystem::CreateColliderSphere(float radius) {
    return common.createSphereShape(radius);
}

rp3d::CapsuleShape* PhysicsSystem::CreateColliderCapsule(float radius, float height) {
    return common.createCapsuleShape(radius, height);
}


