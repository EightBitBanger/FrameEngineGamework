#ifndef PHYSICSFRAMEWORK
#define PHYSICSFRAMEWORK

#include <ReactPhysics3d/ReactPhysics3d.h>

struct PhysicsSystem {
    
    /// Physical world simulation.
    rp3d::PhysicsWorld* world;
    /// Physics engine common access singleton.
    rp3d::PhysicsCommon common;
    
    PhysicsSystem();
    
    /// Prepare the physics system.
    void Initiate(void);
    
    /// Create a rigid body object and return its pointer.
    rp3d::RigidBody* CreateRigidBody(float x=0.0, float y=0.0, float z=0.0);
    /// Destroy a rigid body object.
    void DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    /// Set the gravity for the world simulation.
    void SetWorldGravity(float x, float y, float z);
    
    /// Create a box collider scaled by xscale, yscale, zscale.
    rp3d::BoxShape*     CreateColliderBox(float xscale, float yscale, float zscale);
    /// Create a sphere collider scaled by radius.
    rp3d::SphereShape*  CreateColliderSphere(float radius);
    /// Create a capsule collider scaled by radius and height.
    rp3d::CapsuleShape* CreateColliderCapsule(float radius, float height);
    
};


#endif
