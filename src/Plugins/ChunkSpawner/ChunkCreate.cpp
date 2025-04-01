#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

Chunk ChunkManager::CreateChunk(float x, float y) {
    
    Chunk chunk;
    
    chunk.x = x;
    chunk.y = y;
    
    chunk.gameObject = Engine.Create<GameObject>();
    chunk.staticObject = Engine.Create<GameObject>();
    
    chunk.gameObject->name = Float.ToString(x) + "_" + Float.ToString(y);
    
    chunk.gameObject->renderDistance   = renderDistance * chunkSize * 0.5f;
    chunk.staticObject->renderDistance = staticDistance * chunkSize * 0.45f;
    
    // Add renderers
    chunk.gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    chunk.staticObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    
    MeshRenderer* chunkRenderer = chunk.gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
    
    // Level of detail
    //chunkRenderer->distance = renderDistance * chunkSize * 0.18f;
    
    
    // Bounding box area
    //glm::vec3 boundMin(-chunkSize, -10, -chunkSize);
    //glm::vec3 boundMax(chunkSize, 10, chunkSize);
    
    //chunkRenderer->SetBoundingBoxMin(boundMin);
    //chunkRenderer->SetBoundingBoxMax(boundMax);
    
    //staticRenderer->SetBoundingBoxMin(boundMin);
    //staticRenderer->SetBoundingBoxMax(boundMax);
    
    // Chunk renderer
    
    Transform* chunkTransform = chunk.gameObject->GetComponent<Transform>();
    
    chunkTransform->position = glm::vec3( x, 0, y);
    chunkTransform->scale = glm::vec3( 1, 1, 1 );
    
    chunkRenderer->mesh = Engine.Create<Mesh>();
    chunkRenderer->mesh->isShared = false;
    
    chunkRenderer->EnableFrustumCulling();
    
    chunkRenderer->material = worldMaterial;
    
    // Level of detail
    //Mesh* lodMeshA = Engine.Create<Mesh>();
    //Mesh* lodMeshB = Engine.Create<Mesh>();
    
    //lodMeshA->isShared = false;
    //lodMeshB->isShared = false;
    
    //chunkRenderer->lods.push_back( lodMeshA );
    //chunkRenderer->lods.push_back( lodMeshB );
    
    
    // Static renderer
    
    Transform* staticTransform = chunk.staticObject->GetComponent<Transform>();
    
    staticTransform->position = glm::vec3( x, 0, y);
    staticTransform->scale = glm::vec3( 1, 1, 1 );
    
    staticRenderer->mesh = Engine.Create<Mesh>();
    staticRenderer->mesh->isShared = false;
    staticRenderer->EnableFrustumCulling();
    
    staticRenderer->material = staticMaterial;
    
    
    // Generate perlin
    int chunkSZ = chunkSize+1;
    
    float heightField [ chunkSZ * chunkSZ ];
    glm::vec3 colorField  [ chunkSZ * chunkSZ ];
    
    Engine.SetHeightFieldValues(heightField, chunkSZ, chunkSZ, 0);
    Engine.SetColorFieldValues(colorField, chunkSZ, chunkSZ, Colors.white);
    
    unsigned int numberOfLayers = perlin.size();
    
    for (unsigned int l=0; l < numberOfLayers; l++) {
        
        Perlin* perlinLayer = &perlin[l];
        
        //min = 
        Engine.AddHeightFieldFromPerlinNoise(heightField, chunkSZ, chunkSZ, 
                                            perlinLayer->noiseWidth, 
                                            perlinLayer->noiseHeight, 
                                            perlinLayer->heightMultuplier, 
                                            x, y, worldSeed);
        
        continue;
    }
    
    // Generate terrain color
    
    Color colorLow;
    Color colorHigh;
    
    colorLow  = Colors.brown * Colors.green * Colors.MakeGrayScale(0.4f);
    colorHigh = Colors.brown * Colors.MakeGrayScale(0.2f);
    
    Engine.GenerateColorFieldFromHeightField(colorField, heightField, chunkSZ, chunkSZ, colorLow, colorHigh, 0.024f);
    
    Engine.AddColorFieldSnowCap(colorField, heightField, chunkSZ, chunkSZ, Colors.white, 80.0f, 2.0f);
    
    Engine.GenerateWaterTableFromHeightField(heightField, chunkSZ, chunkSZ, 0);
    
    
    // Finalize chunk
    
    Engine.AddHeightFieldToMesh(chunkRenderer->mesh, heightField, colorField, chunkSZ, chunkSZ, 0, 0, 1, 1);
    chunkRenderer->mesh->Load();
    
    //Engine.AddHeightFieldToMeshHalfSize(chunkRenderer->lods[0], heightField, colorField, chunkSZ, chunkSZ, 0, 0);
    //Engine.AddHeightFieldToMeshHalfSize(chunkRenderer->lods[1], heightField, colorField, chunkSZ, chunkSZ, 0, 0);
    
    //Engine.AddHeightFieldToMeshLOD(chunkRenderer->lods[1], heightField, colorField, chunkSZ, chunkSZ, 0, 0, 2);
    //Engine.AddHeightFieldToMeshReduced(chunkRenderer->lods[1], heightField, colorField, chunkSZ, chunkSZ, 0, 0, 2);
    //Engine.AddHeightFieldToMeshReduced(chunkRenderer->lods[2], heightField, colorField, chunkSZ, chunkSZ, 0, 0, 20);
    
    //chunkRenderer->lods[0]->Load();
    //chunkRenderer->lods[1]->Load();
    //chunkRenderer->lods[2]->Load();
    
    
    // Physics
    
    chunk.rigidBody = Physics.world->createRigidBody( rp3d::Transform::identity() );
    
    chunk.rigidBody->setAngularLockAxisFactor( rp3d::Vector3(0, 0, 0) );
    chunk.rigidBody->setLinearLockAxisFactor( rp3d::Vector3(0, 0, 0) );
    chunk.rigidBody->setType(rp3d::BodyType::STATIC);
    
    rp3d::Transform bodyTransform = rp3d::Transform::identity();
    bodyTransform.setPosition( rp3d::Vector3(x, 0, y) );
    
    chunk.rigidBody->setTransform(bodyTransform);
    
    // Generate a height field collider
    
    MeshCollider*  meshCollider = Physics.CreateHeightFieldMap(heightField, chunkSZ, chunkSZ, 1, 1, 1);
    
    rp3d::Collider* bodyCollider = chunk.rigidBody->addCollider( meshCollider->heightFieldShape, rp3d::Transform::identity() );
    bodyCollider->setUserData( (void*)chunk.gameObject );
    bodyCollider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
    bodyCollider->setCollideWithMaskBits((unsigned short)CollisionMask::Entity);
    
    chunk.bodyCollider = bodyCollider;
    chunk.meshCollider = meshCollider;
    
    return chunk;
}

