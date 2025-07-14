#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>




void DecodeGenome(DecorationSpecifier& decor, Actor* actorPtr) {
    
    if (decor.name == "Human")  {AI.genomes.presets.Human( actorPtr );}
    if (decor.name == "Sheep")  {AI.genomes.presets.Sheep( actorPtr );}
    if (decor.name == "Bear")   {AI.genomes.presets.Bear( actorPtr );}
    if (decor.name == "Dog")    {AI.genomes.presets.Dog( actorPtr );}
    if (decor.name == "Bovine") {AI.genomes.presets.Bovine( actorPtr );}
    if (decor.name == "Horse")  {AI.genomes.presets.Horse( actorPtr );}
    
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
    
    //unsigned int leafAccent = Random.Range(0, 100);
    
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
    
    for (int xx=0; xx < chunkSize; xx++) {
        
        for (int zz=0; zz < chunkSize; zz++) {
            
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
            /*
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
            */
            
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
                    Actor* actor = SummonActor(glm::vec3(from.x, 0, from.z));
                    
                    DecodeGenome(decor, actor);
                    
                    actor->isActive = true;
                    actor->RebuildGeneticExpression();
                    
                    actor->state.current = ActorState::State::None;
                    actor->state.mode = ActorState::Mode::Idle;
                    
                    actor->physical.SetAge( actor->physical.GetAdultAge() + Random.Range(0, 1000) );
                }
                
            }
            
            continue;
        }
        
    }
    
    staticMesh->Load();
    return;
}

