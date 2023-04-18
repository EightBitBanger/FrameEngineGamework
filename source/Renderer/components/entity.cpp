#include "entity.h"


Entity::Entity() {
    
    mesh       = nullptr;
    material   = nullptr;
    script     = nullptr;
    rigidBody  = nullptr;
    
}

Entity::~Entity() {
    
    if (rigidBody != nullptr) 
        Physics.DestroyRigidBody(rigidBody);
    
    return;
}


void Entity::AddForce(float x, float y, float z) {
    if (rigidBody == nullptr) return;
    rp3d::Vector3 nudge = rp3d::Vector3(x, y, z);
    rigidBody->applyLocalForceAtCenterOfMass(nudge);
    return;
}

void Entity::AddTorque(float x, float y, float z) {
    if (rigidBody == nullptr) return;
    rp3d::Vector3 twist = rp3d::Vector3(x, y, z);
    rigidBody->applyLocalTorque(twist);
    return;
}

void Entity::SetMass(float mass) {
    rigidBody->setMass(mass);
}

void Entity::SetLinearDamping(float damping) {
    rigidBody->setLinearDamping(damping);
}

void Entity::SetAngularDamping(float damping) {
    rigidBody->setAngularDamping(damping);
}

void Entity::EnableGravity(bool enabled) {
    rigidBody->enableGravity(enabled);
}


void Entity::CalculatePhysics(void) {
    if (rigidBody == nullptr) return;
    rigidBody->updateMassFromColliders();
    rigidBody->updateLocalCenterOfMassFromColliders();
    rigidBody->updateLocalInertiaTensorFromColliders();
    return;
}


void Entity::SyncRigidBody(void) {
    
    rp3d::Transform physicsTransform = rigidBody->getTransform();
    
    rp3d::Vector3 physicsPosition = physicsTransform.getPosition();
    rp3d::Quaternion quaternion   = physicsTransform.getOrientation();
    rp3d::Vector3 physicsRotation = quaternion.getVectorV();
    
    transform.position.x = physicsPosition.x;
    transform.position.y = physicsPosition.y;
    transform.position.z = physicsPosition.z;
    
    transform.rotation.x = glm::degrees(physicsRotation.x);
    transform.rotation.y = glm::degrees(physicsRotation.y);
    transform.rotation.z = glm::degrees(physicsRotation.z);
    return;
}


void Entity::SetLinearAxisLockFactor(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBody->setLinearLockAxisFactor(lockFactor);
    return;
}

void Entity::SetAngularAxisLockFactor(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBody->setAngularLockAxisFactor(lockFactor);
    return;
}


void Entity::AddCollider(rp3d::BoxShape* boxShape, float x, float y, float z) {
    assert(rigidBody != nullptr);
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBody->addCollider(boxShape, offsetTransform);
    
    return;
}


