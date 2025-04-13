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
        //unsigned int numberOfCycles = numberOfChunks / 3;
        
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
    
    if (numberOfActors == 0) 
        return;
    
    GameObject* actorObject = actors[mActorIndex];
    Actor* actorPtr = actorObject->GetComponent<Actor>();
    
    if (actorObject->isActive) {
        
        HandleActorLifeCycle(actorObject);
        
        UpdateActorPosition(actorPtr, playerPosition);
        
        HandleActorInteraction(actorPtr, numberOfActors);
        
        HandleAutoBreeding(actorPtr, numberOfActors);
        
        CheckActorBreeding(actorPtr);
        
    }
    
    mActorIndex = (mActorIndex + 1) % numberOfActors;
    return;
}

void ChunkManager::HandleActorLifeCycle(GameObject* actorObject) {
    
    Actor* actorPtr = actorObject->GetComponent<Actor>();
    
    if (actorPtr->physical.GetAge() > actorPtr->physical.GetSeniorAge()) 
        actorPtr->biological.health -= 1.0f;
    
    if (actors.size() > 1000) 
        if (Random.Range(0, 100) > 40) 
            actorPtr->biological.health -= 100.0f;
    
    // Check actor death
    if (actorPtr->biological.health < 0) 
        KillActor(actorObject);
    
    return;
}

void ChunkManager::UpdateActorPosition(Actor* actorPtr, const glm::vec3 &playerPosition) {
    glm::vec3 actorPos = actorPtr->navigation.GetPosition();
    
    unsigned int breedingCoolDown = actorPtr->counters.GetCoolDownBreeding();
    if (breedingCoolDown != 0) 
        breedingCoolDown--;
    
    actorPtr->counters.SetCoolDownBreeding(breedingCoolDown);
    
    return;
}


// Check actor interaction

void ChunkManager::HandleActorInteraction(Actor* actorPtr, unsigned int numberOfActors) {
    
    GameObject* actorObjectB = actors[Random.Range(0, numberOfActors)];
    
    Actor* actorPtrA = actorPtr;
    Actor* actorPtrB = actorObjectB->GetComponent<Actor>();
    
    glm::vec3 posA = actorPtrA->navigation.GetPosition();
    glm::vec3 posB = actorPtrB->navigation.GetPosition();
    
    //if (glm::distance(posA, posB) > 40.0f) 
    //    continue;
    
    // Arbitrarily move towards actor
    
    // TODO Should select based on 'carnivore' flag
    
    if (actorPtrA->navigation.GetTargetActor() == nullptr || actorPtrB->navigation.GetTargetActor() == nullptr) 
        return;
    
    actorPtrA->navigation.SetTargetActor( actorPtrB->navigation.GetTargetActor() );
    actorPtrB->navigation.SetTargetActor( actorPtrA->navigation.GetTargetActor() );
    
    return;
}

// Breeding

void ChunkManager::HandleAutoBreeding(Actor* actorPtr, unsigned int numberOfActors) {
    
    if (world.doAutoBreeding && actors.size() < 2000 && actors.size() > 1) {
        
        if (mBreedingCoolDown > 0) 
            mBreedingCoolDown--;
        
        if (mBreedingCoolDown == 0) {
            
            mBreedingCoolDown = 20;
            
            for (unsigned int i=0; i < 8; i++) 
                AttemptBreeding(numberOfActors);
        }
    }
    
    return;
}

bool ChunkManager::AttemptBreeding(unsigned int numberOfActors) {
    GameObject* actorObjectA = actors[Random.Range(0, numberOfActors)];
    GameObject* actorObjectB = actors[Random.Range(0, numberOfActors)];
    
    Actor* actorA = actorObjectA->GetComponent<Actor>();
    Actor* actorB = actorObjectB->GetComponent<Actor>();
    
    // Check must be active
    if (!actorObjectA->isActive || !actorObjectB->isActive) 
        return false;
    
    // Check actors cannot be the same
    if (actorObjectA == actorObjectB) 
        return false;
    
    // Name must match
    if (actorA->GetName() != actorB->GetName()) 
        return false;
    
    // Check breeding cool down counter
    if (actorA->counters.GetCoolDownBreeding() > 0 || actorB->counters.GetCoolDownBreeding() > 0) 
        return false;
    
    // Check avoid homosexual pair
    if (actorA->physical.GetSexualOrientation() == actorB->physical.GetSexualOrientation()) 
        return false;
    
    glm::vec3 posA = actorObjectA->GetPosition();
    glm::vec3 posB = actorObjectB->GetPosition();
    
    // Check maximum distance
    if (glm::distance(posA, posB) > 80.0f) 
        return false;
    
    // Check age is adult
    if (actorA->physical.GetAge() < actorA->physical.GetAdultAge() || 
        actorB->physical.GetAge() < actorB->physical.GetAdultAge()) 
        return false;
    
    // Finally, select the pair for breeding
    actorA->navigation.SetBreedWithActor(actorB);
    actorB->navigation.SetBreedWithActor(actorA);
    
    // Allow some cool down time, allow extra cool down time
    // for females, pregnancy time out
    if (actorA->physical.GetSexualOrientation() == false) 
    {actorA->counters.SetCoolDownBreeding(400);} else 
    {actorA->counters.SetCoolDownBreeding(50);}
    
    if (actorB->physical.GetSexualOrientation() == false) 
    {actorB->counters.SetCoolDownBreeding(400);} else 
    {actorB->counters.SetCoolDownBreeding(50);}
    
    return true;
}


void ChunkManager::CheckActorBreeding(Actor* actorPtr) {
    
    Actor* breedTarget = actorPtr->navigation.GetBreedWithActor();
    
    if (breedTarget != nullptr) {
        glm::vec3 posA = actorPtr->navigation.GetPosition();
        glm::vec3 posB = breedTarget->navigation.GetPosition();
        
        if (glm::distance(posA, posB) < 1.5f) {
            
            glm::vec3 spawnPoint;
            
            // Spawn next to the female
            if (actorPtr->physical.GetSexualOrientation() == false) spawnPoint = posA;
            if (breedTarget->physical.GetSexualOrientation() == false) spawnPoint = posB;
            
            GameObject* newChild = SpawnActor(spawnPoint.x, spawnPoint.y, spawnPoint.z);
            Actor* newChildActor = newChild->GetComponent<Actor>();
            
            unsigned long int newAge = Random.Range(0, 100);
            
            newChildActor->physical.SetAge(newAge);
            
            newChildActor->SetName(actorPtr->GetName());
            AI.genomes.BlendGenomes(actorPtr, breedTarget, newChildActor);
            
            newChildActor->ReexpressPhenotype();
            
            actorPtr->navigation.SetBreedWithActor(nullptr);
            breedTarget->navigation.SetBreedWithActor(nullptr);
        }
    }
    return;
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

