#include "gameobject.h"

GameObject::GameObject():

mesh(nullptr),
material(nullptr),
script(nullptr),
rigidBody(nullptr) {
    
    return;
}


void GameObject::AttachMesh(Mesh* meshPtr) {
    mesh = meshPtr;
    return;
}

void GameObject::DetachMesh(void) {
    mesh = nullptr;
    return;
}

Mesh* GameObject::GetAttachedMesh(void) {
    return mesh;
}


void GameObject::AttachMaterial(Material* materialPtr) {
    material = materialPtr;
    return;
}

void GameObject::DetachMaterial(void) {
    material = nullptr;
    return;
}

Material* GameObject::GetAttachedMaterial(void) {
    return material;
}


void GameObject::AttachScript(Script* scriptPtr) {
    script = scriptPtr;
    return;
}

void GameObject::DetachScript(void) {
    script = nullptr;
    return;
}

Script* GameObject::GetAttachedScript(void) {
    return script;
}


void GameObject::AttachRidigBody(rp3d::RigidBody* rigidBodyPtr){
    rigidBody = rigidBodyPtr;
    return;
}

void GameObject::DetachRidigBody(void) {
    rigidBody = nullptr;
    return;
}

rp3d::RigidBody* GameObject::GetAttachedRidigBody(){
    return rigidBody;
}

