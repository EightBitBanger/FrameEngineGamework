#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
bool testFlip = false;

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
    glm::vec3 boundMin(-1, -1, -1);
    glm::vec3 boundMax(1, 1, 1);
    
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
    unsigned int numberOfBiomes = world.mBiomes.size();
    std::vector<std::vector<float>> biomeWeights(numberOfBiomes);
    std::vector<float> totalWeights(fieldSize, 0.0f);
    
    for (unsigned int b = 0; b < numberOfBiomes; b++) {
        Biome& biome = world.mBiomes[b];
        biomeWeights[b].resize(fieldSize);
        
        for (unsigned int i = 0; i < fieldSize; i++) {
            unsigned int x = i % chunkSZ;
            unsigned int z = i / chunkSZ;
            
            float xCoord = (x + chunk->x + biome.region.offsetX) * biome.region.noiseWidth;
            float zCoord = (z + chunk->y + biome.region.offsetZ) * biome.region.noiseHeight;
            
            float strength = Random.Perlin(xCoord, 0, zCoord, worldSeed);
            strength = glm::clamp(strength, 0.0f, 1.0f);
            
            biomeWeights[b][i] = strength;
            totalWeights[i] += strength;
        }
        
    }
    chunk->biomeWeights = biomeWeights;
    chunk->totalWeights = totalWeights;
    
    // Calculate dominant biome
    std::vector<int> dominantBiome(fieldSize, -1);
    for (unsigned int i = 0; i < fieldSize; i++) {
        float maxW = 0.0f;
        for (unsigned int b = 0; b < numberOfBiomes; b++) {
            float w = chunk->biomeWeights[b][i];
            if (w > maxW) {
                maxW = w;
                dominantBiome[i] = b;
            }
        }
    }
    chunk->biomeMap = dominantBiome;
}


void ChunkManager::GenerateChunkBiomes(Chunk* chunk) {
    unsigned int chunkSZ = chunkSize + 1;
    unsigned int fieldSize = chunkSZ * chunkSZ;
    unsigned int numberOfBiomes = world.mBiomes.size();
    
    chunk->heightField = (float*)malloc(sizeof(float) * (chunkSZ * chunkSZ));
    chunk->colorField = (glm::vec3*)malloc(sizeof(glm::vec3) * (chunkSZ * chunkSZ));
    
    SetHeightFieldValues(chunk->heightField, chunkSZ, chunkSZ, 0);
    SetColorFieldValues(chunk->colorField, chunkSZ, chunkSZ, Colors.red);
    
    // Generate terrain base color
    GenerateColorFieldFromHeightField(chunk->colorField, chunk->heightField, chunkSZ, chunkSZ, world.chunkColorLow, world.chunkColorHigh, world.chunkColorBias);
    
    if (numberOfBiomes > 0) 
        for (unsigned int b=0; b < numberOfBiomes; b++) 
            GenerateBiome(chunk->colorField, chunk->heightField, chunk, &world.mBiomes[b], chunk->biomeWeights[b].data(), chunk->totalWeights.data());
    
    // Fill in blank regions
    for (unsigned int i = 0; i < fieldSize; i++) {
        if (chunk->totalWeights[i] > 0.1f) 
            continue;
        
        float xCoord = ((i % chunkSZ) + chunk->x) * 0.09f;
        float zCoord = ((i / chunkSZ) + chunk->y) * 0.09f;
        
        float noise = Random.Perlin(xCoord, 0, zCoord, worldSeed) * 2.0f;
        
        chunk->heightField[i] += noise;
        
        //Color fallback = Colors.Lerp(world.chunkColorLow, world.chunkColorHigh, glm::clamp(noise * 0.05f, 0.0f, 1.0f));
        //chunk->colorField[i] = glm::vec3(fallback.r, fallback.g, fallback.b);
    }
    chunk->biomeWeights.clear();
    chunk->totalWeights.clear();
}

