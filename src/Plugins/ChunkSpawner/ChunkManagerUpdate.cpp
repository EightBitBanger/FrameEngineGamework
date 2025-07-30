#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

void ChunkManager::Update(void) {
    if (Engine.cameraController == nullptr || !world.doGenerateChunks)
        return;
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    
    InitializePlayerHeight(playerPosition);
    UpdateFogSettings(playerPosition);
    DestroyChunks(playerPosition);
    GenerateChunks(playerPosition);
    
    return;
}

void ChunkManager::GenerateChunks(const glm::vec3 &playerPosition) {
    
    for (mChunkCounterX = 0; mChunkCounterX <= renderDistance; mChunkCounterX++) {
        
        if (mChunkCounterX >= renderDistance) {
            mChunkCounterX = 0;
            break;
        }
        
        int xx = mChunkCounterX;
        
        for (mChunkCounterZ = 0; mChunkCounterZ <= renderDistance; mChunkCounterZ++) {
            
            if (mChunkCounterZ >= renderDistance) {
                mChunkCounterZ = 0;
                break;
            }
            
            int zz = mChunkCounterZ;
            
            int chunkSizeSub = chunkSize;
            
            float chunkX = Math.Round((playerPosition.x + (chunkSizeSub / 2)) / chunkSizeSub + xx);
            float chunkZ = Math.Round((playerPosition.z + (chunkSizeSub / 2)) / chunkSizeSub + zz);
            
            glm::vec2 chunkPos((chunkX * chunkSizeSub) - (renderDistance * (chunkSizeSub / 2)), 
                               (chunkZ * chunkSizeSub) - (renderDistance * (chunkSizeSub / 2)));
            
            if (IsChunkFound(chunkPos)) {
                
                Chunk* chunk = FindChunk(chunkPos.x, chunkPos.y);
                
                // Check active fade in
                if (!chunk->isActive) {
                    
                    chunk->fadeIn += 1;
                    
                    if (chunk->fadeIn > 10) {
                        
                        chunk->isActive = true;
                        
                        MeshRenderer* chunkRenderer  = chunk->gameObject->GetComponent<MeshRenderer>();
                        MeshRenderer* staticRenderer = chunk->staticObject->GetComponent<MeshRenderer>();
                        
                        Engine.sceneMain->AddMeshRendererToSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
                        Engine.sceneMain->AddMeshRendererToSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
                        
                    }
                    
                }
                
                // Check if the chunk has finished being generated
                if (chunk->isGenerated && !chunk->isComplete) {
                    chunk->isComplete = true;
                    
                    int chunkSZ = chunkSize+1;
                    
                    Mesh* chunkMesh = chunk->gameObject->GetComponent<MeshRenderer>()->mesh;
                    AddHeightFieldToMesh(chunkMesh, chunk->heightField, chunk->colorField, chunkSZ, chunkSZ, 0, 0, 1, 1);
                    chunkMesh->Load();
                    
                    // Physics
                    
                    chunk->rigidBody = Physics.world->createRigidBody( rp3d::Transform::identity() );
                    
                    chunk->rigidBody->setAngularLockAxisFactor( rp3d::Vector3(0, 0, 0) );
                    chunk->rigidBody->setLinearLockAxisFactor( rp3d::Vector3(0, 0, 0) );
                    chunk->rigidBody->setType(rp3d::BodyType::STATIC);
                    
                    rp3d::Transform bodyTransform = rp3d::Transform::identity();
                    bodyTransform.setPosition( rp3d::Vector3(chunk->x, 0, chunk->y) );
                    
                    chunk->rigidBody->setTransform(bodyTransform);
                    
                    // Generate a height field collider
                    
                    MeshCollider* meshCollider = Physics.CreateHeightFieldMap(chunk->heightField, chunkSZ, chunkSZ, 1, 1, 1);
                    
                    rp3d::Collider* bodyCollider = chunk->rigidBody->addCollider( meshCollider->heightFieldShape, rp3d::Transform::identity() );
                    bodyCollider->setUserData( (void*)chunk->gameObject );
                    bodyCollider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
                    bodyCollider->setCollideWithMaskBits((unsigned short)CollisionMask::Entity);
                    
                    chunk->bodyCollider = bodyCollider;
                    chunk->meshCollider = meshCollider;
                    
                    
                    //
                    // Load / generate decorations / actors
                    
                    std::string filename = Int.ToString(chunkPos.x) + "_" + Int.ToString(chunkPos.y);
                    std::string chunkFilename = "worlds/" + world.name + "/chunks/" + filename;
                    std::string staticFilename = "worlds/" + world.name + "/static/" + filename;
                    
                    if (Serializer.CheckExists(chunkFilename) || Serializer.CheckExists(staticFilename)) {
                        LoadChunk(*chunk);
                        
                    } else {
                        chunk->seed = worldSeed + ((chunkPos.x * 2) + (chunkPos.y * 4) / 2);
                        
                        Random.SetSeed(chunk->seed);
                        Decorate(*chunk);
                    }
                    
                    free(chunk->heightField);
                    free(chunk->colorField);
                }
                
                continue;
            }
            
            glm::vec2 playerPos(playerPosition.x, playerPosition.z);
            if (glm::distance(chunkPos, playerPos) > (renderDistance * (chunkSizeSub / 2))) 
                continue;
            
            Chunk* chunk = CreateChunk(chunkPos.x, chunkPos.y);
            
            mux.lock();
            generating.push_back(chunk);
            mux.unlock();
            
        }
    }
    return;
}

void ChunkManager::DestroyChunks(const glm::vec3 &playerPosition) {
    unsigned int numberOfChunks = chunks.Size();
    if (numberOfChunks == 0) 
        return;
    
    for (unsigned int c = 0; c < numberOfChunks; c++) {
        Chunk& chunk = *chunks[mChunkIndex];
        if (chunk.gameObject == nullptr) 
            continue;
        
        glm::vec3 chunkPos(chunk.x, 0, chunk.y);
        glm::vec3 playerPos(playerPosition.x, 0, playerPosition.z);
        
        if (glm::distance(chunkPos, playerPos) > (renderDistance * chunkSize) * 1.5f) {
            SaveChunk(chunk, true);
            DestroyChunk(&chunk);
        }
        
        mChunkIndex++;
        if (mChunkIndex >= numberOfChunks) 
            mChunkIndex = 0;
    }
    return;
}

bool ChunkManager::IsChunkFound(const glm::vec2 &chunkPosition) {
    unsigned int numberOfChunks = chunks.Size();
    for (unsigned int c = 0; c < numberOfChunks; c++) {
        Chunk& chunk = *chunks[c];
        if (glm::vec3(chunk.x, 0, chunk.y) == glm::vec3(chunkPosition.x, 0, chunkPosition.y)) 
            return true;
    }
    return false;
}


void ChunkManager::UpdateFogSettings(const glm::vec3 &playerPosition) {
    if (playerPosition.y < world.waterLevel) {
        
        fogWater->fogHeightCutoff = 1000.0f;
        fogWater->fogDensity = 0.8f;
        fogWater->fogBegin = 0.0f;
        fogWater->fogEnd = 24.0f;
        fogWater->fogColorBegin = Colors.blue;
        fogWater->fogColorEnd = Colors.blue;
        
    } else {
        
        fogWater->fogHeightCutoff = world.waterLevel;
        fogWater->fogDensity = 0.8f;
        fogWater->fogBegin = 0.0f;
        fogWater->fogEnd = 1.0f;
        fogWater->fogColorBegin = Colors.blue;
        fogWater->fogColorEnd = Colors.blue;
        
    }
    return;
}


void ChunkManager::InitializePlayerHeight(glm::vec3 &playerPosition) {
    
    if (isInitiated) 
        return;
    
    playerPosition.y = 0;
    
    Hit hit;
    if (Physics.Raycast(playerPosition, glm::vec3(0, -1, 0), 100.0f, hit, LayerMask::Ground)) {
        playerPosition.y = hit.point.y + 3.5f;
        Engine.cameraController->SetPosition(playerPosition);
        isInitiated = true;
    }
    
    return;
}

