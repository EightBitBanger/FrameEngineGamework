#ifndef COMPONENT_ENTITY
#define COMPONENT_ENTITY

#define GLEW_STATIC
#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <ReactPhysics3d/ReactPhysics3d.h>

#include "../../std/timer.h"

#include "../types/bufferlayout.h"
#include "../types/transform.h"
#include "../types/color.h"
#include "../types/viewport.h"

#include "../../Input/InputSystem.h"
#include "../../Physics/PhysicsSystem.h"
#include "../../Resources/assets/colliderTag.h"
#include "../../Scripting/components/script.h"

#include "shader.h"
#include "material.h"
#include "mesh.h"


extern PhysicsSystem Physics;


class Entity {
    
public:
    
    std::string name;
    
    Transform transform;
    
    Entity();
    ~Entity();
    
    /// Attach a mesh object to the entity.
    void AttachMesh(Mesh* meshPtr);
    /// Attach the mesh object from the entity.
    void DetachMesh(void);
    /// Return the attached mesh object.
    Mesh* GetAttachedMesh(void);
    
    /// Attach a material object to the entity.
    void AttachMaterial(Material* materialPtr);
    /// Attach the material object from the entity.
    void DetachMaterial(void);
    /// Return the attached material object.
    Material* GetAttachedMaterial(void);
    
    /// Attach a script object to the entity.
    void AttachScript(Script* scriptPtr);
    /// Attach the script object from the entity.
    void DetachScript(void);
    /// Return the attached script object.
    Script* GetAttachedScript(void);
    
    /// Attach a rigid body object to the entity.
    void AttachRidigBody(rp3d::RigidBody* rigidBodyPtr);
    /// Attach the rigid body object from the entity.
    void DetachRidigBody(void);
    /// Return the attached rigid body object.
    rp3d::RigidBody* GetAttachedRidigBody();
    
    
    
    
    
    
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
    
private:
    
    /// Pointer to a mesh component.
    Mesh*     mesh;
    /// Pointer to a material component.
    Material* material;
    /// Pointer to a user script component.
    Script*   script;
    /// Pointer to a rigid body physics component.
    rp3d::RigidBody* rigidBody;
    
};


#endif
