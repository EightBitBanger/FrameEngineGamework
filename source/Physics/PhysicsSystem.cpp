#include "PhysicsSystem.h"


PhysicsSystem::PhysicsSystem() : 
    
    world(nullptr)
{
    mRigidBodyFreeList.reserve(512);
    
    return;
}

void PhysicsSystem::Initiate(void) {
    assert(world == nullptr);
    rp3d::PhysicsWorld::WorldSettings worldSettings;
    worldSettings.defaultBounciness            = 0.5;
    worldSettings.defaultFrictionCoefficient   = 0.7;
    worldSettings.restitutionVelocityThreshold = 0.9;
    
    world = common.createPhysicsWorld(worldSettings);
    
    SetWorldGravity(0, -9.98 * 3 * 3, 0);
    
    world->enableSleeping(true);
    world->setSleepLinearVelocity(0.1);
    world->setSleepAngularVelocity(0.1);
    
    return;
}

void PhysicsSystem::Shutdown(void) {
    
    while (mRigidBodyFreeList.size() > 0) {
        DestroyRigidBody( RemoveRigidBodyFromFreeList() );
    }
    
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
    
    //rp3d::RigidBody* body = RemoveRigidBodyFromFreeList();
    //if (body != nullptr) {
    //    body->setTransform(physicsTransform);
    //    return body;
    //}
    
    return world->createRigidBody(physicsTransform);
}

void PhysicsSystem::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    assert(rigidBodyPtr != nullptr);
    
    // !! Warning: leak on rigid body destroy !!
    world->destroyRigidBody(rigidBodyPtr);
    
    // Use a free list instead...
    //AddRigidBodyToFreeList(rigidBodyPtr);
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

MeshCollider* PhysicsSystem::CreateColliderFromMesh(Mesh* sourceMesh) {
    
    MeshCollider* newMeshCollider = meshCollider.Create();
    
    unsigned int vertexCount = sourceMesh->GetNumberOfVertices();
    unsigned int indexCount  = sourceMesh->GetNumberOfIndices();
    
    for (int i=0; i < vertexCount; i++) {
        Vertex vertex = sourceMesh->GetVertex(i);
        newMeshCollider->vertexBuffer.push_back(vertex.x);
        newMeshCollider->vertexBuffer.push_back(vertex.y);
        newMeshCollider->vertexBuffer.push_back(vertex.z);
    }
    
    for (int i=0; i < indexCount; i++) {
        unsigned int index = sourceMesh->GetIndex(i).index;
        
        newMeshCollider->indexBuffer.push_back( index );
    }
    
    unsigned int sizeOfVertexBuffer = newMeshCollider->vertexBuffer.size() - 1;
    unsigned int sizeOfIndexBuffer  = newMeshCollider->indexBuffer.size() - 1;
    
    newMeshCollider->triangleArray = new rp3d::TriangleVertexArray(
                                                            sizeOfVertexBuffer,
                                                            newMeshCollider->vertexBuffer.data(),
                                                            3 * sizeof(float),
                                                            
                                                            sizeOfIndexBuffer,
                                                            newMeshCollider->indexBuffer.data(),
                                                            3 * sizeof (int),
                                                            
                                                            rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
                                                            rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
    
    
    
    newMeshCollider->triangleMesh = common.createTriangleMesh();
    newMeshCollider->triangleMesh->addSubpart( newMeshCollider->triangleArray );
    
    newMeshCollider->concaveMeshShape = common.createConcaveMeshShape( newMeshCollider->triangleMesh );
    
    return newMeshCollider;
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
