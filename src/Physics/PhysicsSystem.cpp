#include <GameEngineFramework/Physics/PhysicsSystem.h>

PhysicsSystem::PhysicsSystem() : 
    world(nullptr)
{
    mRigidBodyFreeList.reserve(512);
}

RaybackCastCaller::RaybackCastCaller() : 
    isHit(false),
    point(glm::vec3(0, 0, 0))
{
}

void PhysicsSystem::Initiate(void) {
    rp3d::PhysicsWorld::WorldSettings worldSettings;
    worldSettings.defaultBounciness            = 0.5;
    worldSettings.defaultFrictionCoefficient   = 0.7;
    worldSettings.restitutionVelocityThreshold = 0.9;
    
    worldSettings.defaultVelocitySolverNbIterations = 15;
    worldSettings.defaultPositionSolverNbIterations = 8;
    
    worldSettings.isSleepingEnabled = true;
    worldSettings.gravity = rp3d::Vector3(0, 0, 0);
    
    world = common.createPhysicsWorld(worldSettings);
    
    //world->setSleepLinearVelocity(0.01);
    //world->setSleepAngularVelocity(0.01);
}

void PhysicsSystem::Shutdown(void) {
    for (unsigned int i=0; i < mRigidBodyFreeList.size(); i++) 
        world->destroyRigidBody( mRigidBodyFreeList[i] );
    common.destroyPhysicsWorld(world);
}

void PhysicsSystem::SetWorldGravity(float x, float y, float z) {
    world->setGravity(rp3d::Vector3(x, y, z));
}

rp3d::RigidBody* PhysicsSystem::CreateRigidBody(const glm::vec3& position) {
    rp3d::Vector3 pos = rp3d::Vector3(position.x, position.y, position.z);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform physicsTransform = rp3d::Transform(pos, orientation);
    return world->createRigidBody(physicsTransform);
}

bool PhysicsSystem::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    world->destroyRigidBody(rigidBodyPtr);
    return true;
}

rp3d::CollisionBody* PhysicsSystem::CreateCollisionBody(const glm::vec3& position) {
    rp3d::Vector3 pos = rp3d::Vector3(position.x, position.y, position.z);
    rp3d::Quaternion orientation = rp3d::Quaternion::identity();
    rp3d::Transform physicsTransform = rp3d::Transform(pos, orientation);
    return world->createCollisionBody(physicsTransform);
}

bool PhysicsSystem::DestroyCollisionBody(rp3d::CollisionBody* collisionBodyPtr) {
    world->destroyCollisionBody(collisionBodyPtr);
    return true;
}

rp3d::BoxShape* PhysicsSystem::CreateColliderBox(const glm::vec3& halfExtents) {
    rp3d::Vector3 scale(halfExtents.x, halfExtents.y, halfExtents.z);
    return common.createBoxShape(scale);
}

rp3d::SphereShape* PhysicsSystem::CreateColliderSphere(float radius) {
    return common.createSphereShape(radius);
}

rp3d::CapsuleShape* PhysicsSystem::CreateColliderCapsule(float radius, float height) {
    return common.createCapsuleShape(radius, height);
}

MeshCollider* PhysicsSystem::CreateHeightFieldMap(float* heightField, unsigned int width, unsigned int height, const glm::vec3& scale) {
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
    
    collider->heightFieldShape->setScale( rp3d::Vector3(scale.x, scale.y, scale.z) );
    return collider;
}

bool PhysicsSystem::DestroyHeightFieldMap(MeshCollider* collider) {
    common.destroyHeightFieldShape( collider->heightFieldShape );
    delete[] collider->heightMapBuffer;
    return mMeshColliders.Destroy( collider );
}

MeshCollider* PhysicsSystem::CreateConcaveMeshCollider(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    MeshCollider* collider = mMeshColliders.Create();
    if (collider == NULL) {
        return NULL;
    }
    
    const unsigned int vertexCount = static_cast<unsigned int>(vertices.size());
    const unsigned int indexCount  = static_cast<unsigned int>(indices.size());
    
    if (vertexCount == 0 || indexCount == 0) return nullptr;
    if (vertexCount % 3 != 0) return nullptr;
    if (indexCount % 3 != 0) return nullptr;
    
    const unsigned int nbVertices  = vertexCount / 3;
    const unsigned int nbTriangles = indexCount  / 3;
    
    collider->vertexBuffer = new float[vertexCount];
    std::memcpy(collider->vertexBuffer, vertices.data(), vertexCount * sizeof(float));
    
    collider->indexBuffer = new unsigned int[indexCount];
    std::memcpy(collider->indexBuffer, indices.data(), indexCount * sizeof(unsigned int));
    
    collider->triangleArray = new rp3d::TriangleVertexArray(
        static_cast<int>(nbVertices), collider->vertexBuffer, 3 * sizeof(float), 
        static_cast<int>(nbTriangles), collider->indexBuffer, 3 * sizeof(unsigned int), 
        rp3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
        rp3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
    );
    
    // Create TriangleMesh and ConcaveMeshShape
    collider->triangleMesh = common.createTriangleMesh();
    collider->triangleMesh->addSubpart(collider->triangleArray);
    
    collider->concaveShape = common.createConcaveMeshShape(collider->triangleMesh);
    
    return collider;
}

MeshCollider* PhysicsSystem::CreateConcaveMeshCollider(const SubMesh& submesh) {
    // Build flat position buffer x, y, z
    const unsigned int submeshVertexCount = static_cast<unsigned int>(submesh.vertexBuffer.size());
    std::vector<float> vertexPositions;
    vertexPositions.reserve(submeshVertexCount * 3);
    
    for (unsigned int i = 0; i < submeshVertexCount; ++i) {
        const Vertex& vertex = submesh.vertexBuffer[i];
        
        // If submesh.position is a local offset and you want it baked into the collider
        // vertexPositions.push_back(vertex.x + submesh.position.x);
        // vertexPositions.push_back(vertex.y + submesh.position.y);
        // vertexPositions.push_back(vertex.z + submesh.position.z);
        
        vertexPositions.push_back(vertex.x);
        vertexPositions.push_back(vertex.y);
        vertexPositions.push_back(vertex.z);
    }
    
    // Build index buffer
    const unsigned int submeshIndexCount = static_cast<unsigned int>(submesh.indexBuffer.size());
    std::vector<unsigned int> indices;
    indices.reserve(submeshIndexCount);
    
    for (unsigned int i = 0; i < submeshIndexCount; i++) 
        indices.push_back(submesh.indexBuffer[i].index);
    
    if (indices.size() % 3 != 0U) return nullptr;
    
    return CreateConcaveMeshCollider(vertexPositions, indices);
}

bool PhysicsSystem::DestroyConcaveMeshCollider(MeshCollider* collider) {
    common.destroyTriangleMesh( collider->triangleMesh );
    delete[] collider->vertexBuffer;
    delete[] collider->indexBuffer;
    return mMeshColliders.Destroy( collider );
}

void PhysicsSystem::AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr) {
    mRigidBodyFreeList.push_back(rigidBodyPtr);
    rigidBodyPtr->setIsActive(false);
}

rp3d::RigidBody* PhysicsSystem::RemoveRigidBodyFromFreeList(void) {
    if (mRigidBodyFreeList.size() == 0) 
        return nullptr;
    rp3d::RigidBody* bodyPtr = mRigidBodyFreeList[mRigidBodyFreeList.size()-1];
    bodyPtr->setIsActive(true);
    
    mRigidBodyFreeList.erase( mRigidBodyFreeList.end()-1 );
    return bodyPtr;
}

bool PhysicsSystem::Raycast(const glm::vec3& from, const glm::vec3& direction, float distance, Hit& hit, LayerMask layer) {
    glm::vec3 dir = glm::normalize( direction ) * distance;
    
    rp3d::Vector3 fromVec(from.x, from.y, from.z);
    rp3d::Vector3 toVec(dir.x, dir.y, dir.z);
    
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

rp3d::Vector3 PhysicsSystem::Normalize(rp3d::Vector3 value) {
    glm::vec3 vec(value.x, value.y, value.z);
    vec = glm::normalize(vec);
    return rp3d::Vector3(vec.x, vec.y, vec.z);
}

rp3d::BoxShape* PhysicsSystem::GetColliderBox(glm::vec3 extents) {
    for (unsigned int i=0; i < mBoxCollider.size(); i++) {
        rp3d::BoxShape* colliderShape = mBoxCollider[i];
        rp3d::Vector3 extentItem = colliderShape->getHalfExtents();
        if (extentItem.x == extents.x && 
            extentItem.y == extents.y && 
            extentItem.z == extents.z) 
            return colliderShape;
    }
    rp3d::Vector3 newExtent(extents.x, extents.y, extents.z);
    rp3d::BoxShape* colliderShape = common.createBoxShape(newExtent);
    
    mBoxCollider.push_back( colliderShape );
    return colliderShape;
}
