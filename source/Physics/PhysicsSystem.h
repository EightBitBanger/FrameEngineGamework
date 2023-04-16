#ifndef PHYSICSFRAMEWORK
#define PHYSICSFRAMEWORK


struct PhysicsSystem {
    
    rp3d::PhysicsWorld* world = nullptr;
    rp3d::PhysicsCommon common;
    
    PhysicsSystem();
    
    rp3d::RigidBody* CreateRigidBody(float x, float y, float z);
    void DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    
    void SetWorldGravity(float x, float y, float z);
    
};


#endif
