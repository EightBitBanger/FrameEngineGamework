#ifndef ENGINE_GAME_OBJECT
#define ENGINE_GAME_OBJECT

#include "../../Renderer/components/entity.h"
#include "../../Renderer/components/camera.h"
#include "../../Scripting/components/script.h"
#include "../../Physics/PhysicsSystem.h"

#include "../../Resources/assets/colliderTag.h"

#include "component.h"


class GameObject {
    
public:
    
    // Object elements
    
    /// Object name.
    std::string name;
    
    /// State whether the object is active.
    bool isActive;
    
    
    GameObject();
    
    //
    // Components
    
    /// Add a component to the game object.
    void AddComponent(Component* component);
    /// Remove a component from the game object.
    bool RemoveComponent(Component* component);
    /// Find a component by its type.
    Component* FindComponent(ComponentType type);
    
    /// Get a component by its index position.
    Component* GetComponent(unsigned int index);
    /// Get the number of components attached to the game object.
    unsigned int GetComponentCount(void);
    
    //
    // Physics
    
    /// Set the position of the object.
    void SetPosition(float x, float y, float z);
    
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
    
    /// Make the rigid body statically non movable.
    void SetRigidBodyStatic(void);
    /// Make the rigid body dynamically movable.
    void SetRigidBodyDynamic(void);
    
    
private:
    
    // List of attached components
    std::vector<Component*> mComponentList;
    
    // Cached component pointers, to avoid overhead from working with components internally
    rp3d::RigidBody* mRigidBodyCache;
    Entity*          mEntityCache;
    
};

#endif
