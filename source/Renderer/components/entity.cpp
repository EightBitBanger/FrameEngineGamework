#include "entity.h"


Entity::Entity() {
    
    name = "";
    
    mesh       = nullptr;
    material   = nullptr;
    
    return;
}

Entity::~Entity() {
    
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

