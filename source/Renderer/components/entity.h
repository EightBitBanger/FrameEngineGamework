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
    
    /// Pointer to a mesh component.
    Mesh*     mesh;
    
    /// Pointer to a material component.
    Material* material;
    
    /// Pointer to a user script component.
    Script*   script;
    
    /// Pointer to a rigid body physics component.
    rp3d::RigidBody* rigidBody;
    
    Entity();
    ~Entity();
    
    /// Add directional force to the rigid body.
    void AddForce(float x, float y, float z);
    /// Add rotational torque to the rigid body.
    void AddTorque(float x, float y, float z);
    
    /// Add a collider from a resource tag at the offset position x, y, z.
    void AddCollider(ColliderTag* colliderTag, float x, float y, float z);
    /// Add a box collider shape at the offset position x, y, z.
    void AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z);
    
    /// Set the rigid body mass.
    void SetMass(float mass);
    /// Set the rigid body physical resistance to movement.
    void SetLinearDamping(float damping);
    /// Set the rigid body physical resistance to rotation.
    void SetAngularDamping(float damping);
    
    /// Set the linear movement lock multiplier.
    void SetLinearAxisLockFactor(float x, float y, float z);
    /// Set the angular rotation lock multiplier.
    void SetAngularAxisLockFactor(float x, float y, float z);
    
    /// Set the gravity state.
    void EnableGravity(bool enabled);
    
    /// Calculate the physical weight proportions by the size and offset of attached colliders.
    void CalculatePhysics(void);
    
    /// Make the rigid body non movable.
    void SetRigidBodyStatic(void);
    /// Make the rigid body dynamically movable.
    void SetRigidBodyDynamic(void);
    
};


#endif
