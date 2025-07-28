#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

Chunk ChunkManager::CreateChunk(float x, float y) {
    
    Chunk chunk;
    
    chunk.x = x;
    chunk.y = y;
    
    chunk.gameObject = Engine.Create<GameObject>();
    chunk.staticObject = Engine.Create<GameObject>();
    
    chunk.gameObject->name = Float.ToString(x) + "_" + Float.ToString(y);
    
    // Add renderers
    chunk.gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    chunk.staticObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    
    MeshRenderer* chunkRenderer = chunk.gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
    
    chunk.gameObject->renderDistance   = (renderDistance * chunkSize) * 0.5f;
    chunk.staticObject->renderDistance = (renderDistance * chunkSize) * 0.5f * staticDistance;
    
    // Bounding box area
    glm::vec3 boundMin(-1, -1, -1);
    glm::vec3 boundMax(1, 1, 1);
    
    chunkRenderer->SetBoundingBoxMin(boundMin);
    chunkRenderer->SetBoundingBoxMax(boundMax);
    
    staticRenderer->SetBoundingBoxMin(boundMin);
    staticRenderer->SetBoundingBoxMax(boundMax);
    
    // Chunk renderer
    
    Transform* chunkTransform = chunk.gameObject->GetComponent<Transform>();
    
    chunkTransform->SetPosition(x, 0, y);
    chunkTransform->scale = glm::vec3( 1, 1, 1 );
    
    chunkRenderer->mesh = Engine.Create<Mesh>();
    chunkRenderer->mesh->isShared = false;
    chunkRenderer->EnableFrustumCulling();
    chunkRenderer->material = worldMaterial;
    
    // Static renderer
    
    Transform* staticTransform = chunk.staticObject->GetComponent<Transform>();
    
    staticTransform->SetPosition( x, 0, y);
    staticTransform->scale = glm::vec3( 1, 1, 1 );
    
    staticRenderer->mesh = Engine.Create<Mesh>();
    staticRenderer->mesh->isShared = false;
    staticRenderer->EnableFrustumCulling();
    staticRenderer->material = staticMaterial;
    
    
    // Generate perlin
    int chunkSZ = chunkSize+1;
    
    float heightField [ chunkSZ * chunkSZ ];
    glm::vec3 colorField  [ chunkSZ * chunkSZ ];
    
    SetHeightFieldValues(heightField, chunkSZ, chunkSZ, 0);
    SetColorFieldValues(colorField, chunkSZ, chunkSZ, Colors.white);
    
    unsigned int numberOfLayers = world.mPerlin.size();
    
    for (unsigned int l=0; l < numberOfLayers; l++) {
        Perlin* perlinLayer = &world.mPerlin[l];
        
        AddHeightFieldFromPerlinNoise(heightField, chunkSZ, chunkSZ, 
                                      perlinLayer->noiseWidth, 
                                      perlinLayer->noiseHeight, 
                                      perlinLayer->heightMultuplier, 
                                      x + perlinLayer->offsetX, y + perlinLayer->offsetY, 
                                      perlinLayer->heightThreshold, worldSeed);
    }
    
    // Generate terrain color
    GenerateColorFieldFromHeightField(colorField, heightField, chunkSZ, chunkSZ, world.chunkColorLow, world.chunkColorHigh, world.chunkColorBias);
    
    
    // Example biome generation
    Color biomeColor;
    unsigned int numberOfBiomes = world.mBiomes.size();
    for (unsigned int b=0; b < numberOfBiomes; b++) {
        Biome* biomeLayer = &world.mBiomes[b];
        Color biomeColor;
        biomeColor = glm::vec3(biomeLayer->color.r, biomeLayer->color.g, biomeLayer->color.b);
        float offsetX = chunk.x + biomeLayer->offsetX;
        float offsetZ = chunk.y + biomeLayer->offsetZ;
        
        AddColorFieldFromPerlinNoise(colorField, chunkSZ, chunkSZ, biomeLayer->noiseWidth, biomeLayer->noiseHeight, biomeColor, offsetX, offsetZ);
    }
    
    /*
    // Desert
    float biomeNoise      = 0.001f;
    float biomeThreshold  = 0.3f;
    biomeColor = Colors.yellow * 0.3f;
    AddColorFieldFromPerlinNoise(colorField, chunkSZ, chunkSZ, biomeNoise, biomeNoise, biomeThreshold, Colors.dkgreen, biomeColor, chunk.x, chunk.y);
    
    // Tropical
    biomeNoise      = 0.003f;
    biomeThreshold  = 0.3f;
    biomeColor      = Colors.green * 0.7f;
    AddColorFieldFromPerlinNoise(colorField, chunkSZ, chunkSZ, biomeNoise, biomeNoise, biomeThreshold, Colors.dkgreen, biomeColor, chunk.x + 1000, chunk.y + 1000);
    */
    
    
    
    //AddColorFieldSnowCap(colorField, heightField, chunkSZ, chunkSZ, world.snowCapColor, world.snowCapHeight, world.snowCapBias);
    
    //GenerateWaterTableFromHeightField(heightField, chunkSZ, chunkSZ, world.waterLevel);
    
    AddHeightFieldToMesh(chunkRenderer->mesh, heightField, colorField, chunkSZ, chunkSZ, 0, 0, 1, 1);
    chunkRenderer->mesh->Load();
    
    // Physics
    
    chunk.rigidBody = Physics.world->createRigidBody( rp3d::Transform::identity() );
    
    chunk.rigidBody->setAngularLockAxisFactor( rp3d::Vector3(0, 0, 0) );
    chunk.rigidBody->setLinearLockAxisFactor( rp3d::Vector3(0, 0, 0) );
    chunk.rigidBody->setType(rp3d::BodyType::STATIC);
    
    rp3d::Transform bodyTransform = rp3d::Transform::identity();
    bodyTransform.setPosition( rp3d::Vector3(x, 0, y) );
    
    chunk.rigidBody->setTransform(bodyTransform);
    
    // Generate a height field collider
    
    MeshCollider* meshCollider = Physics.CreateHeightFieldMap(heightField, chunkSZ, chunkSZ, 1, 1, 1);
    
    rp3d::Collider* bodyCollider = chunk.rigidBody->addCollider( meshCollider->heightFieldShape, rp3d::Transform::identity() );
    bodyCollider->setUserData( (void*)chunk.gameObject );
    bodyCollider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
    bodyCollider->setCollideWithMaskBits((unsigned short)CollisionMask::Entity);
    
    chunk.bodyCollider = bodyCollider;
    chunk.meshCollider = meshCollider;
    
    return chunk;
}

