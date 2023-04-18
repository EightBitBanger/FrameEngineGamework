#ifndef COMPONENT_ENTITY
#define COMPONENT_ENTITY

#define GLEW_STATIC
#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ReactPhysics3d/ReactPhysics3d.h>

#include "../types/bufferlayout.h"
#include "../types/transform.h"
#include "../types/color.h"
#include "../types/viewport.h"

#include "../components/script.h"
#include "../components/shader.h"
#include "../components/material.h"
#include "../components/mesh.h"

#include "../../Input/InputSystem.h"
#include "../../std/timer.h"

#include "../../Physics/PhysicsSystem.h"

extern PhysicsSystem Physics;

struct Entity {
    
    Transform transform;
    
    Mesh*     mesh;
    Material* material;
    
    Script*   script;
    
    rp3d::RigidBody* rigidBody;
    
    Entity();
    ~Entity();
    
    void AddForce(float x, float y, float z);
    void AddTorque(float x, float y, float z);
    
    void AddCollider(rp3d::BoxShape* boxShape, float x, float y, float z);
    
    void SetMass(float mass);
    void SetLinearDamping(float damping);
    void SetAngularDamping(float damping);
    
    void SetLinearAxisLockFactor(float x, float y, float z);
    void SetAngularAxisLockFactor(float x, float y, float z);
    
    void EnableGravity(bool enabled);
    
    void CalculatePhysics(void);
    
    void SyncRigidBody(void);
    
};


#endif
