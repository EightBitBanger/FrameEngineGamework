#include "entity.h"


Entity::Entity() {
    
    name = "";
    
    mesh       = nullptr;
    material   = nullptr;
    script     = nullptr;
    rigidBody  = nullptr;
    
    return;
}

Entity::~Entity() {
    
    if (rigidBody != nullptr) 
        Physics.DestroyRigidBody(rigidBody);
    
    return;
}



void Entity::AttachMesh(Mesh* meshPtr) {
    mesh = meshPtr;
    return;
}

void Entity::DetachMesh(void) {
    mesh = nullptr;
    return;
}

Mesh* Entity::GetAttachedMesh(void) {
    return mesh;
}


void Entity::AttachMaterial(Material* materialPtr) {
    material = materialPtr;
    return;
}

void Entity::DetachMaterial(void) {
    material = nullptr;
    return;
}

Material* Entity::GetAttachedMaterial(void) {
    return material;
}


void Entity::AttachScript(Script* scriptPtr) {
    script = scriptPtr;
    return;
}

void Entity::DetachScript(void) {
    script = nullptr;
    return;
}

Script* Entity::GetAttachedScript(void) {
    return script;
}


void Entity::AttachRidigBody(rp3d::RigidBody* rigidBodyPtr){
    rigidBody = rigidBodyPtr;
    return;
}

void Entity::DetachRidigBody(void) {
    rigidBody = nullptr;
    return;
}

rp3d::RigidBody* Entity::GetAttachedRidigBody(){
    return rigidBody;
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


void Entity::AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z) {
    assert(rigidBody != nullptr);
    assert(boxShape != nullptr);
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBody->addCollider(boxShape, offsetTransform);
    
    return;
}

void Entity::AddCollider(ColliderTag* colliderTag, float x, float y, float z) {
    assert(rigidBody != nullptr);
    assert(colliderTag != nullptr);
    if (colliderTag->isStatic) {
        SetRigidBodyStatic();
    } else {
        SetRigidBodyDynamic();
    }
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBody->addCollider(colliderTag->colliderShape, offsetTransform);
    
    return;
}


void Entity::SetRigidBodyStatic(void) {
    assert(rigidBody != nullptr);
    rigidBody->setType(rp3d::BodyType::STATIC);
    rp3d::Vector3 lockFactor(0,0,0);
    rigidBody->setLinearLockAxisFactor(lockFactor);
    rigidBody->setAngularLockAxisFactor(lockFactor);
    return;
}


void Entity::SetRigidBodyDynamic(void) {
    assert(rigidBody != nullptr);
    rigidBody->setType(rp3d::BodyType::DYNAMIC);
    rp3d::Vector3 lockFactor(1,1,1);
    rigidBody->setLinearLockAxisFactor(lockFactor);
    rigidBody->setAngularLockAxisFactor(lockFactor);
    return;
}





