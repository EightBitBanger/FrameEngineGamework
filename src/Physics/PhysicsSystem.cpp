#include <GameEngineFramework/Physics/PhysicsSystem.h>

PhysicsSystem::PhysicsSystem() : 
    
    world(nullptr)
{
    mRigidBodyFreeList.reserve(512);
    
    return;
}

RaybackCastCaller::RaybackCastCaller() : 
    isHit(false),
    point(glm::vec3(0, 0, 0))
{
}

void PhysicsSystem::Initiate(void) {
    assert(world == nullptr);
    rp3d::PhysicsWorld::WorldSettings worldSettings;
    //worldSettings.defaultBounciness            = 0.5;
    //worldSettings.defaultFrictionCoefficient   = 0.7;
    //worldSettings.restitutionVelocityThreshold = 0.9;
    
    worldSettings.isSleepingEnabled = true;
    worldSettings.gravity = rp3d::Vector3(0, 0, 0);
    
    world = common.createPhysicsWorld(worldSettings);
    
    //world->setSleepLinearVelocity(0.01);
    //world->setSleepAngularVelocity(0.01);
    
    //world->setNbIterationsVelocitySolver(15);
    //world->setNbIterationsPositionSolver(8);
    
    return;
}

void PhysicsSystem::Shutdown(void) {
    
    for (unsigned int i=0; i < mRigidBodyFreeList.size(); i++) 
        world->destroyRigidBody( mRigidBodyFreeList[i] );
    
    common.destroyPhysicsWorld(world);
    
    return;
}

void PhysicsSystem::SetWorldGravity(float x, float y, float z) {
    assert(world != nullptr);
    world->setGravity(rp3d::Vector3(x, y, z));
    return;
}

rp3d::RigidBody* PhysicsSystem::CreateRigidBody(float x, float y, float z) {
    assert(world != nullptr);
    
    rp3d::Vector3 position = rp3d::Vector3(x, y, z);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform physicsTransform = rp3d::Transform(position, orientation);
    
    /*
    // Pull from a free list
    rp3d::RigidBody* body = RemoveRigidBodyFromFreeList();
    if (body != nullptr) {
        body->setTransform(physicsTransform);
        return body;
    }
    */
    
    return world->createRigidBody(physicsTransform);
}

bool PhysicsSystem::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    assert(rigidBodyPtr != nullptr);
    
    // Push into a free list...
    //AddRigidBodyToFreeList(rigidBodyPtr);
    
    world->destroyRigidBody(rigidBodyPtr);
    
    return true;
}

rp3d::BoxShape* PhysicsSystem::CreateColliderBox(float xscale, float yscale, float zscale) {
    rp3d::Vector3 scale(xscale, yscale, zscale);
    return common.createBoxShape(scale);
}

rp3d::SphereShape* PhysicsSystem::CreateColliderSphere(float radius) {
    return common.createSphereShape(radius);
}

rp3d::CapsuleShape* PhysicsSystem::CreateColliderCapsule(float radius, float height) {
    return common.createCapsuleShape(radius, height);
}

MeshCollider* PhysicsSystem::CreateHeightFieldMap(float* heightField, unsigned int width, unsigned int height, float scaleX, float scaleY, float scaleZ) {
    
    MeshCollider* collider = mMeshColliders.Create();
    
    unsigned int mapSize = width * height;
    
    int maximumHeight =  1000;
    int minimumHeight = -1000;
    
    collider->heightMapBuffer = new float[ mapSize ];
    
    for (unsigned int index=0; index < mapSize; index++) 
        collider->heightMapBuffer[index] = heightField[index];
    
    collider->heightFieldShape = common.createHeightFieldShape(width, height, 
                                                               minimumHeight, maximumHeight, 
                                                               collider->heightMapBuffer, 
                                                               rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);
    
    collider->heightFieldShape->setScale( rp3d::Vector3(scaleX, scaleY, scaleZ) );
    
    return collider;
}

bool PhysicsSystem::DestroyHeightFieldMap(MeshCollider* collider) {
    
    common.destroyHeightFieldShape( collider->heightFieldShape );
    
    delete( collider->heightMapBuffer );
    
    return mMeshColliders.Destroy( collider );
}


void PhysicsSystem::AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr) {
    
    mRigidBodyFreeList.push_back(rigidBodyPtr);
    
    rigidBodyPtr->setIsActive(false);
    
    return;
}

rp3d::RigidBody* PhysicsSystem::RemoveRigidBodyFromFreeList(void) {
    if (mRigidBodyFreeList.size() == 0) return nullptr;
    rp3d::RigidBody* bodyPtr = mRigidBodyFreeList[mRigidBodyFreeList.size()-1];
    bodyPtr->setIsActive(true);
    mRigidBodyFreeList.erase( mRigidBodyFreeList.end()-1 );
    return bodyPtr;
}

bool PhysicsSystem::Raycast(glm::vec3 from, glm::vec3 direction, float distance, Hit& hit, LayerMask layer) {
    
    direction = glm::normalize( direction ) * distance;
    
    rp3d::Vector3 fromVec(from.x, 
                          from.y, 
                          from.z);
    
    rp3d::Vector3 toVec(direction.x, 
                        direction.y, 
                        direction.z);
    
    rp3d::Ray ray(fromVec, fromVec + toVec);
    
    mRaybackCastCaller.isHit = false;
    
    world->raycast( ray, &mRaybackCastCaller, (unsigned short)layer );
    
    if (!mRaybackCastCaller.isHit) 
        return false;
    
    hit.point  = mRaybackCastCaller.point;
    hit.normal = mRaybackCastCaller.normal;
    
    hit.gameObject = mRaybackCastCaller.userData;
    
    return true;
}



