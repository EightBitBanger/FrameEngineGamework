#ifndef PHYSICSFRAMEWORK
#define PHYSICSFRAMEWORK


struct PhysicsSystem {
    
    rp3d::PhysicsWorld* world = nullptr;
    rp3d::PhysicsCommon common;
    
    rp3d::RigidBody* CreateRigidBody(float x, float y, float z);
    void DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    PhysicsSystem();
    
    void SetGravity(float x, float y, float z);
    
};


#endif
