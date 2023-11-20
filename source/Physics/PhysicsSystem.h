#ifndef _PHYSICS_WRAP_
#define _PHYSICS_WRAP_

#include <ReactPhysics3d/ReactPhysics3d.h>

#include "../../dependencies/CodeBaseLibrary/poolallocator.h"

#include "components/meshcollider.h"
#include "../Renderer/components/mesh.h"

typedef rp3d::RigidBody    RigidBody;
typedef rp3d::BoxShape     BoxShape;
typedef rp3d::SphereShape  SphereShape;
typedef rp3d::CapsuleShape CapsuleShape;


class __declspec(dllexport) PhysicsSystem {
    
public:
    
    PoolAllocator<MeshCollider> meshCollider;
    
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
    
    /// Set the gravity for the world simulation.
    void SetWorldGravity(float x, float y, float z);
    
    /// Create a box collider scaled by x, y, z.
    rp3d::BoxShape* CreateColliderBox(float xscale, float yscale, float zscale);
    /// Create a sphere collider scaled by radius.
    rp3d::SphereShape* CreateColliderSphere(float radius);
    /// Create a capsule collider scaled by radius and height.
    rp3d::CapsuleShape* CreateColliderCapsule(float radius, float height);
    /// Create a concave mesh shape collider.
    MeshCollider* CreateColliderFromMesh(Mesh* sourceMesh);
    /// Create a height map collider from a mesh grid of heights.
    MeshCollider* CreateColliderHeightMapFromMesh(Mesh* sourceMesh);
    
private:
    
    // Add a rigid body to the free list
    void AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr);
    // Remove a rigid body from the free list
    rp3d::RigidBody* RemoveRigidBodyFromFreeList(void);
    
    // Free list of rigid bodies
    std::vector<rp3d::RigidBody*> mRigidBodyFreeList;
    
};


#endif
