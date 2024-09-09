#ifndef ENGINE_GAME_OBJECT
#define ENGINE_GAME_OBJECT

#include <GameEngineFramework/Renderer/RenderSystem.h>
#include <GameEngineFramework/Scripting/ScriptSystem.h>
#include <GameEngineFramework/Physics/PhysicsSystem.h>
#include <GameEngineFramework/Resources/ResourceManager.h>
#include <GameEngineFramework/ActorAI/ActorSystem.h>

#include <GameEngineFramework/Engine/UI/panel.h>
#include <GameEngineFramework/Engine/UI/text.h>

#include <GameEngineFramework/Engine/components/component.h>

#include <type_traits>

extern RenderSystem Renderer;
extern EngineComponents Components;


class ENGINE_API GameObject {
    
public:
    
    friend class EngineSystemManager;
    
    /// Object name.
    std::string name;
    
    /// Should this game object be active in the scene.
    bool isActive;
    
    /// Distance within which this object will be updated.
    float renderDistance;
    
    GameObject();
    
    //
    // Components
    
    /// Add a component to the game object.
    void AddComponent(Component* component);
    /// Remove a component from the game object.
    bool RemoveComponent(Component* component);
    /// Get a component by its index position.
    Component* GetComponentIndex(unsigned int index);
    /// Get the number of components attached to the game object.
    unsigned int GetComponentCount(void);
    
    /// Get a component of a given type.
    template <class T> T* GetComponent(void) {
        
        if (std::is_same<T, Transform>::value)        return (T*)mTransformCache;
        if (std::is_same<T, Camera>::value)           return (T*)mCameraCache;
        if (std::is_same<T, Light>::value)            return (T*)mLightCache;
        if (std::is_same<T, MeshRenderer>::value)     return (T*)mMeshRendererCache;
        if (std::is_same<T, RigidBody>::value)        return (T*)mRigidBodyCache;
        if (std::is_same<T, Actor>::value)            return (T*)mActorCache;
        if (std::is_same<T, Text>::value)             return (T*)mTextCache;
        if (std::is_same<T, Panel>::value)            return (T*)mPanelCache;
        
        return nullptr;
    }
    
    /// Get a script component by name.
    template <class T> T* GetComponent(std::string scriptName) {
        for (std::vector<Component*>::iterator it = mComponentList.begin(); it != mComponentList.end(); ++it) {
            Component* currentComponentPtr = *it;
            
            if (currentComponentPtr->GetType() == Components.Script) {
                T* componentPtr = (T*)currentComponentPtr->GetComponent();
                
                if (componentPtr->name == scriptName) {
                    return componentPtr;
                }
            }
        }
        return nullptr;
    }
    
    
    //
    // Physics
    
    /// Get the position of the object.
    glm::vec3 GetPosition(void);
    
    /// Set the position of the object.
    void SetPosition(float x, float y, float z);
    
    /// Set the position of the object from a Vector3.
    void SetPosition(glm::vec3 newPosition);
    
    
    /// Activate the game object and any associated components.
    void Activate(void);
    
    /// Deactivate the game object and any associated components.
    void Deactivate(void);
    
    
    /// Add directional force to the rigid body.
    void AddForce(float x, float y, float z);
    
    /// Add rotational torque to the rigid body.
    void AddTorque(float x, float y, float z);
    
    /// Add directional force to the rigid body.
    void AddForce(glm::vec3 force);
    
    /// Add rotational torque to the rigid body.
    void AddTorque(glm::vec3 force);
    
    
    /// Add a box collider shape at the offset position x, y and z.
    void AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z, LayerMask layer = LayerMask::Default);
    
    /// Add a capsule collider shape at the offset position x, y and z.
    void AddColliderCapsule(rp3d::CapsuleShape* capsuleShape, float x, float y, float z, LayerMask layer = LayerMask::Default);
    
    /// Add a sphere collider shape at the offset position x, y and z.
    void AddColliderSphere(rp3d::SphereShape* sphereShape, float x, float y, float z, LayerMask layer = LayerMask::Default);
    
    
    /// Add a collider from a resource tag at the offset position x, y and z.
    void AddCollider(ColliderTag* colliderTag, float x, float y, float z, LayerMask layer = LayerMask::Default);
    
    /// Add a collider from a mesh collider at the offset position x, y and z.
    void AddCollider(MeshCollider* meshCollider, float x, float y, float z, LayerMask layer = LayerMask::Default);
    
    
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
    
    
    /// Enable gravity.
    void EnableGravity(void);
    
    /// Disable gravity.
    void DisableGravity(void);
    
    
    /// Calculate the physical weight proportions by the size and offset of attached colliders.
    void CalculatePhysics(void);
    
    
    /// Make the rigid body statically non movable.
    void SetStatic(void);
    
    /// Make the rigid body dynamically movable.
    void SetDynamic(void);
    
    /// Make the rigid body kinematic.
    void SetKinematic(void);
    
private:
    
    // Is this object marked for destruction by garbage collection.
    bool mIsGarbage;
    
    // List of attached components
    std::vector<Component*> mComponentList;
    
    // Cached component pointers, to avoid overhead from working with components internally
    Transform*       mTransformCache;
    Camera*          mCameraCache;
    RigidBody*       mRigidBodyCache;
    MeshRenderer*    mMeshRendererCache;
    Light*           mLightCache;
    Actor*           mActorCache;
    Text*            mTextCache;
    Panel*           mPanelCache;
    
};

#endif
