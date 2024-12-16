#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

unsigned int actorIndex = 0;
unsigned int chunkIndex = 0;

void ChunkManager::Update(void) {
    
    if (Engine.cameraController == nullptr) 
        return;
    
    if (!world.doGenerateChunks) 
        return;
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    playerPosition.y = 0;
    
    //
    // Update actors
    
    unsigned int numberOfActors = actors.size();
    
    if (numberOfActors > 0) {
        
        GameObject* actorObject = actors[actorIndex];
        
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        
        glm::vec3 actorPos = actorPtr->GetPosition();
        
        if (glm::distance(actorPos, playerPosition) < (renderDistance * chunkSize) * 0.9f ) 
            actorPtr->SetGeneticExpressionFlag();
        
        actorIndex++;
        
        if (actorIndex >= numberOfActors) 
            actorIndex = 0;
        
    }
    
    //
    // Destroy chunks
    
    unsigned int numberOfChunks = chunks.size();
    
    if (numberOfChunks > 0) {
        
        for (unsigned int c=0; c < 16; c++) {
            
            Chunk& chunk = chunks[chunkIndex];
            
            glm::vec3 chunkPos = glm::vec3(chunk.x, 0, chunk.y);
            
            if (glm::distance(chunkPos, playerPosition) > (renderDistance * chunkSize) * 1.5f ) {
                
                SaveChunk( chunk, true );
                
                DestroyChunk( chunk );
                
                chunks.erase( chunks.begin() + chunkIndex );
                
            }
            
            chunkIndex++;
            
            if (chunkIndex >= numberOfChunks) 
                chunkIndex = 0;
            
            continue;
        }
        
    }
    
    
    //
    // Generate chunks
    
    for (ChunkCounterX=0; ChunkCounterX <= renderDistance; ChunkCounterX++) {
        
        if (ChunkCounterX >= renderDistance) {
            
            ChunkCounterX = 0;
            break;
        }
        
        int xx = ChunkCounterX;
        
        for (ChunkCounterZ=0; ChunkCounterZ <= renderDistance; ChunkCounterZ++) {
            
            if (ChunkCounterZ >= renderDistance) {
                ChunkCounterZ = 0;
                break;
            }
            
            int zz = ChunkCounterZ;
            
            float chunkX = Math.Round( playerPosition.x / chunkSize + xx);
            float chunkZ = Math.Round( playerPosition.z / chunkSize + zz);
            
            glm::vec2 chunkPosition;
            chunkPosition.x = (chunkX * chunkSize) - (renderDistance * (chunkSize / 2));
            chunkPosition.y = (chunkZ * chunkSize) - (renderDistance * (chunkSize / 2));
            
            bool chunkFound = false;
            
            for (unsigned int c=0; c < chunks.size(); c++) {
                
                Chunk& chunkPtr = chunks[c];
                
                if (glm::vec3(chunkPtr.x, 0, chunkPtr.y) == glm::vec3(chunkPosition.x, 0, chunkPosition.y)) {
                    
                    chunkFound = true;
                    
                    break;
                }
                
            }
            
            if (chunkFound) 
                continue;
            
            if (glm::distance(glm::vec3(chunkPosition.x, 0, chunkPosition.y), playerPosition) > (renderDistance * (chunkSize / 2))) 
                continue;
            
            // Check if the chunk exists on disk
            
            std::string filename = Float.ToString(chunkPosition.x) + "_" + Float.ToString(chunkPosition.y);
            
            std::string worldFilename = "worlds/" + world.name + "/chunks/" + filename;
            
            if (Serializer.CheckExists( worldFilename )) {
                
                Chunk chunk = CreateChunk(chunkPosition.x, chunkPosition.y);
                
                LoadChunk( chunk );
                
                chunks.push_back( chunk );
                
                continue;
            }
            
            // Generate a new chunk
            Chunk chunk = CreateChunk(chunkPosition.x, chunkPosition.y);
            
            // Decorate chunk
            MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
            
            Decorate(chunk, chunkPosition.x, chunkPosition.y, staticRenderer->mesh);
            
            staticRenderer->mesh->Load();
            
            chunks.push_back( chunk );
            
            continue;
        }
        
        continue;
    }
    
    return;
}



void DecodeGenome(Decoration& decor, Actor* actorPtr) {
    
    if (decor.name == "Sheep")  {AI.genomes.Sheep( actorPtr );}
    if (decor.name == "Bear")   {AI.genomes.Bear( actorPtr );}
    
    return;
}



void ChunkManager::Decorate(Chunk& chunk, int chunkX, int chunkZ, Mesh* staticMesh) {
    
    if (world.mDecorations.size() == 0) 
        return;
    
    chunk.statics.clear();
    
    staticMesh->ClearSubMeshes();
    
    for (int xx=0; xx < chunkSize-1; xx++) {
        
        for (int zz=0; zz < chunkSize-1; zz++) {
            
            float xp = xx - (chunkSize / 2);
            float zp = zz - (chunkSize / 2);
            
            float staticX = (chunkX - xp);
            float staticZ = (chunkZ - zp);
            
            glm::vec3 from(staticX, 0, staticZ);
            glm::vec3 direction(0, -1, 0);
            
            Hit hit;
            
            float distance = 2000.0f;
            float height = 0.0f;
            
            if (Physics.Raycast(from, direction, distance, hit, LayerMask::Ground)) 
                height = hit.point.y;
            
            if (height > world.staticHeightCutoff) 
                continue;
            
            if (height == 0.0f) 
                continue;
            
            unsigned int decorIndex = Random.Range(0, world.mDecorations.size());
            
            Decoration decor = world.mDecorations[ decorIndex ];
            
            // Spawn density
            if ((unsigned int)Random.Range(0, 10000) > decor.density) 
                continue;
            
            // World spawn height range
            if ((height > decor.spawnHeightMaximum) | 
                (height < decor.spawnHeightMinimum)) 
                continue;
            
            StaticObject staticObj;
            staticObj.x = xp;
            staticObj.y = height;
            staticObj.z = zp;
            
            // Stack height
            unsigned int stackHeight = Random.Range((float)decor.spawnStackHeightMin, (float)decor.spawnStackHeightMax);
            
            
            // Grass
            if (decor.type == DECORATION_GRASS) {
                
                staticObj.type = DECORATION_GRASS;
                
                staticMesh->AddSubMesh(-xp, height, -zp, subMeshGrassHorz, false);
                staticMesh->AddSubMesh(-xp, height, -zp, subMeshGrassVert, false);
                
                unsigned int index = staticMesh->GetSubMeshCount() - 1;
                
                Color finalColor;
                finalColor = Colors.green * 0.04f;
                
                staticMesh->ChangeSubMeshColor(index, finalColor);
                staticMesh->ChangeSubMeshColor(index-1, finalColor);
                
                staticObj.r = finalColor.r;
                staticObj.g = finalColor.g;
                staticObj.b = finalColor.b;
                
                chunk.statics.push_back(staticObj);
            }
            
            // Thin grass
            if (decor.type == DECORATION_GRASS_THIN) {
                
                for (unsigned int c=0; c < stackHeight; c++) {
                    
                    staticObj.type = DECORATION_GRASS_THIN;
                    staticObj.y = height + c;
                    
                    staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshStemHorz, false);
                    staticMesh->AddSubMesh(-xp, height + c, -zp, subMeshStemVert, false);
                    
                    unsigned int index = staticMesh->GetSubMeshCount() - 1;
                    
                    Color finalColor;
                    finalColor = Colors.green * 0.018f;
                    
                    finalColor += Colors.Make(Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f,
                                              Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f,
                                              Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f);
                    
                    staticMesh->ChangeSubMeshColor(index, finalColor);
                    staticMesh->ChangeSubMeshColor(index-1, finalColor);
                    
                    staticObj.r = finalColor.r;
                    staticObj.g = finalColor.g;
                    staticObj.b = finalColor.b;
                    
                    chunk.statics.push_back(staticObj);
                    
                }
                
                continue;
            }
            
            // Thicker grass
            if (decor.type == DECORATION_GRASS_THICK) {
                
                staticObj.type = DECORATION_GRASS_THICK;
                
                staticMesh->AddSubMesh(-xp, height, -zp, subMeshWallHorz, false);
                staticMesh->AddSubMesh(-xp, height, -zp, subMeshWallVert, false);
                
                unsigned int index = staticMesh->GetSubMeshCount() - 1;
                
                Color finalColor;
                finalColor = Colors.green * 0.05f;
                
                if (Random.Range(0, 100) < 20) finalColor = Colors.yellow * 0.05f;
                if (Random.Range(0, 100) < 20) finalColor = Colors.orange * 0.01f;
                
                staticMesh->ChangeSubMeshColor(index, finalColor);
                staticMesh->ChangeSubMeshColor(index-1, finalColor);
                
                staticObj.r = finalColor.r;
                staticObj.g = finalColor.g;
                staticObj.b = finalColor.b;
                
                chunk.statics.push_back(staticObj);
                
                continue;
            }
            
            // Trees
            if (decor.type == DECORATION_TREE) {
                
                // Tree logs
                
                float logHeight = Random.Range((float)decor.spawnStackHeightMin, (float)decor.spawnStackHeightMax);
                
                for (unsigned int s=0; s < logHeight; s++) {
                    
                    staticObj.type = DECORATION_TREE;
                    staticObj.y = height + s - 1.0f;
                    
                    staticMesh->AddSubMesh(-xp, height + s - 1.0f, -zp, subMeshTree, false);
                    
                    unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
                    
                    unsigned int index = numberOfSubMeshes - 1;
                    
                    Color finalColor;
                    Color lowTrunk;
                    Color highTrunk;
                    
                    lowTrunk  = (Colors.brown * 0.1f) + (Colors.green * 0.01);
                    highTrunk = (Colors.brown * 0.8f) + (Colors.green * 0.01);
                    
                    finalColor = Colors.Lerp(lowTrunk, highTrunk, (s * 0.087f));
                    
                    staticMesh->ChangeSubMeshColor(index, finalColor);
                    
                    staticObj.r = finalColor.r;
                    staticObj.g = finalColor.g;
                    staticObj.b = finalColor.b;
                    
                    chunk.statics.push_back(staticObj);
                    
                }
                
                // Leaves
                
                float leafCount = Random.Range(((float)world.numberOfLeaves) / 2, (float)world.numberOfLeaves);
                
                unsigned int leafAccent = Random.Range(0, 100);
                
                for (unsigned int z=0; z < leafCount; z++) {
                    
                    float offset_xx = Random.Range(0.0f, world.leafSpreadArea)   - Random.Range(0.0f, world.leafSpreadArea);
                    float offset_yy = Random.Range(0.0f, world.leafSpreadHeight) - Random.Range(0.0f, world.leafSpreadHeight);
                    float offset_zz = Random.Range(0.0f, world.leafSpreadArea)   - Random.Range(0.0f, world.leafSpreadArea);
                    
                    staticObj.type = DECORATION_LEAVES;
                    staticObj.x = xp + offset_xx;
                    staticObj.y = world.leafHeightOffset + height + logHeight + offset_yy;
                    staticObj.z = zp + offset_zz;
                    
                    staticMesh->AddSubMesh(0, 0, 0, subMeshWallHorz, false);
                    staticMesh->AddSubMesh(0, 0, 0, subMeshWallVert, false);
                    
                    unsigned int numberOfSubMeshes = staticMesh->GetSubMeshCount();
                    unsigned int index = numberOfSubMeshes - 1;
                    
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
                    
                    staticMesh->ChangeSubMeshColor(index, finalColor);
                    staticMesh->ChangeSubMeshColor(index-1, finalColor);
                    
                    staticMesh->ChangeSubMeshPosition(index,   -xp + offset_xx, world.leafHeightOffset + height + logHeight + offset_yy, -zp + offset_zz);
                    staticMesh->ChangeSubMeshPosition(index-1, -xp + offset_xx, world.leafHeightOffset + height + logHeight + offset_yy, -zp + offset_zz);
                    
                    staticObj.r = finalColor.r;
                    staticObj.g = finalColor.g;
                    staticObj.b = finalColor.b;
                    
                    chunk.statics.push_back(staticObj);
                    
                }
                
                
                continue;
            }
            
            // Actor generation
            if (decor.type == DECORATION_ACTOR) {
                
                GameObject* actorObject = SpawnActor(from.x, 0, from.z);
                
                Actor* actor = actorObject->GetComponent<Actor>();
                
                DecodeGenome(decor, actor);
                
                actor->SetAge( 1000 );
                
                continue;
            }
            
            
        }
        
    }
    
    return;
}

