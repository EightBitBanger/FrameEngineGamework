#include "entity.h"


Entity::Entity() {
    
    mMesh       = nullptr;
    mMaterial   = nullptr;
    
    return;
}

void Entity::AttachMesh(Mesh* meshPtr) {
    mMesh = meshPtr;
    return;
}

void Entity::DetachMesh(void) {
    mMesh = nullptr;
    return;
}

Mesh* Entity::GetAttachedMesh(void) {
    return mMesh;
}


void Entity::AttachMaterial(Material* materialPtr) {
    mMaterial = materialPtr;
    return;
}

void Entity::DetachMaterial(void) {
    mMaterial = nullptr;
    return;
}

Material* Entity::GetAttachedMaterial(void) {
    return mMaterial;
}
