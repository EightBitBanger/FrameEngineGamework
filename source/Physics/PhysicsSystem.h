#ifndef PHYSICSFRAMEWORK
#define PHYSICSFRAMEWORK

#include <ReactPhysics3d/ReactPhysics3d.h>

class PhysicsSystem {
    
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
    void DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    /// Set the gravity for the world simulation.
    void SetWorldGravity(float x, float y, float z);
    
    /// Create a box collider scaled by x, y, z.
    rp3d::BoxShape* CreateColliderBox(float xscale, float yscale, float zscale);
    /// Create a sphere collider scaled by radius.
    rp3d::SphereShape* CreateColliderSphere(float radius);
    /// Create a capsule collider scaled by radius and height.
    rp3d::CapsuleShape* CreateColliderCapsule(float radius, float height);
    
    
private:
    
    // Add a rigid body to the free list
    void AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr);
    // Remove a rigid body from the free list
    rp3d::RigidBody* RemoveRigidBodyFromFreeList(void);
    
    
    // Free list of rigid bodies
    std::vector<rp3d::RigidBody*> mRigidBodyFreeList;
    
    
};


#endif
