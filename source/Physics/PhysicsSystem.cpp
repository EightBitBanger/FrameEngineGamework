#include "PhysicsSystem.h"


PhysicsSystem::PhysicsSystem() {
    
    mRigidBodyFreeList.reserve(512);
    
    world = nullptr;
    return;
}

void PhysicsSystem::Initiate(void) {
    assert(world == nullptr);
    rp3d::PhysicsWorld::WorldSettings worldSettings;
    worldSettings.defaultBounciness            = 0.5;
    worldSettings.defaultFrictionCoefficient   = 0.7;
    worldSettings.restitutionVelocityThreshold = 0.9;
    
    world = common.createPhysicsWorld(worldSettings);
    
    SetWorldGravity(0, -9.98 * 3 * 3, 0);
    
    world->enableSleeping(true);
    world->setSleepLinearVelocity(0.1);
    world->setSleepAngularVelocity(0.1);
    
    return;
}

void PhysicsSystem::Shutdown(void) {
    
    while (mRigidBodyFreeList.size() > 0) {
        DestroyRigidBody( RemoveRigidBodyFromFreeList() );
    }
    return;
}


void PhysicsSystem::SetWorldGravity(float x, float y, float z) {
    assert(world != nullptr);
    world->setGravity(rp3d::Vector3(x, y, z));
    return;
}

rp3d::RigidBody* PhysicsSystem::CreateRigidBody(float x, float y, float z) {
    assert(world != nullptr);
    
    rp3d::Vector3 position = rp3d::Vector3(x, y, z);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform physicsTransform = rp3d::Transform(position, orientation);
    
    rp3d::RigidBody* body = RemoveRigidBodyFromFreeList();
    if (body != nullptr) {
        body->setTransform(physicsTransform);
        return body;
    }
    
    body = world->createRigidBody(physicsTransform);
    
    return body;
}

void PhysicsSystem::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    assert(rigidBodyPtr != nullptr);
    
    // !! Warning: leak on rigid body destroy !!
    //world->destroyRigidBody(rigidBodyPtr);
    
    // Use a free list instead...
    AddRigidBodyToFreeList(rigidBodyPtr);
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



void PhysicsSystem::AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr) {
    mRigidBodyFreeList.push_back(rigidBodyPtr);
    rigidBodyPtr->setIsActive(false);
    return;
}

rp3d::RigidBody* PhysicsSystem::RemoveRigidBodyFromFreeList(void) {
    if (mRigidBodyFreeList.size() == 0) return nullptr;
    rp3d::RigidBody* bodyPtr = mRigidBodyFreeList[mRigidBodyFreeList.size()-1];
    bodyPtr->setIsActive(true);
    mRigidBodyFreeList.erase( mRigidBodyFreeList.end()-1 );
    return bodyPtr;
}

