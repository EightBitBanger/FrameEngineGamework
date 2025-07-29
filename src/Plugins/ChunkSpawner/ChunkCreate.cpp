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
    
    
    //
    // Generate biome blend masks
    
    unsigned int chunkSZ = chunkSize + 1;
    unsigned int fieldSize = chunkSZ * chunkSZ;
    unsigned int numberOfBiomes = world.mBiomes.size();
    std::vector<float> biomeWeights[numberOfBiomes];
    std::vector<float> totalWeights(fieldSize, 0.0f);
    
    for (unsigned int b = 0; b < numberOfBiomes; b++) {
        Biome& biome = world.mBiomes[b];
        biomeWeights[b].resize(fieldSize);
        
        for (unsigned int i = 0; i < fieldSize; i++) {
            unsigned int x = i % chunkSZ;
            unsigned int z = i / chunkSZ;
            
            float xCoord = (x + chunk.x + biome.offsetX) * biome.noiseWidth;
            float zCoord = (z + chunk.y + biome.offsetZ) * biome.noiseHeight;
            
            float strength = Random.Perlin(xCoord, 0, zCoord, worldSeed);
            strength = glm::clamp(strength, 0.0f, 1.0f);
            
            biomeWeights[b][i] = strength;
            totalWeights[i] += strength;
        }
        
    }
    
    // Calculate dominate biome for decoration generation
    std::vector<int> dominantBiome(fieldSize, -1);
    for (unsigned int i = 0; i < fieldSize; i++) {
        float maxW = 0.0f;
        for (unsigned int b = 0; b < numberOfBiomes; b++) {
            float w = biomeWeights[b][i];
            if (w > maxW) {
                maxW = w;
                dominantBiome[i] = b;
            }
        }
    }
    chunk.biomeMap = dominantBiome;
    
    // Zero out
    float heightField [ chunkSZ * chunkSZ ];
    glm::vec3 colorField  [ chunkSZ * chunkSZ ];
    SetHeightFieldValues(heightField, chunkSZ, chunkSZ, 0);
    SetColorFieldValues(colorField, chunkSZ, chunkSZ, Colors.white);
    
    // Generate terrain color
    GenerateColorFieldFromHeightField(colorField, heightField, chunkSZ, chunkSZ, world.chunkColorLow, world.chunkColorHigh, world.chunkColorBias);
    
    // Biome generation
    if (numberOfBiomes > 0) {
        Biome* biomeLayer=nullptr;
        
        for (unsigned int b=0; b < numberOfBiomes; b++) {
            Biome* biomeLayer = &world.mBiomes[b];
            
            Color biomeColor;
            biomeColor = glm::vec3(biomeLayer->color.r, biomeLayer->color.g, biomeLayer->color.b);
            
            float offsetX = chunk.x + biomeLayer->offsetX;
            float offsetZ = chunk.y + biomeLayer->offsetZ;
            
            GenerateBiome(colorField, heightField, &chunk, &world.mBiomes[b], biomeWeights[b].data(), totalWeights.data());
        }
    }
    
    for (unsigned int i = 0; i < fieldSize; i++) {
        if (totalWeights[i] < 0.01f) {
            float xCoord = ((i % chunkSZ) + x) * 0.05f;
            float zCoord = ((i / chunkSZ) + y) * 0.05f;
            
            float noise = Random.Perlin(xCoord, 0, zCoord, worldSeed) * 2.0f;
            
            heightField[i] += noise;
            
            Color fallback = Colors.Lerp(world.chunkColorLow, world.chunkColorHigh, glm::clamp(noise * 0.05f, 0.0f, 1.0f));
            colorField[i] = glm::vec3(fallback.r, fallback.g, fallback.b);
        }
    }
    
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

