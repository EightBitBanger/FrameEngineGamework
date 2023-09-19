#include "gameobject.h"


GameObject::GameObject() : 
    
    name(""),
    isActive(true),
    parent(nullptr),
    
    mCameraCache(nullptr),
    mRigidBodyCache(nullptr),
    mMeshRendererCache(nullptr),
    mLightCache(nullptr)
{
}


//
// Component functions
//

void GameObject::AddComponent(Component* component) {
    assert(component != nullptr);
    
    // Cache the component objects
    
    // Rigid body component
    if (component->GetType() == ComponentType::RigidBody) {
        mRigidBodyCache = (rp3d::RigidBody*)component->GetComponent();
    }
    
    // Mesh renderer renderer component
    if (component->GetType() == ComponentType::MeshRenderer) {
        mMeshRendererCache = (MeshRenderer*)component->GetComponent();
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
            if (component->GetType() == ComponentType::Light)         mLightCache = nullptr;
            if (component->GetType() == ComponentType::Camera)        mCameraCache = nullptr;
            if (component->GetType() == ComponentType::RigidBody)     mRigidBodyCache = nullptr;
            if (component->GetType() == ComponentType::MeshRenderer)  mMeshRendererCache = nullptr;
            
            mComponentList.erase(it);
            return true;
        }
    }
    return false;
}

Component* GameObject::GetComponent(ComponentType component_type) {
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

MeshRenderer* GameObject::GetCachedMeshRenderer(void) {
    return mMeshRendererCache;
}

Light* GameObject::GetCachedLight(void) {
    return mLightCache;
}


//
// Physics functions
//

void GameObject::SetPosition(float x, float y, float z) {
    transform.position = glm::vec3(x, y, z);
    if (mRigidBodyCache != nullptr) {
        rp3d::Vector3 position(x, y, z);
        rp3d::Transform bodyTransform = mRigidBodyCache->getTransform();
        bodyTransform.setPosition(position);
        mRigidBodyCache->setTransform(bodyTransform);
    }
    if (mMeshRendererCache != nullptr) {
        mMeshRendererCache->transform.position = glm::vec3(x, y, z);
    }
    if (mCameraCache != nullptr) {
        mCameraCache->transform.position = glm::vec3(x, y, z);
    }
    if (mLightCache != nullptr) {
        mLightCache->transform.position = glm::vec3(x, y, z);
    }
    return;
}

void GameObject::AddForce(float x, float y, float z) {
    if (mRigidBodyCache == nullptr) return;
    rp3d::Vector3 nudge(x, y, z);
    mRigidBodyCache->applyLocalForceAtCenterOfMass(nudge);
    return;
}

void GameObject::AddTorque(float x, float y, float z) {
    if (mRigidBodyCache == nullptr) return;
    rp3d::Vector3 twist(x, y, z);
    mRigidBodyCache->applyLocalTorque(twist);
    return;
}

void GameObject::SetMass(float mass) {
    if (mRigidBodyCache == nullptr) return;
    mRigidBodyCache->setMass(mass);
}

void GameObject::SetLinearDamping(float damping) {
    if (mRigidBodyCache == nullptr) return;
    mRigidBodyCache->setLinearDamping(damping);
}

void GameObject::SetAngularDamping(float damping) {
    if (mRigidBodyCache == nullptr) return;
    mRigidBodyCache->setAngularDamping(damping);
}

void GameObject::EnableGravity(bool enabled) {
    if (mRigidBodyCache == nullptr) return;
    mRigidBodyCache->enableGravity(enabled);
}

void GameObject::CalculatePhysics(void) {
    if (mRigidBodyCache == nullptr) return;
    mRigidBodyCache->updateMassFromColliders();
    mRigidBodyCache->updateLocalCenterOfMassFromColliders();
    mRigidBodyCache->updateLocalInertiaTensorFromColliders();
    return;
}

void GameObject::SetLinearAxisLockFactor(float x, float y, float z) {
    if (mRigidBodyCache == nullptr) return;
    rp3d::Vector3 lockFactor(x, y, z);
    mRigidBodyCache->setLinearLockAxisFactor(lockFactor);
    return;
}

void GameObject::SetAngularAxisLockFactor(float x, float y, float z) {
    if (mRigidBodyCache == nullptr) return;
    rp3d::Vector3 lockFactor(x, y, z);
    mRigidBodyCache->setAngularLockAxisFactor(lockFactor);
    return;
}

void GameObject::AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z) {
    if (mRigidBodyCache == nullptr) return;
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    mRigidBodyCache->addCollider(boxShape, offsetTransform);
    
    return;
}

void GameObject::AddColliderCapsule(rp3d::CapsuleShape* capsuleShape, float x, float y, float z) {
    if (mRigidBodyCache == nullptr) return;
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    mRigidBodyCache->addCollider(capsuleShape, offsetTransform);
    
    return;
}

void GameObject::AddColliderSphere(rp3d::SphereShape* sphereShape, float x, float y, float z) {
    if (mRigidBodyCache == nullptr) return;
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    mRigidBodyCache->addCollider(sphereShape, offsetTransform);
    
    return;
}

void GameObject::AddColliderMesh(MeshCollider* meshCollider) {
    if (mRigidBodyCache == nullptr) return;
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(0, 0, 0));
    
    mRigidBodyCache->addCollider(meshCollider->concaveMeshShape, offsetTransform);
    
    return;
}

void GameObject::AddCollider(ColliderTag* colliderTag, float x, float y, float z) {
    if (mRigidBodyCache == nullptr) return;
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
    if (mRigidBodyCache == nullptr) return;
    mRigidBodyCache->setType(rp3d::BodyType::STATIC);
    return;
}

void GameObject::SetDynamic(void) {
    if (mRigidBodyCache == nullptr) return;
    mRigidBodyCache->setType(rp3d::BodyType::DYNAMIC);
    return;
}
