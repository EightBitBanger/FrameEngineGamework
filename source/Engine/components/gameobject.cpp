#include "gameobject.h"


GameObject::GameObject() : 
    
    name(""),
    isActive(true),
    
    parent(nullptr),
    
    mCameraCache(nullptr),
    mRigidBodyCache(nullptr),
    mMeshRendererCache(nullptr),
    mLightCache(nullptr),
    mActorCache(nullptr),
    mTextCache(nullptr)
{
}


//
// Component functions
//

void GameObject::AddComponent(Component* component) {
    assert(component != nullptr);
    
    // Cache the component objects
    
    if (component->GetType() == Components.RigidBody) {
        mRigidBodyCache = (rp3d::RigidBody*)component->GetComponent();
    }
    
    if (component->GetType() == Components.MeshRenderer) {
        mMeshRendererCache = (MeshRenderer*)component->GetComponent();
    }
    
    if (component->GetType() == Components.Camera) {
        mCameraCache = (Camera*)component->GetComponent();
    }
    
    if (component->GetType() == Components.Light) {
        mLightCache = (Light*)component->GetComponent();
    }
    
    if (component->GetType() == Components.Actor) {
        mActorCache = (Actor*)component->GetComponent();
    }
    
    if (component->GetType() == Components.Text) {
        mTextCache = (Text*)component->GetComponent();
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
            if (component->GetType() == Components.MeshRenderer)  mMeshRendererCache = nullptr;
            if (component->GetType() == Components.RigidBody)     mRigidBodyCache = nullptr;
            if (component->GetType() == Components.Camera)        mCameraCache = nullptr;
            if (component->GetType() == Components.Light)         mLightCache = nullptr;
            if (component->GetType() == Components.Actor)         mActorCache = nullptr;
            if (component->GetType() == Components.Text)          mTextCache = nullptr;
            
            mComponentList.erase(it);
            return true;
        }
    }
    return false;
}

Component* GameObject::GetComponentIndex(unsigned int index) {
    assert(index < mComponentList.size());
    return mComponentList[index];
    
}

unsigned int GameObject::GetComponentCount(void) {
    return mComponentList.size();
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
        mLightCache->position = glm::vec3(x, y, z);
    }
    return;
}

void GameObject::SetPosition(glm::vec3 newPosition) {
    SetPosition(newPosition.x, newPosition.y, newPosition.z);
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

void GameObject::EnableGravity(void) {
    if (mRigidBodyCache == nullptr) return;
    mRigidBodyCache->enableGravity(true);
}

void GameObject::DisableGravity(void) {
    if (mRigidBodyCache == nullptr) return;
    mRigidBodyCache->enableGravity(false);
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
    
    if (meshCollider->heightFieldShape != nullptr) mRigidBodyCache->addCollider(meshCollider->heightFieldShape, offsetTransform);
    if (meshCollider->concaveMeshShape != nullptr) mRigidBodyCache->addCollider(meshCollider->concaveMeshShape, offsetTransform);
    
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
