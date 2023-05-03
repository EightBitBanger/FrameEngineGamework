#include "gameobject.h"

GameObject::GameObject() {
    
    name = "";
    
    isActive = true;
    
    rigidBody = nullptr;
    
}





//
// Components
//

void GameObject::AddComponent(Component* component) {
    assert(component != nullptr);
    
    if (component->GetType() == COMPONENT_TYPE_RIGIDBODY) 
        rigidBody = (rp3d::RigidBody*)component->GetComponent();
    
    componentList.push_back(component);
    return;
}

bool GameObject::RemoveComponent(Component* component) {
    assert(component != nullptr);
    for (std::vector<Component*>::iterator it = componentList.begin(); it != componentList.end(); ++it) {
        Component* thisComponentPtr = *it;
        if (component == thisComponentPtr) {
            componentList.erase(it);
            return true;
        }
    }
    return false;
}

Component* GameObject::FindComponent(unsigned int component_type) {
    for (std::vector<Component*>::iterator it = componentList.begin(); it != componentList.end(); ++it) {
        Component* thisComponentPtr = *it;
        if (component_type == thisComponentPtr->GetType()) {
            return thisComponentPtr;
        }
    }
    return nullptr;
}


Component* GameObject::GetComponent(unsigned int index) {
    assert(index < componentList.size());
    return componentList[index];
}

unsigned int GameObject::GetComponentCount(void) {
    return componentList.size();
}





//
// Physics functions
//

void GameObject::AddForce(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 nudge(x, y, z);
    rigidBody->applyLocalForceAtCenterOfMass(nudge);
    return;
}

void GameObject::AddTorque(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 twist(x, y, z);
    rigidBody->applyLocalTorque(twist);
    return;
}

void GameObject::SetMass(float mass) {
    assert(rigidBody != nullptr);
    rigidBody->setMass(mass);
}

void GameObject::SetLinearDamping(float damping) {
    assert(rigidBody != nullptr);
    rigidBody->setLinearDamping(damping);
}

void GameObject::SetAngularDamping(float damping) {
    assert(rigidBody != nullptr);
    rigidBody->setAngularDamping(damping);
}

void GameObject::EnableGravity(bool enabled) {
    assert(rigidBody != nullptr);
    rigidBody->enableGravity(enabled);
}


void GameObject::CalculatePhysics(void) {
    assert(rigidBody != nullptr);
    rigidBody->updateMassFromColliders();
    rigidBody->updateLocalCenterOfMassFromColliders();
    rigidBody->updateLocalInertiaTensorFromColliders();
    return;
}


void GameObject::SetLinearAxisLockFactor(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBody->setLinearLockAxisFactor(lockFactor);
    return;
}

void GameObject::SetAngularAxisLockFactor(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBody->setAngularLockAxisFactor(lockFactor);
    return;
}


void GameObject::AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z) {
    assert(rigidBody != nullptr);
    assert(boxShape != nullptr);
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBody->addCollider(boxShape, offsetTransform);
    
    return;
}

void GameObject::AddCollider(ColliderTag* colliderTag, float x, float y, float z) {
    assert(rigidBody != nullptr);
    assert(colliderTag != nullptr);
    if (colliderTag->isStatic) {
        rigidBody->setType(rp3d::BodyType::STATIC);
    } else {
        rigidBody->setType(rp3d::BodyType::DYNAMIC);
    }
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBody->addCollider(colliderTag->colliderShape, offsetTransform);
    
    return;
}


void GameObject::SetRigidBodyStatic(void) {
    assert(rigidBody != nullptr);
    rigidBody->setType(rp3d::BodyType::STATIC);
    return;
}


void GameObject::SetRigidBodyDynamic(void) {
    //if (rigidBody == nullptr) return;
    assert(rigidBody != nullptr);
    rigidBody->setType(rp3d::BodyType::DYNAMIC);
    return;
}


