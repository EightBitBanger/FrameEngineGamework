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

#include "../../Resources/assets/colliderTag.h"

extern PhysicsSystem Physics;


class Entity {
    
public:
    
    std::string name;
    
    Transform transform;
    
    /// Mesh component pointer.
    Mesh*     mesh;
    
    /// Material component pointer.
    Material* material;
    
    /// Script component pointer.
    Script*   script;
    
    /// Rigid body component pointer.
    rp3d::RigidBody* rigidBody;
    
    Entity();
    ~Entity();
    
    /// Add directional force to the associated rigid body.
    void AddForce(float x, float y, float z);
    /// Add rotational torque to the associated rigid body.
    void AddTorque(float x, float y, float z);
    
    /// Add a collider from a resource tag at the offset position x, y, z.
    void AddCollider(ColliderTag* colliderTag, float x, float y, float z);
    /// Add a box collider shape at the offset position x, y, z.
    void AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z);
    
    /// Set the rigid body mass.
    void SetMass(float mass);
    /// Set the physical resistance to movement.
    void SetLinearDamping(float damping);
    /// Set the physical resistance to rotation.
    void SetAngularDamping(float damping);
    
    /// Set the lock multiplier for linear movement.
    void SetLinearAxisLockFactor(float x, float y, float z);
    /// Set the lock multiplier for angular rotation.
    void SetAngularAxisLockFactor(float x, float y, float z);
    
    /// Set the gravity state.
    void EnableGravity(bool enabled);
    
    /// Calculate the physical weight proportions by the size and offset of colliders.
    void CalculatePhysics(void);
    
    /// Make the rigid body non movable.
    void SetRigidBodyStatic(void);
    /// Make the rigid body dynamicly movable.
    void SetRigidBodyDynamic(void);
    
};


#endif
