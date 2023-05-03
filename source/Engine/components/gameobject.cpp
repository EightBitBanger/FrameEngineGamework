#include "gameobject.h"

GameObject::GameObject() {
    
    name = "";
    
    isActive = true;
    
    rigidBodyCache = nullptr;
    
}



//
// Components

void GameObject::AddComponent(Component* component) {
    assert(component != nullptr);
    
    // Set the shortcut pointers
    if (component->GetType() == COMPONENT_TYPE_RIGIDBODY) {
        rigidBodyCache = (rp3d::RigidBody*)component->GetComponent();
        // Update the rigid body position on attach
        if (entityCache != nullptr) SetPosition(entityCache->transform.position.x,
                                                entityCache->transform.position.y,
                                                entityCache->transform.position.z);
    }
    if (component->GetType() == COMPONENT_TYPE_RENDERER) entityCache = (Entity*)component->GetComponent();
    
    componentList.push_back(component);
    return;
}

bool GameObject::RemoveComponent(Component* component) {
    assert(component != nullptr);
    for (std::vector<Component*>::iterator it = componentList.begin(); it != componentList.end(); ++it) {
        Component* thisComponentPtr = *it;
        if (component == thisComponentPtr) {
            
            // Null the shortcut pointers
            if (thisComponentPtr->GetType() == COMPONENT_TYPE_RIGIDBODY) rigidBodyCache = nullptr;
            if (thisComponentPtr->GetType() == COMPONENT_TYPE_RENDERER) entityCache = nullptr;
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

void GameObject::SetPosition(float x, float y, float z) {
    if (rigidBodyCache != nullptr) {
        rp3d::Vector3 position(x, y, z);
        rp3d::Transform bodyTransform = rigidBodyCache->getTransform();
        bodyTransform.setPosition(position);
        rigidBodyCache->setTransform(bodyTransform);
    } else {
        if (entityCache != nullptr) {
            entityCache->transform.position = glm::vec3(x, y, z);
        }
    }
    return;
}

void GameObject::AddForce(float x, float y, float z) {
    assert(rigidBodyCache != nullptr);
    rp3d::Vector3 nudge(x, y, z);
    rigidBodyCache->applyLocalForceAtCenterOfMass(nudge);
    return;
}

void GameObject::AddTorque(float x, float y, float z) {
    assert(rigidBodyCache != nullptr);
    rp3d::Vector3 twist(x, y, z);
    rigidBodyCache->applyLocalTorque(twist);
    return;
}

void GameObject::SetMass(float mass) {
    assert(rigidBodyCache != nullptr);
    rigidBodyCache->setMass(mass);
}

void GameObject::SetLinearDamping(float damping) {
    assert(rigidBodyCache != nullptr);
    rigidBodyCache->setLinearDamping(damping);
}

void GameObject::SetAngularDamping(float damping) {
    assert(rigidBodyCache != nullptr);
    rigidBodyCache->setAngularDamping(damping);
}

void GameObject::EnableGravity(bool enabled) {
    assert(rigidBodyCache != nullptr);
    rigidBodyCache->enableGravity(enabled);
}


void GameObject::CalculatePhysics(void) {
    assert(rigidBodyCache != nullptr);
    rigidBodyCache->updateMassFromColliders();
    rigidBodyCache->updateLocalCenterOfMassFromColliders();
    rigidBodyCache->updateLocalInertiaTensorFromColliders();
    return;
}


void GameObject::SetLinearAxisLockFactor(float x, float y, float z) {
    assert(rigidBodyCache != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBodyCache->setLinearLockAxisFactor(lockFactor);
    return;
}

void GameObject::SetAngularAxisLockFactor(float x, float y, float z) {
    assert(rigidBodyCache != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBodyCache->setAngularLockAxisFactor(lockFactor);
    return;
}


void GameObject::AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z) {
    assert(rigidBodyCache != nullptr);
    assert(boxShape != nullptr);
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBodyCache->addCollider(boxShape, offsetTransform);
    
    return;
}

void GameObject::AddCollider(ColliderTag* colliderTag, float x, float y, float z) {
    assert(rigidBodyCache != nullptr);
    assert(colliderTag != nullptr);
    if (colliderTag->isStatic) {
        rigidBodyCache->setType(rp3d::BodyType::STATIC);
    } else {
        rigidBodyCache->setType(rp3d::BodyType::DYNAMIC);
    }
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBodyCache->addCollider(colliderTag->colliderShape, offsetTransform);
    
    return;
}


void GameObject::SetRigidBodyStatic(void) {
    assert(rigidBodyCache != nullptr);
    rigidBodyCache->setType(rp3d::BodyType::STATIC);
    return;
}


void GameObject::SetRigidBodyDynamic(void) {
    //if (rigidBodyCache == nullptr) return;
    assert(rigidBodyCache != nullptr);
    rigidBodyCache->setType(rp3d::BodyType::DYNAMIC);
    return;
}


