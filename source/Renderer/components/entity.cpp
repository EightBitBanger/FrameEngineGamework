#include "entity.h"


Entity::Entity() {
    
    mesh       = nullptr;
    material   = nullptr;
    script     = nullptr;
    rigidBody  = nullptr;
    
}

Entity::~Entity() {
    
    if (rigidBody == nullptr) return;
    
    for (std::vector<rp3d::BoxShape*>::iterator it = BoxList.begin(); it != BoxList.end(); ++it) {
        
        Physics.common.destroyBoxShape(*it);
    }
    
    int colliderCount = rigidBody->getNbColliders();
    for (int i=0; i < colliderCount; i++) {
        rigidBody->removeCollider(rigidBody->getCollider(i));
    }
    
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
}




void Entity::AddBoxCollider(float x, float y, float z, float xscale, float yscale, float zscale) {
    
    if (rigidBody == nullptr) return;
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rp3d::Vector3 colliderScale(xscale, yscale, zscale);
    rp3d::BoxShape* colliderShape = Physics.common.createBoxShape(colliderScale);
    
    rigidBody->addCollider(colliderShape, offsetTransform);
    
    BoxList.push_back(colliderShape);
    
    return;
}


