#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>


void ChunkManager::Decorate(Chunk* chunk) {
    unsigned int numberOfBiomes = world.mBiomes.size();
    if (numberOfBiomes == 0 || chunk->biomeMap.empty())
        return;
    
    Mesh* staticMesh = chunk->staticObject->GetComponent<MeshRenderer>()->mesh;
    chunk->statics.clear();
    staticMesh->ClearSubMeshes();
    
    unsigned int chunkSZ = chunkSize + 1;
    
    for (int xx = 0; xx < chunkSize; xx++) {
        float xp = xx - (chunkSize / 2);
        float staticX = chunk->x - xp;
        
        for (int zz = 0; zz < chunkSize; zz++) {
            float zp = zz - (chunkSize / 2);
            float staticZ = chunk->y - zp;
            
            glm::vec3 worldPos(staticX, 0, staticZ);
            int index = zz * chunkSZ + xx;
            
            // Find which biome dominates here
            int biomeIndex = chunk->biomeMap[index];
            if (biomeIndex < 0 || biomeIndex >= (int)world.mBiomes.size())
                continue;
            
            Biome& biome = world.mBiomes[biomeIndex];
            
            // Loop over that biome's decorations
            for (unsigned int d = 0; d < biome.decorations.size(); d++) {
                DecorationSpecifier& decor = biome.decorations[d];
                
                if ((unsigned int)Random.Range(0, 10000) > decor.density) 
                    continue;
                
                // Perlin generation
                if (Random.Perlin(xx * decor.noise, 0, zz * decor.noise, chunk->seed) < decor.threshold) 
                    continue;
                
                float height = 0.0f;
                glm::vec3 from(staticX, 0, staticZ);
                
                Hit hit;
                if (Physics.Raycast(from, glm::vec3(0, -1, 0), 2000.0f, hit, LayerMask::Ground)) 
                    height = hit.point.y;
                
                if (height > world.staticHeightCutoff) 
                    continue;
                
                switch (decor.type) {
                    
                case DecorationType::Grass:      AddDecorGrass(chunk, staticMesh, glm::vec3(-xp, height, -zp), decor.name); break;
                case DecorationType::Tree:       AddDecorTree(chunk, staticMesh, glm::vec3(-xp, height, -zp), decor.name); break;
                case DecorationType::Actor:      AddDecoreActor(glm::vec3(from.x, height, from.z), decor.name); break;
                case DecorationType::Structure:  AddDecorStructure(chunk, staticMesh, glm::vec3(-xp, height, -zp), decor.name); break;
                    
                }
                
            }
            
            
            
        }
        
    }
    
    staticMesh->Load();
    return;
}


unsigned int ChunkManager::AddDecor(Chunk* chunk, unsigned int index, Mesh* staticMesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Color color) {
    staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallHorz, false);
    staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallVert, false);
    
    unsigned int indexMesh = staticMesh->GetSubMeshCount() - 1;
    
    staticMesh->ChangeSubMeshColor(indexMesh,   color);
    staticMesh->ChangeSubMeshColor(indexMesh-1, color);
    
    staticMesh->ChangeSubMeshScale(indexMesh,   scale.x, scale.y, scale.z);
    staticMesh->ChangeSubMeshScale(indexMesh-1, scale.x, scale.y, scale.z);
    
    staticMesh->ChangeSubMeshRotation(indexMesh,   rotation.x, glm::vec3(1.0f, 0, 0));
    staticMesh->ChangeSubMeshRotation(indexMesh-1, rotation.x, glm::vec3(1.0f, 0, 0));
    staticMesh->ChangeSubMeshRotation(indexMesh,   rotation.y, glm::vec3(0, 0, 1.0f));
    staticMesh->ChangeSubMeshRotation(indexMesh-1, rotation.y, glm::vec3(0, 0, 1.0f));
    staticMesh->ChangeSubMeshRotation(indexMesh,   rotation.z, glm::vec3(0, 1.0f, 0));
    staticMesh->ChangeSubMeshRotation(indexMesh-1, rotation.z, glm::vec3(0, 1.0f, 0));
    
    StaticObject staticObject;
    staticMesh->GetSubMesh(indexMesh, staticObject.subMesh);
    staticObject.position = position;
    staticObject.rotation = rotation;
    staticObject.scale    = scale;
    staticObject.color    = glm::vec3(color.r, color.g, color.b);
    
    chunk->statics.push_back(staticObject);
    return index;
}


bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                       const glm::vec3& boxMin, const glm::vec3& boxMax,
                       float& outDistance) {
    float tMin = 0.0f;
    float tMax = 10000.0f;
    
    for (int i = 0; i < 3; ++i) {
        if (abs(rayDir[i]) < 0.0001f) {
            if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i])
                return false;
        } else {
            float ood = 1.0f / rayDir[i];
            float t1 = (boxMin[i] - rayOrigin[i]) * ood;
            float t2 = (boxMax[i] - rayOrigin[i]) * ood;
            if (t1 > t2) std::swap(t1, t2);

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);
            if (tMin > tMax)
                return false;
        }
    }
    
    outDistance = tMin;
    return true;
}


void ChunkManager::RemoveDecor(glm::vec3 position, glm::vec3 direction) {
    float threshold   =   0.7f;   // Focal threshold for selection
    float maxDistance =   10.0f;  // Max selection distance
    
    float closestDot = -1.0f;
    int bestChunkIndex = -1;
    int bestSubMeshIndex = -1;
    glm::vec3 bestLocalPos;
    
    for (unsigned int i = 0; i < chunks.Size(); i++) {
        Chunk& chunk = *chunks[i];
        glm::vec3 chunkPos = glm::vec3(chunk.x, 0, chunk.y);
        
        if (glm::distance(chunkPos, position) > chunkSize) 
            continue;
        
        Mesh* chunkMesh = chunks[i]->staticObject->GetComponent<MeshRenderer>()->mesh;
        
        SubMesh subMesh;
        for (unsigned int s = 0; s < chunkMesh->GetSubMeshCount(); s++) {
            chunkMesh->GetSubMesh(s, subMesh);
            
            glm::vec3 worldPos = subMesh.position + glm::vec3(chunks[i]->x, 0, chunks[i]->y);
            
            if (glm::distance(worldPos, position) > maxDistance) 
                continue;
            
            glm::vec3 scale = glm::vec3(2.0f);
            glm::vec3 min = worldPos - (scale * 0.5f);
            glm::vec3 max = worldPos + (scale * 0.5f);
            
            float hitDistance=0;
            if (!RayIntersectsAABB(position, glm::normalize(direction), min, max, hitDistance)) 
                continue;
            
            if (hitDistance > maxDistance) 
                continue;
            
            glm::vec3 toObject = glm::normalize(worldPos - position);
            float dot = glm::dot(toObject, glm::normalize(direction));
            
            if (dot < threshold) 
                continue;
            
            if (dot > closestDot) {
                closestDot = dot;
                bestChunkIndex = i;
                bestSubMeshIndex = s;
                bestLocalPos = subMesh.position;
            }
        }
    }
    
    // Remove the best candidate
    if (bestChunkIndex != -1) {
        Chunk& chunk = *chunks[bestChunkIndex];
        Mesh* mesh = chunk.staticObject->GetComponent<MeshRenderer>()->mesh;
        
        // Remove from statics
        for (int i = 0; i < chunk.statics.size(); ++i) {
            if (chunk.statics[i].position == bestLocalPos) {
                chunk.statics.erase(chunk.statics.begin() + i);
                
                // Remove both sub meshes
                mesh->RemoveSubMesh(bestSubMeshIndex);
                mesh->RemoveSubMesh(bestSubMeshIndex);
                break;
            }
        }
        
        mesh->Load();
    }
}

void ChunkManager::AddDecorGrass(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    Color finalColor = Colors.white;
    float grassWidth = 1.0f;
    float grassHeight = 1.0f;
    
    if (name == "Short") {
        grassWidth = 1.2f;
        grassHeight = 0.5f;
        finalColor = Colors.green * 0.034f;
    }
    if (name == "Tall") {
        grassWidth = 0.7f;
        grassHeight = 2.0f;
        finalColor = Colors.green * 0.034f;
    }
    
    if (name == "Dry") {
        grassWidth = 0.7f;
        grassHeight = 2.0f;
        finalColor = Colors.brown * 0.08f;
    }
    finalColor += Colors.MakeRandomGrayScale() * 0.0087f;
    
    glm::vec3 scale(grassWidth, grassHeight, grassWidth);
    glm::vec3 rotation(0.0f, 0.0f, Random.Range(0, 360));
    
    unsigned int beginningIndex = chunk->statics.size();
    AddDecor(chunk, beginningIndex, staticMesh, position + glm::vec3(0.0f, grassHeight * 0.16f, 0.0f), rotation, scale, finalColor);
}


void ChunkManager::AddDecorTree(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    float minimumHeight = 3.0f;
    float maximumHeight = 5.0f;
    
    int numberOfLeavesMin = 4;
    int numberOfLeavesMax = 8;
    float leafSpreadWidth  = 0.8f;
    float leafSpreadHeight = 0.5f;
    float trunkSize = 0.3f;
    float leafWidth = 1.0f;
    float leafHeight = 0.7f;
    
    if (name == "Oak") {
        minimumHeight = 8.0f;
        maximumHeight = 12.0f;
        
        numberOfLeavesMin = 6;
        numberOfLeavesMax = 10;
        leafSpreadWidth  = 1.25f;
        leafSpreadHeight = 0.5f;
        trunkSize = 0.7f;
        leafWidth = 5.5f;
        leafHeight = 2.0f;
    }
    
    float height = Random.Range(minimumHeight, maximumHeight);
    
    glm::vec3 scale(trunkSize, height, trunkSize);
    glm::vec3 rotation(0.0f, 0.0f, 0.0f);
    
    Color finalColor;
    finalColor = Colors.brown * (0.02f + (Random.Range(0, 80) * 0.001f));
    
    unsigned int beginningIndex = chunk->statics.size();
    AddDecor(chunk, beginningIndex, staticMesh, glm::vec3(position.x, position.y + (height * 0.25f), position.z), rotation, scale, finalColor);
    
    int numberOfLeaves = numberOfLeavesMin + (Random.Range(0, numberOfLeavesMax));
    
    for (int i=0; i < numberOfLeaves; i++) {
        glm::vec3 scale(leafWidth, leafHeight, leafWidth);
        glm::vec3 rotation(Random.Range(0, 360), Random.Range(0, 360), Random.Range(0, 360));
        
        Color finalColor;
        finalColor = Colors.green * (0.04f + (Random.Range(0, 80) * 0.001f));
        
        float xx = Random.Range(0.0f, leafSpreadWidth) - Random.Range(0.0f, leafSpreadWidth);
        float yy = Random.Range(0.0f, leafSpreadHeight) - Random.Range(0.0f, leafSpreadHeight);
        float zz = Random.Range(0.0f, leafSpreadWidth) - Random.Range(0.0f, leafSpreadWidth);
        
        glm::vec3 offsetPosition = position + glm::vec3(xx, yy + (height * 0.5f), zz);
        
        AddDecor(chunk, beginningIndex, staticMesh, offsetPosition, rotation, scale, finalColor);
    }
}




void ChunkManager::AddDecorStructure(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    
    return;
    
    //if ((unsigned int)Random.Range(0, 10000) < 7) 
    //    return;
    
    //float xCoord = (float)position.x * 0.9f;
    //float zCoord = (float)position.z * 0.9f;
    
    //if ((Random.Perlin(xCoord, 0, zCoord, chunk.seed) * 4.0f) < 2.0f) 
    //    continue;
    
    {
    /*
    glm::vec3 scale(10.3f, 20.0f, 10.3f);
    glm::vec3 rotation(0.0f, 0.0f, Random.Range(0, 360));
    
    Color finalColor;
    finalColor = Colors.yellow;
    
    StaticObject* staticObject = AddDecor(chunk, staticMesh, position + glm::vec3(0.0f, 30.0f, 0.0f), rotation, scale, finalColor);
    
    Scenery scenery;
    scenery.grometry.push_back(staticObject);
    
    chunk.scenery.push_back(scenery);
    */
    
    }
    
    float wikiupHeight = 24.0f;
    float wikiupSpread = 1.8f;
    float wikiupSpreadExt = wikiupSpread * 0.7f;
    
    float stickThickness = 0.4f;
    
    /*
    {
    glm::vec3 pos(position.x - 3.0f, position.y + (wikiupHeight * 0.1f), position.z);
    AddDecor(chunk, staticMesh, pos, glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(stickThickness, wikiupHeight, stickThickness), Colors.brown);
    }
    {
    glm::vec3 pos(position.x - 1.0f, position.y + (wikiupHeight * 0.1f), position.z - 3.25f);
    AddDecor(chunk, staticMesh, pos, glm::vec3(0.0f, 65.0f, 30.0f), glm::vec3(stickThickness, wikiupHeight, stickThickness), Colors.brown);
    }
    {
    glm::vec3 pos(position.x + 3.0f, position.y + (wikiupHeight * 0.1f), position.z - 2.5f);
    AddDecor(chunk, staticMesh, pos, glm::vec3(0.0f, 135.0f, 30.0f), glm::vec3(stickThickness, wikiupHeight, stickThickness), Colors.brown);
    }
    {
    glm::vec3 pos(position.x + 3.0f, position.y + (wikiupHeight * 0.1f), position.z + 2.5f);
    AddDecor(chunk, staticMesh, pos, glm::vec3(0.0f, -135.0f, 30.0f), glm::vec3(stickThickness, wikiupHeight, stickThickness), Colors.brown);
    }
    {
    glm::vec3 pos(position.x - 1.0f, position.y + (wikiupHeight * 0.1f), position.z + 3.25f);
    AddDecor(chunk, staticMesh, pos, glm::vec3(0.0f, -65.0f, 30.0f), glm::vec3(stickThickness, wikiupHeight, stickThickness), Colors.brown);
    }
    */
    
    /*
    {
    glm::vec3 pos(position.x - wikiupSpreadExt, position.y + (wikiupHeight * 0.1f), position.z - wikiupSpreadExt);
    AddDecor(chunk, staticMesh, pos, glm::vec3(0.0f, 20.0f, 45.0f), glm::vec3(stickThickness, wikiupHeight, stickThickness), Colors.brown);
    }
    {
    glm::vec3 pos(position.x - wikiupSpreadExt, position.y + (wikiupHeight * 0.1f), position.z + wikiupSpreadExt);
    AddDecor(chunk, staticMesh, pos, glm::vec3(0.0f, -20.0f, -45.0f), glm::vec3(stickThickness, wikiupHeight, stickThickness), Colors.brown);
    }
    {
    glm::vec3 pos(position.x + wikiupSpread, position.y + (wikiupHeight * 0.1f), position.z);
    AddDecor(chunk, staticMesh, pos, glm::vec3(20.0f, 0.0f, 0.0f), glm::vec3(stickThickness, wikiupHeight, stickThickness), Colors.brown);
    }
    
    
    {
    glm::vec3 pos(position.x - wikiupSpreadExt, position.y + (wikiupHeight * 0.1f), position.z + wikiupSpreadExt);
    AddDecor(chunk, staticMesh, pos, glm::vec3(0.0f, -20.0f, -70.0f), glm::vec3(stickThickness, wikiupHeight, stickThickness), Colors.brown);
    }
    */
    
    /*
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
    */
}


void ChunkManager::DecodeGenome(std::string name, Actor* actorPtr) {
    
    if (name == "Human")  {AI.genomes.presets.Human( actorPtr );}
    if (name == "Sheep")  {AI.genomes.presets.Sheep( actorPtr );}
    if (name == "Bear")   {AI.genomes.presets.Bear( actorPtr );}
    if (name == "Dog")    {AI.genomes.presets.Dog( actorPtr );}
    if (name == "Bovine") {AI.genomes.presets.Bovine( actorPtr );}
    if (name == "Horse")  {AI.genomes.presets.Horse( actorPtr );}
    
    return;
}

void ChunkManager::AddDecoreActor(glm::vec3 position, std::string name) {
    Actor* actor = SummonActor(position);
    DecodeGenome(name, actor);
    
    actor->isActive = true;
    actor->RebuildGeneticExpression();
    
    actor->state.current = ActorState::State::None;
    actor->state.mode = ActorState::Mode::Idle;
    
    actor->physical.UpdatePhysicalCollider();
    actor->physical.SetAge( actor->physical.GetAdultAge() + Random.Range(0, 1000) );
}


/*


void ChunkManager::AddDecorGrassThin(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, glm::vec3 position) {
    glm::vec3 scale(0.1f, 1.0f, 0.1f);
    glm::vec3 rotation(0.0f, 0.0f, Random.Range(0, 360));
    
    Color finalColor;
    finalColor = Colors.green * (0.04f + (Random.Range(0, 80) * 0.001f));
    staticObject.color.x = finalColor.r;
    staticObject.color.y = finalColor.g;
    staticObject.color.z = finalColor.b;
    
    AddDecor(chunk, staticObject, staticMesh, position, rotation, scale);
}


void ChunkManager::AddDecorGrassThick(Chunk& chunk, StaticObject& staticObject, Mesh* staticMesh, glm::vec3 position) {
    
    
}


*/



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

