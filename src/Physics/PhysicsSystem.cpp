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
    
    worldSettings.isSleepingEnabled = true;
    worldSettings.gravity = rp3d::Vector3(0 , -9.81 , 0);
    
    world = common.createPhysicsWorld(worldSettings);
    
    world->setSleepLinearVelocity(0.01);
    world->setSleepAngularVelocity(0.01);
    
    world->setNbIterationsVelocitySolver(15);
    world->setNbIterationsPositionSolver(8);
    
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

bool PhysicsSystem::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    assert(rigidBodyPtr != nullptr);
    
    // !! Warning: leak on rigid body destroy !!
    world->destroyRigidBody(rigidBodyPtr);
    
    // Use a free list instead...
    //AddRigidBodyToFreeList(rigidBodyPtr);
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

MeshCollider* PhysicsSystem::CreateColliderFromMesh(Mesh* sourceMesh) {
    
    // Storage for the pointers and buffers associated with reactphysics3D
    MeshCollider* newMeshCollider = meshCollider.Create();
    
    unsigned int vertexCount = sourceMesh->GetNumberOfVertices();
    unsigned int indexCount  = sourceMesh->GetNumberOfIndices();
    
    
    for (unsigned int i=0; i < vertexCount; i++) {
        Vertex vertex = sourceMesh->GetVertex(i);
        newMeshCollider->vertexBuffer.push_back(vertex.x);
        newMeshCollider->vertexBuffer.push_back(vertex.y);
        newMeshCollider->vertexBuffer.push_back(vertex.z);
    }
    
    
    for (unsigned int i=0; i < indexCount; i++) {
        unsigned int index = sourceMesh->GetIndex(i).index;
        
        newMeshCollider->indexBuffer.push_back( index );
        
        //Vertex vertex = sourceMesh->GetVertex( index );
        
        //newMeshColliPhysicsSystem::der->vertexBuffer.push_back(vertex.x);
        //newMeshCollider->vertexBuffer.push_back(vertex.y);
        //newMeshCollider->vertexBuffer.push_back(vertex.z);
        
    }
    
    unsigned int sizeOfVertexBuffer = newMeshCollider->vertexBuffer.size() - 1;
    unsigned int sizeOfIndexBuffer  = newMeshCollider->indexBuffer.size() / 3;
    
    newMeshCollider->triangleArray = new rp3d::TriangleVertexArray(
                                                            sizeOfVertexBuffer,
                                                            newMeshCollider->vertexBuffer.data(),
                                                            3 * sizeof(float),
                                                            
                                                            sizeOfIndexBuffer,
                                                            newMeshCollider->indexBuffer.data(),
                                                            sizeof (int),
                                                            
                                                            rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
                                                            rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);
    
    
    
    newMeshCollider->triangleMesh = common.createTriangleMesh();
    newMeshCollider->triangleMesh->addSubpart( newMeshCollider->triangleArray );
    
    newMeshCollider->concaveMeshShape = common.createConcaveMeshShape( newMeshCollider->triangleMesh );
    
    return newMeshCollider;
}



MeshCollider* PhysicsSystem::CreateColliderHeightMapFromMesh(Mesh* sourceMesh) {
    
    MeshCollider* newMeshCollider = meshCollider.Create();
    
    //unsigned int vertexCount = sourceMesh->GetNumberOfVertices();
    //unsigned int indexCount  = sourceMesh->GetNumberOfIndices();
    
    for (int i=0; i < (10 * 10); i++) {
        Vertex vertex = sourceMesh->GetVertex(i);
        
        newMeshCollider->heightMapBuffer[i] = vertex.y;
    }
    
    
    newMeshCollider->heightFieldShape = 
    common.createHeightFieldShape(10,
                                  10,
                                  100,
                                  200,
                                  newMeshCollider->heightMapBuffer,
                                  rp3d::HeightFieldShape::HeightDataType::HEIGHT_FLOAT_TYPE);
    
    newMeshCollider->heightFieldShape->setScale(rp3d::Vector3(10, 10, 10));
    
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
