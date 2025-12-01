#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

Chunk* ChunkManager::CreateChunk(float x, float y) {
    Chunk* chunk = chunks.Create();
    chunk->isActive = false;
    
    chunk->x = x;
    chunk->y = y;
    
    chunk->gameObject = Engine.Create<GameObject>();
    chunk->staticObject = Engine.Create<GameObject>();
    
    chunk->gameObject->name = Float.ToString(x) + "_" + Float.ToString(y);
    
    // Add renderers
    chunk->gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    chunk->staticObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    
    MeshRenderer* chunkRenderer = chunk->gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk->staticObject->GetComponent<MeshRenderer>();
    
    chunk->gameObject->renderDistance   = (renderDistance * chunkSize) * 0.5f;
    chunk->staticObject->renderDistance = (renderDistance * chunkSize) * 0.5f * staticDistance;
    
    // Bounding box area
    glm::vec3 boundMin(-chunkSize, -100, -chunkSize);
    glm::vec3 boundMax(chunkSize, 100, chunkSize);
    
    chunkRenderer->SetBoundingBoxMin(boundMin);
    chunkRenderer->SetBoundingBoxMax(boundMax);
    
    staticRenderer->SetBoundingBoxMin(boundMin);
    staticRenderer->SetBoundingBoxMax(boundMax);
    
    // Chunk renderer
    
    Transform* chunkTransform = chunk->gameObject->GetComponent<Transform>();
    
    chunkTransform->SetPosition(x, 0, y);
    chunkTransform->scale = glm::vec3( 1, 1, 1 );
    
    chunkRenderer->mesh = Engine.Create<Mesh>();
    chunkRenderer->mesh->isShared = false;
    chunkRenderer->EnableFrustumCulling();
    chunkRenderer->material = worldMaterial;
    
    // Static renderer
    
    Transform* staticTransform = chunk->staticObject->GetComponent<Transform>();
    
    staticTransform->SetPosition( x, 0, y);
    staticTransform->scale = glm::vec3( 1, 1, 1 );
    
    staticRenderer->mesh = Engine.Create<Mesh>();
    staticRenderer->mesh->isShared = false;
    staticRenderer->EnableFrustumCulling();
    staticRenderer->material = staticMaterial;
    
    return chunk;
}


void ChunkManager::GenerateChunkBlendMasks(Chunk* chunk) {
    // Generate biome blend masks
    unsigned int chunkSZ = chunkSize + 1;
    unsigned int fieldSize = chunkSZ * chunkSZ;
    unsigned int numberOfBiomes = world.biomes.size();
    std::vector<std::vector<float>> biomeWeights(numberOfBiomes);
    std::vector<float> totalWeights(fieldSize, 0.0f);
    
    for (unsigned int b = 0; b < numberOfBiomes; b++) {
        Biome& biome = world.biomes[b];
        biomeWeights[b].resize(fieldSize);
        
        // Default to the first biome
        float baseBiome = 0.0f;
        if (b == 0)
            baseBiome = 0.1f;
        
        for (unsigned int i = 0; i < fieldSize; i++) {
            unsigned int x = i % chunkSZ;
            unsigned int z = i / chunkSZ;
            
            float xCoord = (x + chunk->x + biome.region.offsetX) * biome.region.noiseWidth;
            float zCoord = (z + chunk->y + biome.region.offsetZ) * biome.region.noiseHeight;
            
            float strength = Random.Perlin(xCoord, 0, zCoord, worldSeed) * 4.0f;
            
            strength = glm::clamp(strength, 0.0f, 1.0f);
            
            biomeWeights[b][i] = strength + baseBiome;
            totalWeights[i] += strength;
        }
        
        // Clamp for proper biome blending
        const float clampMin = 0.25f;
        for (unsigned int i = 0; i < fieldSize; i++)
            totalWeights[i] = glm::clamp(totalWeights[i], clampMin, 1.0f);
        
    }
    
    chunk->biomeWeights = biomeWeights;
    chunk->totalWeights = totalWeights;
    
    // Calculate dominant biome
    std::vector<int> dominantBiome(fieldSize, -1);
    for (unsigned int i = 0; i < fieldSize; i++) {
        float maxW = 0.0f;
        for (unsigned int b = 0; b < numberOfBiomes; b++) {
            float w = chunk->biomeWeights[b][i];
            if (w >= maxW) {
                maxW = w;
                dominantBiome[i] = b;
            }
            if (dominantBiome[i] < 0.0f)
                dominantBiome[i] = 0.0f;
        }
    }
    chunk->biomeMap = dominantBiome;
}


void ChunkManager::GenerateChunkBiomes(Chunk* chunk) {
    unsigned int chunkSZ = chunkSize + 1;
    unsigned int fieldSize = chunkSZ * chunkSZ;
    unsigned int numberOfBiomes = world.biomes.size();
    
    chunk->heightField = (float*)malloc(sizeof(float) * (chunkSZ * chunkSZ));
    chunk->colorField = (glm::vec3*)malloc(sizeof(glm::vec3) * (chunkSZ * chunkSZ));
    
    SetHeightFieldValues(chunk->heightField, chunkSZ, chunkSZ, 0);
    SetColorFieldValues(chunk->colorField, chunkSZ, chunkSZ, Colors.black, 0.01f);
    
    // Generate terrain base color
    
    if (numberOfBiomes > 0)
        for (unsigned int b=0; b < numberOfBiomes; b++)
            GenerateBiome(chunk->colorField, chunk->heightField, chunk, &world.biomes[b], chunk->biomeWeights[b].data(), chunk->totalWeights.data());
    
    chunk->biomeWeights.clear();
    chunk->totalWeights.clear();
}

