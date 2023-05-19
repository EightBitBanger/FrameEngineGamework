#include "gameobject.h"

GameObject::GameObject() {
    
    name = "";
    
    isActive = true;
    
    mCameraCache = nullptr;
    mRigidBodyCache = nullptr;
    mEntityCache = nullptr;
}

//
// Component functions
//

void GameObject::AddComponent(Component* component) {
    assert(component != nullptr);
    
    //
    // Cache the component objects
    
    // Rigid body component
    if (component->GetType() == ComponentType::RigidBody) {
        mRigidBodyCache = (rp3d::RigidBody*)component->GetComponent();
        // Update the rigid body position on attach
        if (mEntityCache != nullptr) SetPosition(mEntityCache->transform.position.x,
                                                 mEntityCache->transform.position.y,
                                                 mEntityCache->transform.position.z);
    }
    
    // Entity renderer component
    if (component->GetType() == ComponentType::Renderer) {
        mEntityCache = (Entity*)component->GetComponent();
    }
    
    // Camera component
    if (component->GetType() == ComponentType::Camera) {
        mCameraCache = (Camera*)component->GetComponent();
    }
    
    // Light component
    if (component->GetType() == ComponentType::Light) {
        mLightCache = (Light*)component->GetComponent();
    }
    
    mComponentList.push_back(component);
    return;
}

bool GameObject::RemoveComponent(Component* component) {
    assert(component != nullptr);
    for (std::vector<Component*>::iterator it = mComponentList.begin(); it != mComponentList.end(); ++it) {
        Component* thisComponentPtr = *it;
        if (component == thisComponentPtr) {
            
            // Null the cache pointers
            if (component->GetType() == ComponentType::RigidBody) mRigidBodyCache = nullptr;
            if (component->GetType() == ComponentType::Renderer)  mEntityCache = nullptr;
            if (component->GetType() == ComponentType::Camera)    mCameraCache = nullptr;
            if (component->GetType() == ComponentType::Light)     mLightCache = nullptr;
            
            mComponentList.erase(it);
            return true;
        }
    }
    return false;
}

Component* GameObject::FindComponent(ComponentType component_type) {
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

Camera* GameObject::GetCachedCamera(void) {
    return mCameraCache;
}

rp3d::RigidBody* GameObject::GetCachedRigidBody(void) {
    return mRigidBodyCache;
}

Entity* GameObject::GetCachedEntity(void) {
    return mEntityCache;
}

//
// Physics functions
//

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

void GameObject::SetStatic(void) {
    assert(mRigidBodyCache != nullptr);
    mRigidBodyCache->setType(rp3d::BodyType::STATIC);
    return;
}

void GameObject::SetDynamic(void) {
    assert(mRigidBodyCache != nullptr);
    mRigidBodyCache->setType(rp3d::BodyType::DYNAMIC);
    return;
}
