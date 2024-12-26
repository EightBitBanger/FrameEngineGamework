#ifndef _PHYSICS_WRAP_
#define _PHYSICS_WRAP_

#include <GameEngineFramework/MemoryAllocation/PoolAllocator.h>
#include <GameEngineFramework/Math/Math.h>

#include "../../../vendor/ReactPhysics3d/ReactPhysics3d.h"
#include <GameEngineFramework/Physics/components/meshcollider.h>

#include <GameEngineFramework/Physics/Raycast.h>
#include <GameEngineFramework/Physics/Masks.h>

#include <cstdlib>

typedef rp3d::RigidBody      RigidBody;
typedef rp3d::CollisionBody  CollisionBody;

typedef rp3d::BoxShape       BoxShape;
typedef rp3d::SphereShape    SphereShape;
typedef rp3d::CapsuleShape   CapsuleShape;


class ENGINE_API PhysicsSystem {
    
public:
    
    /// Physical world simulation.
    rp3d::PhysicsWorld* world;
    
    /// Physics library common access singleton.
    rp3d::PhysicsCommon common;
    
    PhysicsSystem();
    
    /// Prepare the physics system.
    void Initiate(void);
    
    /// Prepare the physics system.
    void Shutdown(void);
    
    /// Create a rigid body object and return its pointer.
    rp3d::RigidBody* CreateRigidBody(float x=0.0, float y=0.0, float z=0.0);
    /// Destroy a rigid body object.
    bool DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    /// Create a collision body object and return its pointer.
    rp3d::CollisionBody* CreateCollisionBody(float x=0.0, float y=0.0, float z=0.0);
    /// Destroy a collision body object.
    bool DestroyCollisionBody(rp3d::CollisionBody* collisionBodyPtr);
    
    /// Set the gravity for the world simulation.
    void SetWorldGravity(float x, float y, float z);
    
    /// Create a box collider scaled by x, y, z.
    rp3d::BoxShape* CreateColliderBox(float xscale, float yscale, float zscale);
    /// Create a sphere collider scaled by radius.
    rp3d::SphereShape* CreateColliderSphere(float radius);
    /// Create a capsule collider scaled by radius and height.
    rp3d::CapsuleShape* CreateColliderCapsule(float radius, float height);
    
    /// Create a height field collider from a height field map and return its pointer.
    MeshCollider* CreateHeightFieldMap(float* heightField, unsigned int width, unsigned int height, float scaleX=1, float scaleY=1, float scaleZ=1);
    
    /// Destroy a height field map collider.
    bool DestroyHeightFieldMap(MeshCollider* collider);
    
    /// Cast a ray and return the hit data of any objects that intersected the ray.
    bool Raycast(glm::vec3 from, glm::vec3 direction, float distance, Hit& hit, LayerMask layer=LayerMask::Default);
    
    
private:
    
    // Add a rigid body to the free list
    void AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr);
    // Remove a rigid body from the free list
    rp3d::RigidBody* RemoveRigidBodyFromFreeList(void);
    
    // Free list of rigid bodies
    std::vector<rp3d::RigidBody*> mRigidBodyFreeList;
    
    // Callback function for physics ray casting
    RaybackCastCaller mRaybackCastCaller;
    
    // Allocator of mesh colliders
    PoolAllocator<MeshCollider> mMeshColliders;
    
};


#endif
