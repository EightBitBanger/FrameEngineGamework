#ifndef _PHYSICS_WRAP_
#define _PHYSICS_WRAP_

#include <GameEngineFramework/MemoryAllocation/PoolAllocator.h>
#include <GameEngineFramework/Math/Math.h>

#include "../../../vendor/ReactPhysics3d/ReactPhysics3d.h"
#include <GameEngineFramework/Physics/components/meshcollider.h>
#include <GameEngineFramework/Renderer/components/submesh.h>

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
    rp3d::RigidBody* CreateRigidBody(const glm::vec3& position={0.0f,0.0f,0.0f});
    /// Destroy a rigid body object.
    bool DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    /// Create a collision body object and return its pointer.
    rp3d::CollisionBody* CreateCollisionBody(const glm::vec3& position={0.0f,0.0f,0.0f});
    /// Destroy a collision body object.
    bool DestroyCollisionBody(rp3d::CollisionBody* collisionBodyPtr);
    
    /// Set the gravity for the world simulation.
    void SetWorldGravity(float x, float y, float z);
    
    /// Create a box collider scaled by x, y, z.
    rp3d::BoxShape* CreateColliderBox(const glm::vec3& halfExtents);
    /// Create a sphere collider scaled by radius.
    rp3d::SphereShape* CreateColliderSphere(float radius);
    /// Create a capsule collider scaled by radius and height.
    rp3d::CapsuleShape* CreateColliderCapsule(float radius, float height);
    
    /// Get a collider from the collider list. If one does not exist it will be generated.
    rp3d::BoxShape* GetColliderBox(glm::vec3 extents);
    
    /// Create a height field collider from a height field map and return its pointer.
    MeshCollider* CreateHeightFieldMap(float* heightField, unsigned int width, unsigned int height, const glm::vec3& scale);
    /// Destroy a height field map collider.
    bool DestroyHeightFieldMap(MeshCollider* collider);
    
    /// Create a physics triangle mesh collider.
    MeshCollider* CreateConcaveMeshCollider(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    MeshCollider* CreateConcaveMeshCollider(const SubMesh& submesh);
    
    /// Destroy a physics triangle mesh collider.
    bool DestroyConcaveMeshCollider(MeshCollider* collider);
    
    /// Cast a ray and return the hit data of any objects that intersected the ray.
    bool Raycast(const glm::vec3& from, const glm::vec3& direction, float distance, Hit& hit, LayerMask layer=LayerMask::Default);
    
    /// Normalize an rp3d::vector3.
    rp3d::Vector3 Normalize(rp3d::Vector3 value);
    
private:
    
    // Add a rigid body to the free list
    void AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr);
    // Remove a rigid body from the free list
    rp3d::RigidBody* RemoveRigidBodyFromFreeList(void);
    
    // Free list of rigid bodies
    std::vector<rp3d::RigidBody*> mRigidBodyFreeList;
    
    // List of unique box colliders
    std::vector<rp3d::BoxShape*> mBoxCollider;
    
    // Callback function for physics ray casting
    RaybackCastCaller mRaybackCastCaller;
    
    // Allocator of mesh colliders
    PoolAllocator<MeshCollider> mMeshColliders;
    
};


#endif
