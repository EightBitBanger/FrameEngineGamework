#ifndef PHYSICSFRAMEWORK
#define PHYSICSFRAMEWORK

#include <ReactPhysics3d/ReactPhysics3d.h>

struct PhysicsSystem {
    
    rp3d::PhysicsWorld* world = nullptr;
    rp3d::PhysicsCommon common;
    
    PhysicsSystem();
    
    void Initiate(void);
    
    rp3d::RigidBody* CreateRigidBody(float x=0.0, float y=0.0, float z=0.0);
    void DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    rp3d::BoxShape* CreateBoxShape(float xscale, float yscale, float zscale);
    
    void SetWorldGravity(float x, float y, float z);
    
};


#endif
