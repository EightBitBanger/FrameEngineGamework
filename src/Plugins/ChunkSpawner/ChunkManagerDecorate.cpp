#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>


void ChunkManager::Decorate(Chunk* chunk) {
    unsigned int numberOfBiomes = world.biomes.size();
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
            if (biomeIndex < 0 || biomeIndex >= (int)world.biomes.size())
                continue;
            
            Biome& biome = world.biomes[biomeIndex];
            
            // Loop over that biome's decorations
            for (unsigned int d = 0; d < biome.decorations.size(); d++) {
                DecorationSpecifier& decor = biome.decorations[d];
                
                if ((unsigned int)Random.Range(0, 100000) > decor.density) 
                    continue;
                
                // Perlin generation
                if (Random.Perlin(xx * decor.noise, 0, zz * decor.noise, chunk->seed) < decor.threshold) 
                    continue;
                
                float height = 0.0f;
                glm::vec3 from(staticX, 0, staticZ);
                
                Hit hit;
                if (Physics.Raycast(from, glm::vec3(0, -1, 0), 2000.0f, hit, LayerMask::Ground)) 
                    height = hit.point.y;
                
                if (height < decor.spawnHeightMinimum || height > decor.spawnHeightMaximum) 
                    continue;
                
                switch (decor.type) {
                    
                case DecorationType::Grass:      AddDecorGrass(chunk, staticMesh, glm::vec3(-xp, height, -zp), decor.name); break;
                case DecorationType::Tree:       AddDecorTree(chunk, staticMesh, glm::vec3(-xp, height, -zp), decor.name); break;
                case DecorationType::Actor:      AddDecoreActor(glm::vec3(from.x, height, from.z), decor.name); break;
                case DecorationType::Structure:  AddDecorStructure(chunk, staticMesh, glm::vec3(-xp, height, -zp), decor.name); break;
                    default: break;
                }
                
            }
            
            
            
        }
        
    }
    
    staticMesh->Load();
    return;
}


void ChunkManager::AddDecorGrass(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    float grassWidth = 0.0f;
    float grassHeight = 0.0f;
    
    Color colorMin = Colors.white;
    Color colorMax = Colors.white;
    
    for (std::unordered_map<std::string, DefinitionTypeGrass>::iterator it = world.definitionGrass.begin(); it != world.definitionGrass.end(); ++it) {
        if (it->first != name) 
            continue;
        DefinitionTypeGrass& definition = it->second;
        
        grassWidth  = definition.width;
        grassHeight = definition.height;
        colorMin    = definition.colorMin;
        colorMax    = definition.colorMax;
        break;
    }
    if (grassWidth == 0.0f && grassHeight == 0.0f) 
        return;
    
    Color finalColor = Colors.Range(colorMin, colorMax);
    glm::vec3 scale(grassWidth, grassHeight, grassWidth);
    glm::vec3 rotation(0.0f, 0.0f, Random.Range(0, 360));
    
    unsigned int beginningIndex = chunk->statics.size();
    AddDecor(chunk, beginningIndex, staticMesh, DecorationMesh::Cross, position + glm::vec3(0.0f, grassHeight * 0.16f, 0.0f), rotation, scale, finalColor);
}

void ChunkManager::AddDecorTree(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    float heightMin = 0.0f;
    float heightMax = 0.0f;
    
    unsigned int numberOfLeavesMin = 0;
    unsigned int numberOfLeavesMax = 0;
    
    float leafSpreadArea  = 0.0f;
    float leafSpreadHeight = 0.0f;
    float leafWidth = 0.0f;
    float leafHeight = 0.0f;
    Color leafColorMin;
    Color leafColorMax;
    
    float trunkSize = 0.0f;
    Color trunkColorMin;
    Color trunkColorMax;
    
    for (std::unordered_map<std::string, DefinitionTypeTree>::iterator it = world.definitionTree.begin(); it != world.definitionTree.end(); ++it) {
        if (it->first != name) 
            continue;
        DefinitionTypeTree& definition = it->second;
        
        heightMin = definition.heightMin;
        heightMax = definition.heightMax;
        
        numberOfLeavesMin = definition.leafCountMin;
        numberOfLeavesMax = definition.leafCountMax;
        
        leafSpreadArea    = definition.leafSpreadArea;
        leafSpreadHeight  = definition.leafSpreadHeight;
        leafWidth         = definition.leafWidth;
        leafHeight        = definition.leafHeight;
        leafColorMin      = definition.leafColorMin;
        leafColorMax      = definition.leafColorMax;
        
        trunkSize         = definition.trunkSize;
        trunkColorMin     = definition.trunkColorMin;
        trunkColorMax     = definition.trunkColorMax;
        break;
    }
    if (heightMax == 0.0f && heightMin == 0.0f) 
        return;
    
    float height = Random.Range(heightMin, heightMax);
    
    glm::vec3 scale(trunkSize, height, trunkSize);
    glm::vec3 rotation(0.0f, 0.0f, 0.0f);
    
    unsigned int beginningIndex = chunk->statics.size();
    
    Color trunkColor = Colors.Range(trunkColorMin, trunkColorMax);
    AddDecor(chunk, beginningIndex, staticMesh, DecorationMesh::Cross, glm::vec3(position.x, position.y + (height * 0.25f), position.z), rotation, scale, trunkColor);
    
    unsigned int numberOfLeaves = Random.Range(numberOfLeavesMin, numberOfLeavesMax);
    for (unsigned int i=0; i < numberOfLeaves; i++) {
        glm::vec3 scale(leafWidth, leafHeight, leafWidth);
        glm::vec3 rotation(Random.Range(0, 360), Random.Range(0, 360), Random.Range(0, 360));
        
        float xx = Random.Range(0.0f, leafSpreadArea) - Random.Range(0.0f, leafSpreadArea);
        float yy = Random.Range(0.0f, leafSpreadHeight) - Random.Range(0.0f, leafSpreadHeight);
        float zz = Random.Range(0.0f, leafSpreadArea) - Random.Range(0.0f, leafSpreadArea);
        
        glm::vec3 offsetPosition = position + glm::vec3(xx, yy + (height * 0.5f), zz);
        
        Color leafColorTotal = Colors.Range(leafColorMin, leafColorMax);
        AddDecor(chunk, beginningIndex, staticMesh, DecorationMesh::WallHorizontal, offsetPosition, rotation, scale, leafColorTotal);
    }
}


void ChunkManager::AddDecorStructure(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    Structure* structure = nullptr;
    
    // Find the matching structure by name
    for (unsigned int i = 0; i < world.structures.size(); i++) {
        if (world.structures[i].name != name) 
            continue;
        structure = &world.structures[i];
        break;
    }
    if (structure == nullptr)
        return;
    
    // Check distance to nearby structures
    glm::vec3 structurePosition(position.x + chunk->x, position.y, position.z + chunk->y);
    for (unsigned int i=0; i < chunk->structureLocations.size(); i++) {
        glm::vec3 pos = chunk->structureLocations[i];
        
        if (glm::distance(pos, structurePosition) < structure->buildSpread) 
            return;
    }
    chunk->structureLocations.push_back(structurePosition);
    
    // Check chunk boundary
    glm::vec3 chunkPos(chunk->x, 0, chunk->y);
    if (glm::distance(chunkPos, structurePosition) > chunkSize * 0.6f) 
        return;
    
    // Place structure parts
    for (unsigned int p = 0; p < structure->parts.size(); p++) {
        StructurePart part = structure->parts[p];
        
        glm::vec3 translation = position + glm::vec3(part.offset.x, 0.0f, part.offset.y);
        glm::vec3 orientation = part.rotation;
        glm::vec3 scale = part.scale;
        Color color = part.color;
        
        AddDecor(chunk, chunk->statics.size(), staticMesh, DecorationMesh::Cross, translation, orientation, scale, color);
    }
    
    // Spawn actors
    if (structure->actorCountMin >= structure->actorCountMax) 
        return;
    if (structure->actorCountMax == 0) 
        return;
    
    unsigned int numberOfActors = Random.Range(structure->actorCountMin, structure->actorCountMax);
    
    for (int i = 0; i < numberOfActors; i++) {
        float offsetX = chunk->x + Random.Range(-structure->actorSpread, structure->actorSpread);
        float offsetZ = chunk->y + Random.Range(-structure->actorSpread, structure->actorSpread);
        
        Actor* actor = SummonActor(position + glm::vec3(offsetX, 0.0f, offsetZ));
        DecodeGenome("Human", actor);
        
        if (i < (numberOfActors / 2)) {
            actor->physical.SetSexualOrientation(true);
        } else {
            actor->physical.SetSexualOrientation(false);
        }
        
        actor->physical.SetAge(Random.Range(actor->physical.GetAdultAge(), actor->physical.GetAdultAge() * 2));
        actor->RebuildGeneticExpression();
        actor->isActive = true;
        actor->physical.UpdatePhysicalCollider();
    }
}


void ChunkManager::AddDecorMesh(Mesh* staticMesh, DecorationMesh type, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Color color) {
    unsigned int numberOfSubMeshes=0;
    switch (type) {
        case DecorationMesh::WallHorizontal: staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallHorz, false); numberOfSubMeshes = 1; break;
        case DecorationMesh::WallVerticle:   staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallVert, false); numberOfSubMeshes = 1; break;
        case DecorationMesh::Cross: numberOfSubMeshes = 2; 
            staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallHorz, false);
            staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallVert, false);
        break;
    }
    
    unsigned int indexMesh = staticMesh->GetSubMeshCount() - 1;
    for (unsigned int i=0; i < numberOfSubMeshes; i++) {
        staticMesh->ChangeSubMeshColor(indexMesh - i,   color);
        staticMesh->ChangeSubMeshScale(indexMesh - i,   scale.x, scale.y, scale.z);
        
        staticMesh->ChangeSubMeshRotation(indexMesh - i, rotation.x, glm::vec3(1.0f, 0, 0));
        staticMesh->ChangeSubMeshRotation(indexMesh - i, rotation.y, glm::vec3(0, 0, 1.0f));
        staticMesh->ChangeSubMeshRotation(indexMesh - i, rotation.z, glm::vec3(0, 1.0f, 0));
    }
}

unsigned int ChunkManager::AddDecor(Chunk* chunk, unsigned int index, Mesh* staticMesh, DecorationMesh type, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Color color) {
    AddDecorMesh(staticMesh, type, position, rotation, scale, color);
    
    StaticObject staticObject;
    staticObject.position = position;
    staticObject.rotation = rotation;
    staticObject.scale    = scale;
    staticObject.color    = color.ToVec3();
    staticObject.type     = type;
    
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
            if (chunk.statics[i].position != bestLocalPos) 
                continue;
            
            chunk.statics.erase(chunk.statics.begin() + i);
            
            unsigned int numberOfSubMeshes = 0;
            switch (chunk.statics[i].type) {
                
            case DecorationMesh::WallHorizontal:  numberOfSubMeshes = 1; break;
            case DecorationMesh::WallVerticle:    numberOfSubMeshes = 1; break;
            case DecorationMesh::Cross:           numberOfSubMeshes = 2; break;
            }
            
            // Remove both sub meshes
            for (unsigned int a=0; a < numberOfSubMeshes; a++) 
                mesh->RemoveSubMesh(bestSubMeshIndex);
            
            break;
        }
        
        mesh->Load();
    }
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

