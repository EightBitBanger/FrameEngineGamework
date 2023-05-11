#include "gameobject.h"

GameObject::GameObject() {
    
    name = "";
    
    isActive = true;
    
    mRigidBodyCache = nullptr;
    
}



//
// Components

void GameObject::AddComponent(Component* component) {
    assert(component != nullptr);
    
    // Set the shortcut pointers
    if (component->GetType() == COMPONENT_TYPE_RIGIDBODY) {
        mRigidBodyCache = (rp3d::RigidBody*)component->GetComponent();
        // Update the rigid body position on attach
        if (mEntityCache != nullptr) SetPosition(mEntityCache->transform.position.x,
                                                mEntityCache->transform.position.y,
                                                mEntityCache->transform.position.z);
    }
    if (component->GetType() == COMPONENT_TYPE_RENDERER) mEntityCache = (Entity*)component->GetComponent();
    if (component->GetType() == COMPONENT_TYPE_SCRIPT) {
        Script* scriptPtr = (Script*)component->GetComponent();
        scriptPtr->isActive = true;
    }
    
    mComponentList.push_back(component);
    return;
}

bool GameObject::RemoveComponent(Component* component) {
    assert(component != nullptr);
    for (std::vector<Component*>::iterator it = mComponentList.begin(); it != mComponentList.end(); ++it) {
        Component* thisComponentPtr = *it;
        if (component == thisComponentPtr) {
            
            // Null the shortcut pointers
            if (component->GetType() == COMPONENT_TYPE_RIGIDBODY) mRigidBodyCache = nullptr;
            if (component->GetType() == COMPONENT_TYPE_RENDERER) mEntityCache = nullptr;
            if (component->GetType() == COMPONENT_TYPE_SCRIPT) {
                Script* scriptPtr = (Script*)component->GetComponent();
                scriptPtr->isActive = false;
            }
            
            mComponentList.erase(it);
            return true;
        }
    }
    return false;
}

Component* GameObject::FindComponent(unsigned int component_type) {
    for (std::vector<Component*>::iterator it = mComponentList.begin(); it != mComponentList.end(); ++it) {
        Component* thisComponentPtr = *it;
        if (component_type == thisComponentPtr->GetType()) {
            return thisComponentPtr;
        }
    }
    return nullptr;
}


Component* GameObject::GetComponent(unsigned int index) {
    assert(index < mComponentList.size());
    return mComponentList[index];
}

unsigned int GameObject::GetComponentCount(void) {
    return mComponentList.size();
}


//
// Physics functions

void GameObject::SetPosition(float x, float y, float z) {
    if (mRigidBodyCache != nullptr) {
        rp3d::Vector3 position(x, y, z);
        rp3d::Transform bodyTransform = mRigidBodyCache->getTransform();
        bodyTransform.setPosition(position);
        mRigidBodyCache->setTransform(bodyTransform);
    }
    if (mEntityCache != nullptr) {
        mEntityCache->transform.position = glm::vec3(x, y, z);
    }
    return;
}

void GameObject::AddForce(float x, float y, float z) {
    assert(mRigidBodyCache != nullptr);
    rp3d::Vector3 nudge(x, y, z);
    mRigidBodyCache->applyLocalForceAtCenterOfMass(nudge);
    return;
}

void GameObject::AddTorque(float x, float y, float z) {
    assert(mRigidBodyCache != nullptr);
    rp3d::Vector3 twist(x, y, z);
    mRigidBodyCache->applyLocalTorque(twist);
    return;
}

void GameObject::SetMass(float mass) {
    assert(mRigidBodyCache != nullptr);
    mRigidBodyCache->setMass(mass);
}

void GameObject::SetLinearDamping(float damping) {
    assert(mRigidBodyCache != nullptr);
    mRigidBodyCache->setLinearDamping(damping);
}

void GameObject::SetAngularDamping(float damping) {
    assert(mRigidBodyCache != nullptr);
    mRigidBodyCache->setAngularDamping(damping);
}

void GameObject::EnableGravity(bool enabled) {
    assert(mRigidBodyCache != nullptr);
    mRigidBodyCache->enableGravity(enabled);
}


void GameObject::CalculatePhysics(void) {
    assert(mRigidBodyCache != nullptr);
    mRigidBodyCache->updateMassFromColliders();
    mRigidBodyCache->updateLocalCenterOfMassFromColliders();
    mRigidBodyCache->updateLocalInertiaTensorFromColliders();
    return;
}


void GameObject::SetLinearAxisLockFactor(float x, float y, float z) {
    assert(mRigidBodyCache != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    mRigidBodyCache->setLinearLockAxisFactor(lockFactor);
    return;
}

void GameObject::SetAngularAxisLockFactor(float x, float y, float z) {
    assert(mRigidBodyCache != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    mRigidBodyCache->setAngularLockAxisFactor(lockFactor);
    return;
}


void GameObject::AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z) {
    assert(mRigidBodyCache != nullptr);
    assert(boxShape != nullptr);
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    mRigidBodyCache->addCollider(boxShape, offsetTransform);
    
    return;
}

void GameObject::AddCollider(ColliderTag* colliderTag, float x, float y, float z) {
    assert(mRigidBodyCache != nullptr);
    assert(colliderTag != nullptr);
    if (colliderTag->isStatic) {
        mRigidBodyCache->setType(rp3d::BodyType::STATIC);
    } else {
        mRigidBodyCache->setType(rp3d::BodyType::DYNAMIC);
    }
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    mRigidBodyCache->addCollider(colliderTag->colliderShape, offsetTransform);
    
    return;
}


void GameObject::SetRigidBodyStatic(void) {
    assert(mRigidBodyCache != nullptr);
    mRigidBodyCache->setType(rp3d::BodyType::STATIC);
    return;
}


void GameObject::SetRigidBodyDynamic(void) {
    assert(mRigidBodyCache != nullptr);
    mRigidBodyCache->setType(rp3d::BodyType::DYNAMIC);
    return;
}


