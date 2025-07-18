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
    
    world->setGravity(rp3d::Vector3(x, y, z));
    
    return;
}

rp3d::RigidBody* PhysicsSystem::CreateRigidBody(float x, float y, float z) {
    
    rp3d::Vector3 position = rp3d::Vector3(x, y, z);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform physicsTransform = rp3d::Transform(position, orientation);
    
    return world->createRigidBody(physicsTransform);
}

bool PhysicsSystem::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    
    world->destroyRigidBody(rigidBodyPtr);
    
    return true;
}

rp3d::CollisionBody* PhysicsSystem::CreateCollisionBody(float x, float y, float z) {
    
    rp3d::Vector3 position = rp3d::Vector3(x, y, z);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform physicsTransform = rp3d::Transform(position, orientation);
    
    return world->createCollisionBody(physicsTransform);
}

bool PhysicsSystem::DestroyCollisionBody(rp3d::CollisionBody* collisionBodyPtr) {
    
    world->destroyCollisionBody(collisionBodyPtr);
    
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
    
    float maximumHeight =  1000;
    float minimumHeight = -1000;
    
    collider->heightMapBuffer = new float[ mapSize ];
    
    memcpy(collider->heightMapBuffer, heightField, mapSize);
    
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

MeshCollider* PhysicsSystem::CreateTriangleMesh(float* vertices, unsigned int vertexCount, unsigned int* indices, unsigned int indexCount) {
    
    // TODO Physics triangle mesh collider
    
    MeshCollider* collider = mMeshColliders.Create();
    
    collider->vertexBuffer = new float[vertexCount * 3];
    collider->indexBuffer = new unsigned int[indexCount];
    
    for (unsigned int i = 0; i < vertexCount * 3; i++) {
        collider->vertexBuffer[i] = vertices[i];
    }
    
    for (unsigned int i = 0; i < indexCount; i++) {
        collider->indexBuffer[i] = indices[i];
    }
    
    collider->triangleMesh = common.createTriangleMesh();
    
    return collider;
}

bool PhysicsSystem::DestroyTriangleMesh(MeshCollider* collider) {
    
    common.destroyTriangleMesh( collider->triangleMesh );
    
    delete( collider->heightMapBuffer );
    
    return mMeshColliders.Destroy( collider );
}

void PhysicsSystem::AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr) {
    
    mRigidBodyFreeList.push_back(rigidBodyPtr);
    
    rigidBodyPtr->setIsActive(false);
    
    return;
}

rp3d::RigidBody* PhysicsSystem::RemoveRigidBodyFromFreeList(void) {
    
    if (mRigidBodyFreeList.size() == 0) 
        return nullptr;
    
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
    
    hit.userData  = mRaybackCastCaller.userData;
    hit.collider  = mRaybackCastCaller.collider;
    
    return true;
}



