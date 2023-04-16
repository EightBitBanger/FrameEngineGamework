#ifndef PHYSICSFRAMEWORK
#define PHYSICSFRAMEWORK


struct PhysicsSystem {
    
    rp3d::PhysicsWorld* world = nullptr;
    rp3d::PhysicsCommon common;
    
    PhysicsSystem();
    
    void Initiate(void);
    
    rp3d::RigidBody* CreateRigidBody(float x=0.0, float y=0.0, float z=0.0);
    void DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr);
    
    
    void SetWorldGravity(float x, float y, float z);
    
};


#endif
