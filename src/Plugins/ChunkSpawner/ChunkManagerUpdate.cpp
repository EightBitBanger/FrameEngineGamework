#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

void ChunkManager::Update(void) {
    
    if (Engine.cameraController == nullptr || !world.doGenerateChunks)
        return;
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    
    InitializePlayerHeight(playerPosition);
    
    UpdateFogSettings(playerPosition);
    
    UpdateActors(playerPosition);
    
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
            
            float chunkX = Math.Round(playerPosition.x / chunkSize + xx);
            float chunkZ = Math.Round(playerPosition.z / chunkSize + zz);
            
            glm::vec2 chunkPos((chunkX * chunkSize) - (renderDistance * (chunkSize / 2)), 
                               (chunkZ * chunkSize) - (renderDistance * (chunkSize / 2)));
            
            glm::vec2 playerPos(playerPosition.x, playerPosition.z);
            
            
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
                
                continue;
            }
            
            if (glm::distance(chunkPos, playerPos) > (renderDistance * (chunkSize / 2))) 
                continue;
            
            GenerateChunk(chunkPos);
        }
    }
}

bool ChunkManager::IsChunkFound(const glm::vec2 &chunkPosition) {
    for (const auto& chunkPtr : chunks) {
        if (glm::vec3(chunkPtr.x, 0, chunkPtr.y) == glm::vec3(chunkPosition.x, 0, chunkPosition.y)) {
            return true;
        }
    }
    return false;
}

void ChunkManager::GenerateChunk(const glm::vec2 &chunkPosition) {
    std::string filename = Int.ToString(chunkPosition.x) + "_" + Int.ToString(chunkPosition.y);
    std::string chunkFilename = "worlds/" + world.name + "/chunks/" + filename;
    std::string staticFilename = "worlds/" + world.name + "/static/" + filename;
    
    Chunk chunk = CreateChunk(chunkPosition.x, chunkPosition.y);
    MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
    
    if (Serializer.CheckExists(chunkFilename) || Serializer.CheckExists(staticFilename)) {
        
        LoadChunk(chunk);
        
    } else {
        
        chunk.seed = worldSeed + ((chunkPosition.x * 2) + (chunkPosition.y * 4) / 2);
        
        Random.SetSeed(chunk.seed);
        
        Decorate(chunk);
    }
    
    staticRenderer->mesh->Load();
    chunks.push_back(chunk);
}

void ChunkManager::DestroyChunks(const glm::vec3 &playerPosition) {
    
    unsigned int numberOfChunks = chunks.size();
    
    if (numberOfChunks > 0) {
        unsigned int numberOfCycles = numberOfChunks / 3;
        
        for (unsigned int c = 0; c < numberOfChunks; c++) {
            
            Chunk& chunk = chunks[mChunkIndex];
            
            if (chunk.gameObject == nullptr) 
                continue;
            
            glm::vec3 chunkPos(chunk.x, 0, chunk.y);
            glm::vec3 playerPos(playerPosition.x, 0, playerPosition.z);
            
            if (glm::distance(chunkPos, playerPos) > (renderDistance * chunkSize) * 1.5f) {
                
                SaveChunk(chunk, true);
                
                DestroyChunk(chunk);
                
                chunks.erase(chunks.begin() + mChunkIndex);
            }
            
            mChunkIndex++;
            if (mChunkIndex >= numberOfChunks) 
                mChunkIndex = 0;
            
            continue;
        }
    }
}

void ChunkManager::UpdateActors(const glm::vec3 &playerPosition) {
    unsigned int numberOfActors = actors.size();
    if (numberOfActors > 0) {
        GameObject* actorObject = actors[mActorIndex];
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        
        if (actorObject->isActive) {
            HandleActorLifeCycle(actorObject);
            UpdateActorPosition(actorPtr, playerPosition);
            HandleAutoBreeding(actorPtr, numberOfActors);
            CheckActorBreeding(actorPtr);
        }
        
        mActorIndex = (mActorIndex + 1) % numberOfActors;
    }
}

void ChunkManager::HandleActorLifeCycle(GameObject* actorObject) {
    Actor* actorPtr = actorObject->GetComponent<Actor>();
    
    if (numberOfActiveActors > 1000) {
        
        if (mDeathCoolDown > 0) 
            mDeathCoolDown--;
        
        if (mDeathCoolDown == 0 && actorPtr->GetAge() > 10000) {
            
            mDeathCoolDown = 80;
            
            KillActor(actorObject);
            
            return;
        }
    }
}

void ChunkManager::UpdateActorPosition(Actor* actorPtr, const glm::vec3 &playerPosition) {
    glm::vec3 actorPos = actorPtr->GetPosition();
    
    if (glm::distance(actorPos, playerPosition) < (renderDistance * chunkSize)) 
        actorPtr->SetGeneticExpressionFlag();
    
    unsigned int breedingCoolDown = actorPtr->GetCoolDownBreeding();
    if (breedingCoolDown != 0) {
        
        breedingCoolDown--;
        actorPtr->SetCoolDownBreeding(breedingCoolDown);
    }
    
    return;
}

void ChunkManager::HandleAutoBreeding(Actor* actorPtr, unsigned int numberOfActors) {
    
    if (world.doAutoBreeding && numberOfActiveActors < 1400 && numberOfActiveActors > 1) {
        
        if (mBreedingCoolDown != 0) 
            mBreedingCoolDown--;
        
        if (mBreedingCoolDown == 0) {
            
            mBreedingCoolDown = 400;
            
            AttemptBreeding(numberOfActors);
        }
    }
    
    return;
}

void ChunkManager::AttemptBreeding(unsigned int numberOfActors) {
    for (unsigned int i = 0; i < 8; i++) {
        GameObject* actorObjectA = actors[Random.Range(0, numberOfActors)];
        GameObject* actorObjectB = actors[Random.Range(0, numberOfActors)];
        
        if (!actorObjectA->isActive || !actorObjectB->isActive || actorObjectA == actorObjectB) 
            continue;
        
        Actor* actorA = actorObjectA->GetComponent<Actor>();
        Actor* actorB = actorObjectB->GetComponent<Actor>();
        
        if (actorA->GetCoolDownBreeding() != 0 || actorB->GetCoolDownBreeding() != 0) 
            continue;
        
        glm::vec3 posA = actorObjectA->GetPosition();
        glm::vec3 posB = actorObjectB->GetPosition();
        
        if (glm::distance(posA, posB) > 50.0f || actorA->GetAge() < 1000 || actorB->GetAge() < 1000) 
            continue;
        
        actorA->SetBreedWithActor(actorB);
        actorB->SetBreedWithActor(actorA);
        
        actorA->SetCoolDownBreeding(200);
        actorB->SetCoolDownBreeding(200);
        
        break;
    }
}


void ChunkManager::CheckActorBreeding(Actor* actorPtr) {
    Actor* breedTarget = actorPtr->GetBreedWithActor();
    if (breedTarget != nullptr) {
        glm::vec3 posA = actorPtr->GetPosition();
        glm::vec3 posB = breedTarget->GetPosition();
        
        if (glm::distance(posA, posB) < 2.0f) {
            glm::vec3 halfDistance = (posA + posB) * 0.5f;
            GameObject* newChild = SpawnActor(halfDistance.x, halfDistance.y, halfDistance.z);
            Actor* newChildActor = newChild->GetComponent<Actor>();
            
            newChildActor->SetName(actorPtr->GetName());
            AI.genomes.ConjugateGenome(actorPtr, breedTarget, newChildActor);
            
            newChildActor->SetGeneticUpdateFlag();
            newChildActor->SetGeneticExpressionFlag();
            
            actorPtr->SetBreedWithActor(nullptr);
            breedTarget->SetBreedWithActor(nullptr);
        }
    }
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







/*

void ChunkManager::Update(void) {
    
    if (Engine.cameraController == nullptr || !world.doGenerateChunks)
        return;
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    
    // New world, set player height
    if (!isInitiated) {
        
        playerPosition.y = 0;
        
        Hit hit;
        if (Physics.Raycast(playerPosition, glm::vec3(0, -1, 0), 100.0f, hit, LayerMask::Ground)) {
            
            playerPosition.y = hit.point.y + 3.5f;
            Engine.cameraController->SetPosition(playerPosition);
            isInitiated = true;
        }
    }
    
    // Check player underwater
    if (playerPosition.y < world.waterLevel) {
        // Underwater
        Renderer.fogHeightCutoff[RENDER_FOG_LAYER_3] = 1000.0f;
        Renderer.fogDensity[RENDER_FOG_LAYER_3] = 0.8f;
        Renderer.fogBegin[RENDER_FOG_LAYER_3] = 0.0f;
        Renderer.fogEnd[RENDER_FOG_LAYER_3] = 24.0f;
        Renderer.fogColorBegin[RENDER_FOG_LAYER_3] = Colors.blue;
        Renderer.fogColorEnd[RENDER_FOG_LAYER_3] = Colors.blue;
    } else {
        // Above water
        Renderer.fogHeightCutoff[RENDER_FOG_LAYER_3] = world.waterLevel;
        Renderer.fogDensity[RENDER_FOG_LAYER_3] = 0.8f;
        Renderer.fogBegin[RENDER_FOG_LAYER_3] = 0.0f;
        Renderer.fogEnd[RENDER_FOG_LAYER_3] = 1.0f;
        Renderer.fogColorBegin[RENDER_FOG_LAYER_3] = Colors.blue;
        Renderer.fogColorEnd[RENDER_FOG_LAYER_3] = Colors.blue;
    }
    
    // Update actors
    unsigned int numberOfActors = actors.size();
    
    if (numberOfActors > 0) {
        GameObject* actorObject = actors[mActorIndex];
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        
        if (actorObject->isActive) {
            if (numberOfActiveActors > 1000) {
                if (mDeathCoolDown > 0) 
                    mDeathCoolDown--;
                
                if (mDeathCoolDown == 0 && actorPtr->GetAge() > 10000) {
                    mDeathCoolDown = 300;
                    KillActor(actorObject);
                    return;
                }
            }
            
            glm::vec3 actorPos = actorPtr->GetPosition();
            if (glm::distance(actorPos, playerPosition) < (renderDistance * chunkSize)) 
                actorPtr->SetGeneticExpressionFlag();
            
            unsigned int breedingCoolDown = actorPtr->GetCoolDownBreeding();
            if (breedingCoolDown != 0) {
                breedingCoolDown--;
                actorPtr->SetCoolDownBreeding(breedingCoolDown);
            }
            
            // Auto breeding
            if (world.doAutoBreeding && numberOfActiveActors < 1400 && numberOfActiveActors > 1) {
                if (mBreedingCoolDown != 0) 
                    mBreedingCoolDown--;
                
                if (mBreedingCoolDown == 0) {
                    mBreedingCoolDown = 400;
                    
                    for (unsigned int i = 0; i < 8; i++) {
                        GameObject* actorObjectA = actors[Random.Range(0, numberOfActors)];
                        GameObject* actorObjectB = actors[Random.Range(0, numberOfActors)];
                        
                        if (!actorObjectA->isActive || !actorObjectB->isActive || actorObjectA == actorObjectB) 
                            continue;
                        
                        Actor* actorA = actorObjectA->GetComponent<Actor>();
                        Actor* actorB = actorObjectB->GetComponent<Actor>();
                        
                        if (actorA->GetCoolDownBreeding() != 0 || actorB->GetCoolDownBreeding() != 0) 
                            continue;
                        
                        glm::vec3 posA = actorObjectA->GetPosition();
                        glm::vec3 posB = actorObjectB->GetPosition();
                        
                        if (glm::distance(posA, posB) > 50.0f || actorA->GetAge() < 1000 || actorB->GetAge() < 1000) 
                            continue;
                        
                        actorA->SetBreedWithActor(actorB);
                        actorB->SetBreedWithActor(actorA);
                        
                        actorA->SetCoolDownBreeding(200);
                        actorB->SetCoolDownBreeding(200);
                        
                        break;
                    }
                }
            }
            
            // Check actor breeding
            Actor* breedTarget = actorPtr->GetBreedWithActor();
            if (breedTarget != nullptr) {
                glm::vec3 posA = actorPtr->GetPosition();
                glm::vec3 posB = breedTarget->GetPosition();
                
                if (glm::distance(posA, posB) < 2.0f) {
                    glm::vec3 halfDistance = (posA + posB) * 0.5f;
                    
                    GameObject* newChild = SpawnActor(halfDistance.x, halfDistance.y, halfDistance.z);
                    Actor* newChildActor = newChild->GetComponent<Actor>();
                    
                    newChildActor->SetName(actorPtr->GetName());
                    AI.genomes.ConjugateGenome(actorPtr, breedTarget, newChildActor);
                    
                    newChildActor->SetGeneticUpdateFlag();
                    newChildActor->SetGeneticExpressionFlag();
                    
                    actorPtr->SetBreedWithActor(nullptr);
                    breedTarget->SetBreedWithActor(nullptr);
                }
            }
        }
        
        mActorIndex++;
        if (mActorIndex >= numberOfActors) 
            mActorIndex = 0;
    }
    
    // Destroy chunks
    unsigned int numberOfChunks = chunks.size();
    if (numberOfChunks > 0) {
        unsigned int numberOfCycles = numberOfChunks / 3;
        
        for (unsigned int c = 0; c < numberOfCycles; c++) {
            Chunk& chunk = chunks[mChunkIndex];
            glm::vec3 chunkPos(chunk.x, 0, chunk.y);
            
            if (glm::distance(chunkPos, playerPosition) > (renderDistance * chunkSize) * 1.5f) {
                SaveChunk(chunk, true);
                DestroyChunk(chunk);
                chunks.erase(chunks.begin() + mChunkIndex);
            }
            
            mChunkIndex++;
            if (mChunkIndex >= numberOfChunks) 
                mChunkIndex = 0;
        }
    }
    
    // Generate chunks
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
            float chunkX = Math.Round(playerPosition.x / chunkSize + xx);
            float chunkZ = Math.Round(playerPosition.z / chunkSize + zz);
            
            glm::vec2 chunkPosition((chunkX * chunkSize) - (renderDistance * (chunkSize / 2)), 
                                    (chunkZ * chunkSize) - (renderDistance * (chunkSize / 2)));
            
            bool chunkFound = false;
            for (const auto& chunkPtr : chunks) {
                if (glm::vec3(chunkPtr.x, 0, chunkPtr.y) == glm::vec3(chunkPosition.x, 0, chunkPosition.y)) {
                    chunkFound = true;
                    break;
                }
            }
            
            if (chunkFound || glm::distance(glm::vec3(chunkPosition.x, 0, chunkPosition.y), playerPosition) > (renderDistance * (chunkSize / 2))) 
                continue;
            
            std::string filename = Int.ToString(chunkPosition.x) + "_" + Int.ToString(chunkPosition.y);
            std::string chunkFilename = "worlds/" + world.name + "/chunks/" + filename;
            std::string staticFilename = "worlds/" + world.name + "/static/" + filename;
            
            Chunk chunk = CreateChunk(chunkPosition.x, chunkPosition.y);
            MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
            
            if (Serializer.CheckExists(chunkFilename) || Serializer.CheckExists(staticFilename)) {
                LoadChunk(chunk);
            } else {
                chunk.seed = worldSeed + ((chunkPosition.x * 2) + (chunkPosition.y * 4) / 2);
                Random.SetSeed(chunk.seed);
                Decorate(chunk);
            }
            
            staticRenderer->mesh->Load();
            chunks.push_back(chunk);
        }
    }
    
    return;
}


*/

