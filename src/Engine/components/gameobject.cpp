#include <GameEngineFramework/Engine/components/gameobject.h>


GameObject::GameObject() : 
    name(""),
    isActive(true),
    isGarbage(false),
    renderDistance(-1),
    mUserData(nullptr),
    mComponentRegistry(nullptr)
{
    for (unsigned int i=0; i < EngineComponent::NumberOfComponents; i++) mComponents[i] = nullptr;
}


//
// Component functions

void GameObject::AddComponent(Component* component) {
    mComponentList.push_back(component);
    mComponents[component->GetType()] = component->GetComponent();
}

bool GameObject::RemoveComponent(Component* component) {
    for (std::vector<Component*>::iterator it = mComponentList.begin(); it != mComponentList.end(); ++it) {
        Component* thisComponentPtr = *it;
        
        if (component != thisComponentPtr) 
            continue;
        
        mComponentList.erase(it);
        mComponents[component->GetType()] = nullptr;
        return true;
    }
    return false;
}

Component* GameObject::GetComponentIndex(unsigned int index) {
    return mComponentList[index];
}

unsigned int GameObject::GetComponentCount(void) {
    return mComponentList.size();
}



//
// Physics functions

glm::vec3 GameObject::GetPosition(void) {
    Transform* transformCache = (Transform*)mComponents[EngineComponent::Transform];
    if (transformCache == nullptr) return glm::vec3(0);
    return transformCache->position;
}

void GameObject::SetPosition(float x, float y, float z) {
    Transform* transformCache = (Transform*)mComponents[EngineComponent::Transform];
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (transformCache != nullptr) 
        transformCache->position = glm::vec3(x, y, z);
    if (rigidBodyCache != nullptr) {
        rp3d::Vector3 position(x, y, z);
        rp3d::Transform bodyTransform = rigidBodyCache->getTransform();
        bodyTransform.setPosition(position);
        rigidBodyCache->setTransform(bodyTransform);
    }
    
    MeshRenderer* meshRendererCache = (MeshRenderer*)mComponents[EngineComponent::MeshRenderer];
    if (meshRendererCache != nullptr) meshRendererCache->transform.position = glm::vec3(x, y, z);
}

void GameObject::SetPosition(glm::vec3 newPosition) {
    SetPosition(newPosition.x, newPosition.y, newPosition.z);
}

void GameObject::Activate(void) {
    isActive = true;
    for (unsigned int i=0; i < mComponentList.size(); i++) {
        Component* componentPtr = mComponentList[i];
        ComponentType type = componentPtr->GetType();
        
        if (type == EngineComponent::Script) {
            Script* script = (Script*)componentPtr->GetComponent();
            script->isActive = true;
        }
        if (type == EngineComponent::RigidBody) {
            RigidBody* rigidBody = (RigidBody*)componentPtr->GetComponent();
            rigidBody->setIsActive(true);
        }
        if (type == EngineComponent::MeshRenderer) {
            MeshRenderer* meshRenderer = (MeshRenderer*)componentPtr->GetComponent();
            meshRenderer->isActive = true;
        }
        if (type == EngineComponent::Light) {
            Light* light = (Light*)componentPtr->GetComponent();
            light->isActive = true;
        }
        if (type == EngineComponent::Sound) {
            Sound* sound = (Sound*)componentPtr->GetComponent();
            sound->isActive = true;
        }
        
    }
}

void GameObject::Deactivate(void) {
    
    isActive = false;
    
    for (unsigned int i=0; i < mComponentList.size(); i++) {
        
        Component* componentPtr = mComponentList[i];
        
        ComponentType type = componentPtr->GetType();
        
        if (type == EngineComponent::Script) {
            Script* script = (Script*)componentPtr->GetComponent();
            script->isActive = false;
        }
        if (type == EngineComponent::RigidBody) {
            RigidBody* rigidBody = (RigidBody*)componentPtr->GetComponent();
            rigidBody->setIsActive(false);
        }
        if (type == EngineComponent::MeshRenderer) {
            MeshRenderer* meshRenderer = (MeshRenderer*)componentPtr->GetComponent();
            meshRenderer->isActive = false;
        }
        if (type == EngineComponent::Light) {
            Light* light = (Light*)componentPtr->GetComponent();
            light->isActive = false;
        }
        if (type == EngineComponent::Sound) {
            Sound* sound = (Sound*)componentPtr->GetComponent();
            sound->isActive = false;
        }
        continue;
    }
}

void GameObject::AddForce(float x, float y, float z) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rp3d::Vector3 nudge(x, y, z);
    rigidBodyCache->applyLocalForceAtCenterOfMass(nudge);
}

void GameObject::AddForce(glm::vec3 force) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rp3d::Vector3 nudge(force.x, force.y, force.z);
    rigidBodyCache->applyLocalForceAtCenterOfMass(nudge);
}

void GameObject::AddTorque(float x, float y, float z) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rp3d::Vector3 twist(x, y, z);
    rigidBodyCache->applyLocalTorque(twist);
}

void GameObject::AddTorque(glm::vec3 force) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rp3d::Vector3 twist(force.x, force.y, force.z);
    rigidBodyCache->applyLocalTorque(twist);
}

void GameObject::SetMass(float mass) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rigidBodyCache->setMass(mass);
}

void GameObject::SetLinearDamping(float damping) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rigidBodyCache->setLinearDamping(damping);
}

void GameObject::SetAngularDamping(float damping) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rigidBodyCache->setAngularDamping(damping);
}

void GameObject::EnableGravity(void) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rigidBodyCache->enableGravity(true);
}

void GameObject::DisableGravity(void) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rigidBodyCache->enableGravity(false);
}

void GameObject::CalculatePhysics(void) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rigidBodyCache->updateMassFromColliders();
    rigidBodyCache->updateLocalCenterOfMassFromColliders();
    rigidBodyCache->updateLocalInertiaTensorFromColliders();
}

void GameObject::SetLinearAxisLockFactor(float x, float y, float z) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBodyCache->setLinearLockAxisFactor(lockFactor);
}

void GameObject::SetAngularAxisLockFactor(float x, float y, float z) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBodyCache->setAngularLockAxisFactor(lockFactor);
}

void GameObject::AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z, LayerMask layer) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    rp3d::Collider* collider = rigidBodyCache->addCollider(boxShape, offsetTransform);
    collider->setCollideWithMaskBits( (unsigned short)layer );
}

void GameObject::AddColliderCapsule(rp3d::CapsuleShape* capsuleShape, float x, float y, float z, LayerMask layer) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    rp3d::Collider* collider = rigidBodyCache->addCollider(capsuleShape, offsetTransform);
    collider->setCollideWithMaskBits( (unsigned short)layer );
}

void GameObject::AddColliderSphere(rp3d::SphereShape* sphereShape, float x, float y, float z, LayerMask layer) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rp3d::Collider* collider = rigidBodyCache->addCollider(sphereShape, offsetTransform);
    collider->setCollideWithMaskBits( (unsigned short)layer );
}

void GameObject::AddCollider(ColliderTag* colliderTag, float x, float y, float z, LayerMask layer) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    if (colliderTag->isStatic) {
        rigidBodyCache->setType(rp3d::BodyType::STATIC);
    } else {
        rigidBodyCache->setType(rp3d::BodyType::DYNAMIC);
    }
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rp3d::Collider* collider = rigidBodyCache->addCollider(colliderTag->colliderShape, offsetTransform);
    collider->setCollideWithMaskBits( (unsigned short)layer );
}

void GameObject::AddCollider(MeshCollider* meshCollider, float x, float y, float z, LayerMask layer) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    rp3d::Collider* collider = rigidBodyCache->addCollider( meshCollider->heightFieldShape, offsetTransform );
    collider->setCollideWithMaskBits( (unsigned short)layer );
}

void GameObject::SetStatic(void) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rigidBodyCache->setType(rp3d::BodyType::STATIC);
}

void GameObject::SetDynamic(void) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rigidBodyCache->setType(rp3d::BodyType::DYNAMIC);
}

void GameObject::SetKinematic(void) {
    rp3d::RigidBody* rigidBodyCache = (rp3d::RigidBody*)mComponents[EngineComponent::RigidBody];
    if (rigidBodyCache == nullptr) return;
    rigidBodyCache->setType(rp3d::BodyType::KINEMATIC);
}

void GameObject::SetUserData(void* ptr){
    mUserData = ptr;
}

void* GameObject::GetUserData(void) {
    return mUserData;
}
