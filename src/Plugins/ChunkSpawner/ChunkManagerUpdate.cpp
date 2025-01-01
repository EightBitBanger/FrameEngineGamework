#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>


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



void DecodeGenome(DecorationSpecifier& decor, Actor* actorPtr) {
    
    if (decor.name == "Sheep")  {AI.genomes.presets.Sheep( actorPtr );}
    if (decor.name == "Bear")   {AI.genomes.presets.Bear( actorPtr );}
    
    return;
}


void ChunkManager::AddDecorGrass(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz) {
    
    staticObject.type = DECORATION_GRASS;
    
    staticMesh->AddSubMesh(xx, yy, zz, subMeshGrassHorz, false);
    staticMesh->AddSubMesh(xx, yy, zz, subMeshGrassVert, false);
    
    unsigned int index = staticMesh->GetSubMeshCount() - 1;
    
    Color finalColor;
    
    finalColor.r = staticObject.r;
    finalColor.g = staticObject.g;
    finalColor.b = staticObject.b;
    
    staticMesh->ChangeSubMeshColor(index, finalColor);
    staticMesh->ChangeSubMeshColor(index-1, finalColor);
    
    // TODO Make static objects destructible
    
    //staticObject.collisionBody = Physics.CreateCollisionBody(staticObject.x, staticObject.y, staticObject.z);
    //rp3d::Collider* collider = staticObject.rigidBody->addCollider(Engine.colliders.box, transform);
    
    //rp3d::Transform transform;
    //staticObject.rigidBody->setType(rp3d::BodyType::STATIC);
    
    //collider->setCollisionCategoryBits((unsigned short)LayerMask::Object); // Ray cast as an "Object"
    //collider->setCollideWithMaskBits((unsigned short)LayerMask::Ground);   // Collide as a solid object
    
    chunk.statics.push_back(staticObject);
    
    return;
}



void ChunkManager::AddDecorGrassThin(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz) {
    
    StaticObject newStaticObject = staticObject;
    
    newStaticObject.type = DECORATION_GRASS_THIN;
    newStaticObject.y = yy;
    
    staticMesh->AddSubMesh(xx, yy, zz, subMeshStemHorz, false);
    staticMesh->AddSubMesh(xx, yy, zz, subMeshStemVert, false);
    
    unsigned int index = staticMesh->GetSubMeshCount() - 1;
    
    Color finalColor;
    
    finalColor.r = newStaticObject.r;
    finalColor.g = newStaticObject.g;
    finalColor.b = newStaticObject.b;
    
    staticMesh->ChangeSubMeshColor(index, finalColor);
    staticMesh->ChangeSubMeshColor(index-1, finalColor);
    
    chunk.statics.push_back(newStaticObject);
    
    return;
}

void ChunkManager::AddDecorGrassThick(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz) {
    
    StaticObject newStaticObject = staticObject;
    
    newStaticObject.type = DECORATION_GRASS_THICK;
    newStaticObject.y = yy;
    
    staticMesh->AddSubMesh(xx, yy, zz, subMeshWallHorz, false);
    staticMesh->AddSubMesh(xx, yy, zz, subMeshWallVert, false);
    
    unsigned int index = staticMesh->GetSubMeshCount() - 1;
    
    Color finalColor;
    
    finalColor.r = newStaticObject.r;
    finalColor.g = newStaticObject.g;
    finalColor.b = newStaticObject.b;
    
    if (Random.Range(0, 100) < 20) finalColor = Colors.yellow * 0.05f;
    if (Random.Range(0, 100) < 20) finalColor = Colors.orange * 0.01f;
    
    staticMesh->ChangeSubMeshColor(index, finalColor);
    staticMesh->ChangeSubMeshColor(index-1, finalColor);
    
    newStaticObject.r = finalColor.r;
    newStaticObject.g = finalColor.g;
    newStaticObject.b = finalColor.b;
    
    chunk.statics.push_back(newStaticObject);
    
    return;
}



void ChunkManager::AddDecorTreeLogs(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz) {
    
    StaticObject newStaticObject = staticObject;
    
    newStaticObject.type = DECORATION_TREE;
    newStaticObject.y = yy - 1.0f;
    
    staticMesh->AddSubMesh(xx, yy - 1.0f, zz, subMeshTree, false);
    
    unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
    
    unsigned int index = numberOfSubMeshes - 1;
    
    
    Color finalColor;
    
    finalColor.r = newStaticObject.r;
    finalColor.g = newStaticObject.g;
    finalColor.b = newStaticObject.b;
    
    staticMesh->ChangeSubMeshColor(index, finalColor);
    
    chunk.statics.push_back(newStaticObject);
    
    return;
}

void ChunkManager::AddDecorTreeLeaves(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz) {
    
    unsigned int leafAccent = Random.Range(0, 100);
    
    StaticObject newStaticObject = staticObject;
    newStaticObject.type = DECORATION_LEAVES;
    
    staticMesh->AddSubMesh(xx, world.leafHeightOffset + yy, zz, subMeshWallHorz, false);
    staticMesh->AddSubMesh(xx, world.leafHeightOffset + yy, zz, subMeshWallVert, false);
    
    unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
    unsigned int index = numberOfSubMeshes - 1;
    
    Color finalColor;
    finalColor.r = newStaticObject.r;
    finalColor.g = newStaticObject.g;
    finalColor.b = newStaticObject.b;
    
    staticMesh->ChangeSubMeshColor(index, finalColor);
    staticMesh->ChangeSubMeshColor(index-1, finalColor);
    
    chunk.statics.push_back(newStaticObject);
    
    return;
}


void ChunkManager::AddDecorTree(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz, Decoration treeType) {
    
    unsigned int leafCount   = Random.Range(10, 14);
    unsigned int logHeight   = Random.Range(6, 8);
    unsigned int leafAccent  = Random.Range(0, 100);
    
    float leafSpreadArea     = world.leafSpreadArea;
    float leafSpreadHeight   = world.leafSpreadHeight;
    
    if (treeType == Decoration::TreeOak) {
        
        leafCount   = Random.Range(10, 15);
        logHeight   = Random.Range(6, 8);
        leafAccent  = 0;
        
        leafSpreadArea     = 3.0f;
        leafSpreadHeight   = 2.0f;
        
    }
    
    // Tree logs
    
    for (unsigned int s=0; s < logHeight; s++) {
        
        Color lowTrunk;
        Color highTrunk;
        Color finalColor;
        
        lowTrunk  = (Colors.brown * 0.13f) + (Colors.green * 0.03);
        highTrunk = (Colors.brown * 0.87f) + (Colors.green * 0.0087);
        
        finalColor = Colors.Lerp(lowTrunk, highTrunk, 0.087f + (s * 0.087f));
        
        staticObject.r = finalColor.r;
        staticObject.g = finalColor.g;
        staticObject.b = finalColor.b;
        
        AddDecorTreeLogs(chunk, staticObject, staticMesh, xx, yy + s, zz);
    }
    
    // Leaves
    
    for (unsigned int s=0; s < leafCount; s++) {
        
        float offset_xx = Random.Range(0.0f, leafSpreadArea)   - Random.Range(0.0f, leafSpreadArea);
        float offset_yy = Random.Range(0.0f, leafSpreadHeight) - Random.Range(0.0f, leafSpreadHeight);
        float offset_zz = Random.Range(0.0f, leafSpreadArea)   - Random.Range(0.0f, leafSpreadArea);
        
        StaticObject newStaticObject = staticObject;
        newStaticObject.x += offset_xx;
        newStaticObject.y += offset_yy + logHeight + world.leafHeightOffset;
        newStaticObject.z += offset_zz;
        
        Color finalColor;
        Color lowLeaves;
        Color highLeaves;
        
        lowLeaves  = Colors.green * 0.08f;
        highLeaves = Colors.green * 0.01f;
        
        if ((leafAccent > 70) & (leafAccent < 80)) 
            lowLeaves = Colors.orange * 0.1f;
        
        if ((leafAccent > 20) & (leafAccent < 40)) 
            lowLeaves = Colors.yellow * 0.1f;
        
        finalColor = Colors.Lerp(lowLeaves, highLeaves, Random.Range(0, 100) * 0.01f);
        
        newStaticObject.r = finalColor.r;
        newStaticObject.g = finalColor.g;
        newStaticObject.b = finalColor.b;
        
        AddDecorTreeLeaves(chunk, newStaticObject, staticMesh, xx + offset_xx, yy + logHeight + world.leafHeightOffset + offset_yy, zz + offset_zz);
        
        continue;
    }
    
    return;
}




void ChunkManager::Decorate(Chunk& chunk) {
    
    if (world.mDecorations.size() == 0) 
        return;
    
    Mesh* staticMesh = chunk.staticObject->GetComponent<MeshRenderer>()->mesh;
    
    chunk.statics.clear();
    
    staticMesh->ClearSubMeshes();
    
    for (int xx=0; xx < chunkSize-1; xx++) {
        
        for (int zz=0; zz < chunkSize-1; zz++) {
            
            float xp = xx - (chunkSize / 2);
            float zp = zz - (chunkSize / 2);
            
            float staticX = (chunk.x - xp);
            float staticZ = (chunk.y - zp);
            
            glm::vec3 from(staticX, 0, staticZ);
            glm::vec3 direction(0, -1, 0);
            
            // Pick a random decoration for this world
            unsigned int decorIndex = Random.Range(0, world.mDecorations.size());
            
            DecorationSpecifier decor = world.mDecorations[ decorIndex ];
            
            // Perlin generation
            float xCoord = (float)xx * decor.noise;
            float zCoord = (float)zz * decor.noise;
            
            if (Random.Perlin(xCoord, 0, zCoord, chunk.seed) < decor.threshold) 
                continue;
            
            Hit hit;
            
            float distance = 2000.0f;
            float height = 0.0f;
            
            if (Physics.Raycast(from, direction, distance, hit, LayerMask::Ground)) 
                height = hit.point.y;
            
            if (height > world.staticHeightCutoff) 
                continue;
            
            if (height == 0.0f) 
                continue;
            
            // World spawn height range
            if ((height > decor.spawnHeightMaximum) | 
                (height < decor.spawnHeightMinimum)) 
                continue;
            
            StaticObject staticObj;
            staticObj.x = xp;
            staticObj.y = height;
            staticObj.z = zp;
            
            // Structures
            
            for (unsigned int s=0; s < world.mStructures.size(); s++) {
                
                float xCoord = (float)xx * 0.9f;
                float zCoord = (float)zz * 0.9f;
                
                if (Random.Perlin(xCoord, 0, zCoord, chunk.seed) < 0.1f) 
                    continue;
                
                if (Random.Range(0, (world.mStructures[s].rarity)) > 1) 
                    break;
                
                unsigned int numberOfElements = world.mStructures[s].elements.size();
                
                for (unsigned int e=0; e < numberOfElements; e++) {
                    
                    glm::vec3 pos(world.mStructures[s].elements[e].position.x - xp, 
                                world.mStructures[s].elements[e].position.y + height, 
                                world.mStructures[s].elements[e].position.z - zp);
                    
                    if (world.mStructures[s].elements[e].type == DECORATION_GRASS) 
                        AddDecorGrass(chunk, staticObj, staticMesh, pos.x, pos.y, pos.z);
                    
                    if (world.mStructures[s].elements[e].type == DECORATION_GRASS_THIN) 
                        AddDecorGrassThin(chunk, staticObj, staticMesh, pos.x, pos.y, pos.z);
                    
                    if (world.mStructures[s].elements[e].type == DECORATION_GRASS_THICK) 
                        AddDecorGrassThick(chunk, staticObj, staticMesh, pos.x, pos.y, pos.z);
                    
                    if (world.mStructures[s].elements[e].type == DECORATION_TREE) 
                        AddDecorTreeLogs(chunk, staticObj, staticMesh, pos.x, pos.y, pos.z);
                    
                    if (world.mStructures[s].elements[e].type == DECORATION_LEAVES) 
                        AddDecorTreeLeaves(chunk, staticObj, staticMesh, pos.x, pos.y, pos.z);
                    
                    continue;
                }
                
                continue;
            }
            
            
            // Grass
            if (decor.type == DECORATION_GRASS) {
                
                if ((unsigned int)Random.Range(0, 100) < decor.density) {
                    
                    
                    Color finalColor;
                    finalColor = Colors.green * 0.04f;
                    
                    finalColor += Colors.Make(Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                              Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                              Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f);
                    
                    staticObj.r = finalColor.r;
                    staticObj.g = finalColor.g;
                    staticObj.b = finalColor.b;
                    
                    AddDecorGrass(chunk, staticObj, staticMesh, -xp, height, -zp);
                }
                
            }
            
            // Thin grass
            if (decor.type == DECORATION_GRASS_THIN) {
                
                if ((unsigned int)Random.Range(0, 100) < decor.density) {
                    
                    unsigned int stackHeight = Random.Range((float)decor.spawnStackHeightMin, (float)decor.spawnStackHeightMax);
                    
                    for (unsigned int s=0; s < stackHeight; s++) {
                        
                        Color finalColor;
                        finalColor = (Colors.green * 0.018f) + (s * 0.001f);
                        
                        finalColor += Colors.Make(Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                                Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                                Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f);
                        
                        staticObj.r = finalColor.r;
                        staticObj.g = finalColor.g;
                        staticObj.b = finalColor.b;
                        
                        AddDecorGrassThin(chunk, staticObj, staticMesh, -xp, height + s, -zp);
                    }
                    
                }
                
            }
            
            // Thick grass
            if (decor.type == DECORATION_GRASS_THICK) {
                
                if ((unsigned int)Random.Range(0, 100) < decor.density) {
                    
                    Color finalColor;
                    finalColor = Colors.green * 0.05f;
                    
                    finalColor += Colors.Make(Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                            Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                            Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f);
                    
                    staticObj.r = finalColor.r;
                    staticObj.g = finalColor.g;
                    staticObj.b = finalColor.b;
                    
                    AddDecorGrassThick(chunk, staticObj, staticMesh, -xp, height, -zp);
                    
                }
                
            }
            
            // Trees
            if (decor.type == DECORATION_TREE) {
                
                if ((unsigned int)Random.Range(0, 100) < decor.density) {
                    
                    AddDecorTree(chunk, staticObj, staticMesh, -xp, height, -zp, Decoration::TreeOak);
                }
                
            }
            
            // Actor generation
            if (decor.type == DECORATION_ACTOR) {
                
                if ((unsigned int)Random.Range(0, 10000) < decor.density) {
                    
                    GameObject* actorObject = SpawnActor(from.x, 0, from.z);
                    
                    Actor* actor = actorObject->GetComponent<Actor>();
                    
                    DecodeGenome(decor, actor);
                    
                    actor->SetAge( 1000 + Random.Range(0, 1000) );
                    
                    if (Random.Range(0, 100) > 95) {
                        
                        unsigned int numberOfChildren = Random.Range(0, 4);
                        
                        for (unsigned int c=0; c < numberOfChildren; c++) {
                            
                            glm::vec3 actorPosition = from;
                            actorPosition.x += Random.Range(0, 3) - Random.Range(0, 3);
                            actorPosition.z += Random.Range(0, 3) - Random.Range(0, 3);
                            
                            GameObject* actorObject = SpawnActor(actorPosition.x, 0, actorPosition.z);
                            
                            Actor* actor = actorObject->GetComponent<Actor>();
                            
                            DecodeGenome(decor, actor);
                            
                            actor->SetAge( 100 + Random.Range(0, 200) );
                            
                        }
                        
                    }
                    
                }
                
            }
            
            continue;
        }
        
    }
    
    return;
}

