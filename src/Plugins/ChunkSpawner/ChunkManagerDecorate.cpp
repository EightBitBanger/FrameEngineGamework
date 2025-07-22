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
            staticObj.position.x = xp;
            staticObj.position.y = height;
            staticObj.position.z = zp;
            
            //if ((unsigned int)Random.Range(0, 10) < decor.density) 
                AddDecorGrass(chunk, staticObj, staticMesh, glm::vec3(-xp, height, -zp));
            
            
            
            
            // Structures
            /*
            if ((unsigned int)Random.Range(0, 10000) < 7) {
                float xCoord = (float)xp * 0.9f;
                float zCoord = (float)zp * 0.9f;
                
                //if ((Random.Perlin(xCoord, 0, zCoord, chunk.seed) * 4.0f) < 2.0f) 
                //    continue;
                
                float villageSpread = 8.0f;
                float wikiupSpread = 0.0f;
                
                unsigned int numberOfWikiups = 1;//Random.Range(1, 3);
                for (unsigned int i=0; i < numberOfWikiups; i++) {
                    float xx = Random.Range(0.0f, wikiupSpread) - Random.Range(0.0f, wikiupSpread);
                    float zz = Random.Range(0.0f, wikiupSpread) - Random.Range(0.0f, wikiupSpread);
                    
                    float wikiupHeight = height - Random.Range(0.0f, 1.0f);
                    
                    AddDecor(chunk, staticObj, staticMesh, -xp + xx,      wikiupHeight + 2, -zp + zz,        0.0f, 0.0f);
                    AddDecor(chunk, staticObj, staticMesh, -xp-1 + xx,    wikiupHeight + 2, -zp-1 + zz,      0.0f,  0.0f);
                    AddDecor(chunk, staticObj, staticMesh, -xp-1.5f + xx, wikiupHeight + 2, -zp+0.5f + zz,   0.0f,   0.0f);
                }
                
                
                float spawnVillagerLow  = 4.0f;
                float spawnVillagerHigh = 8.0f;
                
                unsigned int numberOfVillagers = Random.Range(spawnVillagerLow, spawnVillagerHigh);
                for (unsigned int i=0; i < numberOfVillagers; i++) {
                    
                    float xx = Random.Range(0.0f, villageSpread) - Random.Range(0.0f, villageSpread);
                    float zz = Random.Range(0.0f, villageSpread) - Random.Range(0.0f, villageSpread);
                    Actor* actor = SummonActor( glm::vec3(from.x + xx, 0.0f, from.z + zz) );
                    AI.genomes.presets.Human(actor);
                    
                    if (i < (numberOfVillagers / 2)) {
                        actor->physical.SetSexualOrientation(true);
                    } else {
                        actor->physical.SetSexualOrientation(false);
                    }
                    
                    actor->physical.SetAge( Random.Range(actor->physical.GetAdultAge(), actor->physical.GetAdultAge() * 2.0f) );
                    actor->RebuildGeneticExpression();
                    
                    actor->isActive = true;
                    actor->physical.UpdatePhysicalCollider();
                }
                
                
            }
            */
            
            
            
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
            
            
            /*
            switch (decor.type) {
                
            case DECORATION_GRASS:
                if ((unsigned int)Random.Range(0, 100) < decor.density) {
                    
                    Color finalColor;
                    finalColor = Colors.green * 0.04f;
                    
                    finalColor += Colors.Make(Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                              Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                              Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f);
                    
                    staticObj.red   = finalColor.r;
                    staticObj.green = finalColor.g;
                    staticObj.blue  = finalColor.b;
                    
                    AddDecorGrass(chunk, staticObj, staticMesh, -xp, height, -zp);
                }
                break;
                
            case DECORATION_GRASS_THIN:
                if ((unsigned int)Random.Range(0, 100) < decor.density) {
                    
                    unsigned int stackHeight = Random.Range((float)decor.spawnStackHeightMin, (float)decor.spawnStackHeightMax);
                    
                    for (unsigned int s=0; s < stackHeight; s++) {
                        
                        Color finalColor;
                        finalColor = (Colors.green * 0.018f) + (s * 0.001f);
                        
                        finalColor += Colors.Make(Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                                  Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                                  Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f);
                        
                        staticObj.red    = finalColor.r;
                        staticObj.green = finalColor.g;
                        staticObj.blue  = finalColor.b;
                        
                        AddDecorGrassThin(chunk, staticObj, staticMesh, -xp, height + s, -zp);
                    }
                    
                }
                break;
                
            case DECORATION_GRASS_THICK:
                if ((unsigned int)Random.Range(0, 100) < decor.density) {
                    
                    Color finalColor;
                    finalColor = Colors.green * 0.05f;
                    
                    finalColor += Colors.Make(Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                              Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f, 
                                              Random.Range(0, 10) * 0.001f - Random.Range(0, 10) * 0.001f);
                    staticObj.red   = finalColor.r;
                    staticObj.green = finalColor.g;
                    staticObj.blue  = finalColor.b;
                    
                    AddDecorGrassThick(chunk, staticObj, staticMesh, -xp, height, -zp);
                }
                break;
                
            case DECORATION_TREE:
                if ((unsigned int)Random.Range(0, 100) < decor.density) {
                    
                    AddDecorTree(chunk, staticObj, staticMesh, -xp, height, -zp, Decoration::TreeOak);
                }
                break;
                
            case DECORATION_ACTOR:
                
                if ((unsigned int)Random.Range(0, 10000) < decor.density) {
                    Actor* actor = SummonActor(glm::vec3(from.x, 0, from.z));
                    
                    DecodeGenome(decor, actor);
                    
                    actor->isActive = true;
                    actor->RebuildGeneticExpression();
                    
                    actor->state.current = ActorState::State::None;
                    actor->state.mode = ActorState::Mode::Idle;
                    
                    actor->physical.UpdatePhysicalCollider();
                    actor->physical.SetAge( actor->physical.GetAdultAge() + Random.Range(0, 1000) );
                }
                break;
                
            }
            */
            
            
        }
        
    }
    
    staticMesh->Load();
    return;
}







void ChunkManager::AddDecor(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
    StaticObject newStaticObject = staticObject;
    newStaticObject.position.y = position.y;
    
    staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallHorz, false);
    staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallVert, false);
    
    unsigned int index = staticMesh->GetSubMeshCount() - 1;
    
    staticMesh->ChangeSubMeshScale(index,   scale.x, scale.y, scale.z);
    staticMesh->ChangeSubMeshScale(index-1, scale.x, scale.y, scale.z);
    
    Color finalColor;
    finalColor.r = staticObject.color.x;
    finalColor.g = staticObject.color.y;
    finalColor.b = staticObject.color.z;
    
    staticMesh->ChangeSubMeshColor(index,   finalColor);
    staticMesh->ChangeSubMeshColor(index-1, finalColor);
    
    staticMesh->ChangeSubMeshRotation(index,   rotation.x, glm::vec3(0, 0, 1.0f));
    staticMesh->ChangeSubMeshRotation(index-1, rotation.x, glm::vec3(0, 0, 1.0f));
    staticMesh->ChangeSubMeshRotation(index,   rotation.y, glm::vec3(1.0f, 0, 0));
    staticMesh->ChangeSubMeshRotation(index-1, rotation.y, glm::vec3(1.0f, 0, 0));
    staticMesh->ChangeSubMeshRotation(index,   rotation.z, glm::vec3(0, 1.0f, 0));
    staticMesh->ChangeSubMeshRotation(index-1, rotation.z, glm::vec3(0, 1.0f, 0));
    
    newStaticObject.rotation = rotation;
    newStaticObject.rotation = rotation;
    newStaticObject.scale    = scale;
    newStaticObject.color    = staticObject.color;
    
    chunk.statics.push_back(newStaticObject);
    
    return;
}


void ChunkManager::AddDecorGrass(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, glm::vec3 position) {
    Color finalColor;
    finalColor = Colors.green * (0.04f + (Random.Range(0, 80) * 0.001f));
    staticObject.color.x = finalColor.r;
    staticObject.color.y = finalColor.g;
    staticObject.color.z = finalColor.b;
    
    float height = 0.45f;
    float width = 0.8f;
    position.y += height * 0.25f;
    
    for (unsigned int i=0; i < 1; i++) {
        glm::vec3 scale(width, height, width);
        glm::vec3 rotation(0.0f, 0.0f, Random.Range(0, 360));
        
        AddDecor(chunk, staticObject, staticMesh, position, rotation, scale);
        
        finalColor = Colors.green * (0.02f + (Random.Range(0, 80) * 0.001f));
        staticObject.color.x = finalColor.r;
        staticObject.color.y = finalColor.g;
        staticObject.color.z = finalColor.b;
        
        position.y += height * 0.5f;
        
        width *= 0.87f;
        
    }
    
    // TODO Make static objects destructible
    
    //staticObject.collisionBody = Physics.CreateCollisionBody(staticObject.x, staticObject.y, staticObject.z);
    //rp3d::Collider* collider = staticObject.rigidBody->addCollider(Engine.colliders.box, transform);
    
    //rp3d::Transform transform;
    //staticObject.rigidBody->setType(rp3d::BodyType::STATIC);
    
    //collider->setCollisionCategoryBits((unsigned short)LayerMask::Object); // Ray cast as an "Object"
    //collider->setCollideWithMaskBits((unsigned short)LayerMask::Ground);   // Collide as a solid object
    
    return;
}








/*
void ChunkManager::AddDecorGrass(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, float xx, float yy, float zz) {
    
    staticObject.type = DECORATION_GRASS;
    
    staticMesh->AddSubMesh(xx, yy, zz, subMeshGrassHorz, false);
    staticMesh->AddSubMesh(xx, yy, zz, subMeshGrassVert, false);
    
    unsigned int index = staticMesh->GetSubMeshCount() - 1;
    
    Color finalColor;
    
    finalColor.r = staticObject.red;
    finalColor.g = staticObject.green;
    finalColor.b = staticObject.blue;
    
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
    
    finalColor.r = newStaticObject.red;
    finalColor.g = newStaticObject.green;
    finalColor.b = newStaticObject.blue;
    
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
    
    finalColor.r = newStaticObject.red;
    finalColor.g = newStaticObject.green;
    finalColor.b = newStaticObject.blue;
    
    if (Random.Range(0, 100) < 20) finalColor = Colors.yellow * 0.05f;
    if (Random.Range(0, 100) < 20) finalColor = Colors.orange * 0.01f;
    
    staticMesh->ChangeSubMeshColor(index, finalColor);
    staticMesh->ChangeSubMeshColor(index-1, finalColor);
    
    newStaticObject.red   = finalColor.r;
    newStaticObject.green = finalColor.g;
    newStaticObject.blue  = finalColor.b;
    
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
    
    finalColor.r = newStaticObject.red;
    finalColor.g = newStaticObject.green;
    finalColor.b = newStaticObject.blue;
    
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
    finalColor.r = newStaticObject.red;
    finalColor.g = newStaticObject.green;
    finalColor.b = newStaticObject.blue;
    
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
        
        staticObject.red   = finalColor.r;
        staticObject.green = finalColor.g;
        staticObject.blue  = finalColor.b;
        
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
        
        newStaticObject.red   = finalColor.r;
        newStaticObject.green = finalColor.g;
        newStaticObject.blue  = finalColor.b;
        
        AddDecorTreeLeaves(chunk, newStaticObject, staticMesh, xx + offset_xx, yy + logHeight + world.leafHeightOffset + offset_yy, zz + offset_zz);
        
        continue;
    }
    
    return;
}

*/

