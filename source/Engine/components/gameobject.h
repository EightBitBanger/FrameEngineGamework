#ifndef ENGINE_GAME_OBJECT
#define ENGINE_GAME_OBJECT

#include "../../Renderer/components/entity.h"


class GameObject {
    
public:
    
    bool isActive;
    
    
    GameObject();
    
    /// Attach an entity object to the game object.
    void AttachEntity(Entity* scriptPtr);
    /// Detach the material object from the game object.
    void DetachEntity(void);
    /// Return the attached material object.
    Entity* GetAttachedEntity(void);
    
    /// Attach a script object to the game object.
    void AttachScript(Script* scriptPtr);
    /// Detach the script object from the game object.
    void DetachScript(void);
    /// Return the attached script object.
    Script* GetAttachedScript(void);
    
    /// Attach a rigid body object to the game object.
    void AttachRidigBody(rp3d::RigidBody* rigidBodyPtr);
    /// Detach the rigid body object from the game object.
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
    
    /// Pointer to an entity component.
    Entity*          entity;
    /// Pointer to a user script component.
    Script*          script;
    /// Pointer to a rigid body physics component.
    rp3d::RigidBody* rigidBody;
    
};

#endif
