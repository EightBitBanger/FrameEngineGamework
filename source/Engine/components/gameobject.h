#ifndef ENGINE_GAME_OBJECT
#define ENGINE_GAME_OBJECT

#include "../../Renderer/components/meshrenderer.h"
#include "../../Renderer/components/camera.h"
#include "../../Renderer/components/light.h"
#include "../../Physics/PhysicsSystem.h"
#include "../../Renderer/RenderSystem.h"

#include "../../Resources/assets/colliderTag.h"

#include "component.h"

extern RenderSystem Renderer;
extern EngineComponents Components;


class GameObject {
    
public:
    
    // Object elements
    
    /// Object name.
    std::string name;
    
    /// Object transform.
    Transform transform;
    
    /// State whether the object is active.
    bool isActive;
    
    /// Pointer to a parent object. nullptr represents the root parent object.
    GameObject* parent;
    
    GameObject();
    
    //
    // Components
    
    /// Add a component to the game object.
    void AddComponent(Component* component);
    /// Remove a component from the game object.
    bool RemoveComponent(Component* component);
    /// Get a component by its type.
    void* GetComponent(ComponentType type);
    /// Get a component by its index position.
    Component* GetComponentIndex(unsigned int index);
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
    
    /// Add a collider from a resource tag at the offset position x, y and z.
    void AddCollider(ColliderTag* colliderTag, float x, float y, float z);
    /// Add a box collider shape at the offset position x, y and z.
    void AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z);
    /// Add a capsule collider shape at the offset position x, y and z.
    void AddColliderCapsule(rp3d::CapsuleShape* capsuleShape, float x, float y, float z);
    /// Add a sphere collider shape at the offset position x, y and z.
    void AddColliderSphere(rp3d::SphereShape* sphereShape, float x, float y, float z);
    /// Add a mesh collider.
    void AddColliderMesh(MeshCollider* meshCollider);
    
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
    void SetStatic(void);
    /// Make the rigid body dynamically movable.
    void SetDynamic(void);
    
    /// Get the cached camera object pointer.
    Camera* GetCachedCamera(void);
    /// Get the cached rigid body object pointer.
    rp3d::RigidBody* GetCachedRigidBody(void);
    /// Get the cached mesh renderer object pointer.
    MeshRenderer* GetCachedMeshRenderer(void);
    /// Get the cached light object pointer.
    Light* GetCachedLight(void);
    
    
private:
    
    // List of attached components
    std::vector<Component*> mComponentList;
    
    // Cached component pointers, to avoid overhead from working with components internally
    Camera*          mCameraCache;
    rp3d::RigidBody* mRigidBodyCache;
    MeshRenderer*    mMeshRendererCache;
    Light*           mLightCache;
    
};

#endif
