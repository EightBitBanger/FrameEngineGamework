#ifndef PHYSICSFRAMEWORK
#define PHYSICSFRAMEWORK

#include <ReactPhysics3d/ReactPhysics3d.h>

struct PhysicsSystem {
    
    rp3d::PhysicsWorld* world;
    rp3d::PhysicsCommon common;
    
    PhysicsSystem();
    
    void Initiate(void);
    
    rp3d::RigidBody* CreateRigidBody(float x=0.0, float y=0.0, float z=0.0);
    void DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    void SetWorldGravity(float x, float y, float z);
    
    rp3d::BoxShape*     CreateColliderBox(float xscale, float yscale, float zscale);
    rp3d::SphereShape*  CreateColliderSphere(float radius);
    rp3d::CapsuleShape* CreateColliderCapsule(float radius, float height);
    
};


#endif
