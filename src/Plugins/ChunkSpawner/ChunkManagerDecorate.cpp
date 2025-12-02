#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

float Snap1D(float v, float grid, float origin = 0.0f);
glm::vec3 SnapAxes(glm::vec3 p, glm::bvec3 axes, float grid, glm::vec3 origin = glm::vec3(0.0f));
glm::bvec3 AxesForFace(const glm::vec3& n);


void ChunkManager::Decorate(Chunk* chunk) {
    unsigned int numberOfBiomes = world.biomes.size();
    if (numberOfBiomes == 0 || chunk->biomeMap.empty())
        return;
    
    Mesh* staticMesh = chunk->staticObject->GetComponent<MeshRenderer>()->mesh;
    chunk->statics.clear();
    staticMesh->ClearSubMeshes();
    
    const float grid         = 1.0f;
    const glm::vec3 gridOrigin(0.5f, 0.5f, 0.5f);
    
    unsigned int chunkSZ = chunkSize + 1;
    
    uint8_t placementGrid[chunkSize][chunkSize];
    ZeroMemory(placementGrid, chunkSize * chunkSize);
    
    for (int xx = 0; xx < chunkSize; xx++) {
        float xp = xx - (chunkSize / 2) + 0.5f;
        float staticX = chunk->x - xp;
        
        for (int zz = 0; zz < chunkSize; zz++) {
            float zp = zz - (chunkSize / 2) + 0.5f;
            float staticZ = chunk->y - zp;
            int index = zz * chunkSZ + xx;
            
            // Find which biome dominates here
            int biomeIndex = chunk->biomeMap[index];
            if (biomeIndex < 0 || biomeIndex >= (int)world.biomes.size())
                continue;
            Biome& biome = world.biomes[biomeIndex];
            
            // Loop over the biome's decorations
            for (unsigned int d = 0; d < biome.decorations.size(); d++) {
                DecorationSpecifier& decoration = biome.decorations[d];
                if (placementGrid[xx][zz] == 0xff) 
                    continue;
                
                // Use world-space for noise, not chunk-local loop coords
                float wx = Snap1D(staticX, grid, gridOrigin.x);
                float wz = Snap1D(staticZ, grid, gridOrigin.z);
                
                if (Random.Range(1, 1000) > decoration.density) 
                    continue;
                
                if (Random.Perlin(wx * decoration.noise, 0.0f, wz * decoration.noise, chunk->seed) < decoration.threshold) 
                    continue;
                
                if (Random.Perlin(wx * decoration.noise * 0.5f, 0.0f, wz * decoration.noise * 0.5f, chunk->seed) < decoration.threshold) 
                    continue;
                
                float height = 0.0f;
                glm::vec3 from(staticX, 0, staticZ);
                
                Hit hit;
                if (Physics.Raycast(from, glm::vec3(0, -1, 0), 2000.0f, hit, LayerMask::Ground)) 
                    height = hit.point.y;
                
                height = Snap1D(height, grid, gridOrigin.y);
                
                if (height < decoration.spawnHeightMinimum || height > decoration.spawnHeightMaximum) 
                    continue;
                
                glm::vec3 position(-xp, height, -zp);
                
                // Special generation
                if (decoration.name.find("oak_tree") != std::string::npos) {
                    
                    BuildDecorStructure(chunk, position, "oak_tree");
                    
                    placementGrid[xx][zz] = 0xff;
                    continue;
                }
                
                // Place static
                if (world.classDefinitions.find(decoration.name) != world.classDefinitions.end()) {
                    ClassDefinition& definition = world.classDefinitions[decoration.name];
                    if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
                        continue;
                    
                    AddDecor(chunk, definition.mesh, decoration.name, position, glm::vec3(0.0f));
                    placementGrid[xx][zz] = 0xff;
                    
                    continue;
                }
                
            }
            
        }
        
    }
    
    staticMesh->Load();
}


void ChunkManager::AddDecor(Chunk* chunk, const std::string& mesh, const std::string& type, glm::vec3 position, glm::vec3 rotation) {
    ClassDefinition definition = world.classDefinitions[type];
    
    Color color = Colors.Lerp(definition.colorMax, definition.colorMin, 0.5f);
    glm::vec3 scale(definition.width, definition.height, definition.width);
    
    if (mStaticMeshes.find(mesh) == mStaticMeshes.end()) 
        return;
    
    Mesh* staticMesh = chunk->staticObject->GetComponent<MeshRenderer>()->mesh;
    
    SubMesh& subMesh = mStaticMeshes[mesh];
    staticMesh->AddSubMesh(position.x, position.y, position.z, subMesh, false);
    
    unsigned int indexMesh = staticMesh->GetSubMeshCount() - 1;
    staticMesh->ChangeSubMeshColor(indexMesh, color);
    
    staticMesh->ChangeSubMeshScale(indexMesh, scale.x, scale.y, scale.z);
    
    staticMesh->ChangeSubMeshRotation(indexMesh, rotation.x, glm::vec3(1, 0, 0));
    staticMesh->ChangeSubMeshRotation(indexMesh, rotation.y, glm::vec3(0, 1, 0));
    staticMesh->ChangeSubMeshRotation(indexMesh, rotation.z, glm::vec3(0, 0, 1));
    
    // Add static object to the chunk list
    StaticObject staticObject;
    staticObject.position = position;
    staticObject.rotation = rotation;
    staticObject.scale    = scale;
    staticObject.color    = color.ToVec3();
    staticObject.mesh     = mStaticMeshToIndex[mesh];
    staticObject.type     = world.classNameToIndex[type];
    
    chunk->statics.push_back(staticObject);
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

DecorationHitInfo ChunkManager::QueryDecor(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold) {
    DecorationHitInfo result;
    direction = glm::normalize(direction);
    float bestDot = -1.0f;
    
    for (unsigned int i = 0; i < chunks.Size(); i++) {
        Chunk& c = *chunks[i];
        
        glm::vec3 chunkPos(c.x, 0.0f, c.y);
        if (glm::distance(chunkPos, position) > chunkSize)
            continue;
        
        Mesh* mesh = c.staticObject->GetComponent<MeshRenderer>()->mesh;
        unsigned int subCount = mesh->GetSubMeshCount();
        
        for (unsigned int s = 0; s < subCount; ++s) {
            SubMesh sub;
            mesh->GetSubMesh(s, sub);
            
            glm::vec3 worldPos = sub.position + glm::vec3(c.x, 0.0f, c.y);
            
            if (glm::distance(worldPos, position) > maxDistance)
                continue;
            
            glm::vec3 aabbScale = glm::vec3(2.0f);
            glm::vec3 boxMin = worldPos - (aabbScale * 0.5f);
            glm::vec3 boxMax = worldPos + (aabbScale * 0.5f);
            
            float hitT = 0.0f;
            if (!RayIntersectsAABB(position, direction, boxMin, boxMax, hitT))
                continue;
            if (hitT > maxDistance)
                continue;
            
            glm::vec3 toObject = glm::normalize(worldPos - position);
            float dot = glm::dot(toObject, direction);
            if (dot < threshold)
                continue;
            
            glm::vec3 hitPoint = position + direction * hitT;
            glm::vec3 n(0.0f);
            const float faceEps = 0.001f;
            if      (std::abs(hitPoint.x - boxMin.x) < faceEps) n = glm::vec3(-1, 0, 0);
            else if (std::abs(hitPoint.x - boxMax.x) < faceEps) n = glm::vec3( 1, 0, 0);
            else if (std::abs(hitPoint.y - boxMin.y) < faceEps) n = glm::vec3( 0,-1, 0);
            else if (std::abs(hitPoint.y - boxMax.y) < faceEps) n = glm::vec3( 0, 1, 0);
            else if (std::abs(hitPoint.z - boxMin.z) < faceEps) n = glm::vec3( 0, 0,-1);
            else if (std::abs(hitPoint.z - boxMax.z) < faceEps) n = glm::vec3( 0, 0, 1);
            
            // Found a better-aligned hit
            if (dot > bestDot) {
                bestDot = dot;
                result.didHit = false;
                
                // Find matching Decoration in chunk.statics to get type/name
                for (const StaticObject& object : c.statics) {
                    if (object.position != sub.position) 
                        continue;
                    
                    result.didHit         = true;
                    result.worldPosition  = worldPos;
                    result.hitPoint       = hitPoint;
                    result.normal         = n;
                    result.rotation       = object.rotation;
                    result.scale          = object.scale;
                    
                    result.mesh  = mStaticIndexToMesh[object.mesh];
                    result.type  = world.classIndexToName[object.type];
                    break;
                }
            }
        }
    }
    
    return result;
}


bool ChunkManager::RemoveDecor(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold) {
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
            
            mesh->RemoveSubMesh(bestSubMeshIndex);
            
            mesh->Load();
            return true;
        }
    }
    return false;
}

bool ChunkManager::PlaceDecor(glm::vec3 position, glm::vec3 direction, const std::string& name, float maxDistance, float threshold) {
    std::unordered_map<std::string, ClassDefinition>::iterator itClass = world.classDefinitions.find(name);
    if (itClass == world.classDefinitions.end())
        return false;
    
    const ClassDefinition& definition = itClass->second;
    auto itMesh = mStaticMeshes.find(definition.mesh);
    if (itMesh == mStaticMeshes.end()) 
        return false;
    
    glm::vec3 cameraDir = glm::normalize(direction);
    
    const float grid = 1.0f;
    const glm::vec3 gridOrigin(0.5f, 0.5f, 0.5f);
    
    float bestDot = -1.0f;
    int   bestChunkIndex   = -1;
    int   bestSubMeshIndex = -1;
    glm::vec3 bestHitPoint(0.0f);
    glm::vec3 bestHitNormal(0.0f);
    glm::vec3 bestSubLocalPos(0.0f);
    
    for (unsigned int ci = 0; ci < chunks.Size(); ++ci) {
        Chunk& chunk = *chunks[ci];
        glm::vec3 chunkWorld(chunk.x, 0.0f, chunk.y);
        
        if (glm::distance(chunkWorld, position) > chunkSize)
            continue;
        
        Mesh* mesh = chunk.staticObject->GetComponent<MeshRenderer>()->mesh;
        const unsigned subCount = mesh->GetSubMeshCount();
        
        for (unsigned s = 0; s < subCount; ++s) {
            SubMesh sub; mesh->GetSubMesh(s, sub);
            
            glm::vec3 centerWorld = sub.position + chunkWorld;
            if (glm::distance(centerWorld, position) > maxDistance)
                continue;
            
            const glm::vec3 half(0.5f);
            glm::vec3 boxMin = centerWorld - half;
            glm::vec3 boxMax = centerWorld + half;
            
            float t = 0.0f;
            if (!RayIntersectsAABB(position, cameraDir, boxMin, boxMax, t))
                continue;
            if (t > maxDistance)
                continue;
            
            glm::vec3 toObj = glm::normalize(centerWorld - position);
            float dot = glm::dot(toObj, cameraDir);
            if (dot < threshold)
                continue;
            
            glm::vec3 hitPoint = position + cameraDir * t;
            glm::vec3 n(0.0f);
            const float faceEps = 0.001f;
            if      (std::abs(hitPoint.x - boxMin.x) < faceEps) n = glm::vec3(-1, 0, 0);
            else if (std::abs(hitPoint.x - boxMax.x) < faceEps) n = glm::vec3( 1, 0, 0);
            else if (std::abs(hitPoint.y - boxMin.y) < faceEps) n = glm::vec3( 0,-1, 0);
            else if (std::abs(hitPoint.y - boxMax.y) < faceEps) n = glm::vec3( 0, 1, 0);
            else if (std::abs(hitPoint.z - boxMin.z) < faceEps) n = glm::vec3( 0, 0,-1);
            else if (std::abs(hitPoint.z - boxMax.z) < faceEps) n = glm::vec3( 0, 0, 1);
            
            if (dot > bestDot) {
                bestDot          = dot;
                bestChunkIndex   = (int)ci;
                bestSubMeshIndex = (int)s;
                bestHitPoint     = hitPoint;
                bestHitNormal    = (glm::length(n) > 0.0f) ? glm::normalize(n) : glm::vec3(0,1,0);
                bestSubLocalPos  = sub.position;
            }
        }
    }
    
    const auto placeStatic = [&](Chunk& chunk, const glm::vec3& worldPos) -> bool {
        glm::vec3 localPos(worldPos.x - chunk.x, worldPos.y, worldPos.z - chunk.y);
        
        Color color = Colors.Range(definition.colorMin, definition.colorMax);
        glm::vec3 scale(definition.width, definition.height, definition.width);
        glm::vec3 rotation(0.0f);
        
        AddDecor(&chunk, definition.mesh, world.classIndexToName[definition.id], localPos, rotation);
        Mesh* staticMesh = chunk.staticObject->GetComponent<MeshRenderer>()->mesh;
        staticMesh->Load();
        return true;
    };
    
    if (bestChunkIndex != -1) {
        Chunk& chunk = *chunks[bestChunkIndex];
        
        glm::vec3 hitBlockWorld = bestSubLocalPos + glm::vec3(chunk.x, 0.0f, chunk.y);
        hitBlockWorld = SnapAxes(hitBlockWorld, glm::bvec3(true, true, true), grid, gridOrigin);
        
        glm::vec3 placeWorld = hitBlockWorld + bestHitNormal * grid;
        glm::bvec3 axes = AxesForFace(bestHitNormal);
        placeWorld = SnapAxes(placeWorld, axes, grid, gridOrigin);
        
        return placeStatic(chunk, placeWorld);
    }
    
    // No static object was hit, check ground
    Hit groundHit;
    if (!Physics.Raycast(position, cameraDir, maxDistance, groundHit, LayerMask::Ground)) 
        return false;
    
    glm::vec3 worldHit = groundHit.point;
    worldHit = SnapAxes(worldHit, glm::bvec3(true, false, true), grid, gridOrigin);
    
    int bestIdx = -1;
    float bestDist = 1e9f;
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        glm::vec3 cpos(chunks[i]->x, 0.0f, chunks[i]->y);
        float d = glm::distance(cpos, worldHit);
        if (d < bestDist && d <= chunkSize) {
            bestDist = d;
            bestIdx = (int)i;
        }
    }
    if (bestIdx == -1)
        return false;
    
    return placeStatic(*chunks[bestIdx], worldHit);
}





void ChunkManager::DecodeGenome(std::string name, Actor* actorPtr) {
    if (name == "Human")  {AI.genomes.presets.Human( actorPtr );}
    if (name == "Sheep")  {AI.genomes.presets.Sheep( actorPtr );}
    if (name == "Bear")   {AI.genomes.presets.Bear( actorPtr );}
    if (name == "Dog")    {AI.genomes.presets.Dog( actorPtr );}
    if (name == "Bovine") {AI.genomes.presets.Bovine( actorPtr );}
    if (name == "Horse")  {AI.genomes.presets.Horse( actorPtr );}
}

/*
bool ChunkManager::AddDecoreActor(glm::vec3 position, std::string name) {
    Actor* actor = SummonActor(position);
    DecodeGenome(name, actor);
    
    actor->isActive = true;
    actor->RebuildGeneticExpression();
    
    actor->state.current = ActorState::State::None;
    actor->state.mode = ActorState::Mode::Idle;
    
    actor->physical.UpdatePhysicalCollider();
    actor->physical.SetAge( actor->physical.GetAdultAge() + Random.Range(0, 1000) );
    return true;
}
*/


float Snap1D(float v, float grid, float origin) {
    return origin + std::round((v - origin) / grid) * grid;
}

glm::vec3 SnapAxes(glm::vec3 p, glm::bvec3 axes, float grid, glm::vec3 origin) {
    if (axes.x) p.x = Snap1D(p.x, grid, origin.x);
    if (axes.y) p.y = Snap1D(p.y, grid, origin.y);
    if (axes.z) p.z = Snap1D(p.z, grid, origin.z);
    return p;
}

// Choose which axes to snap based on the face normal we’re placing onto.
//  - Up/Down face => snap XZ (ground-like)
//  - +/-X face    => snap YZ (vertical wall)
//  - +/-Z face    => snap XY (vertical wall)
glm::bvec3 AxesForFace(const glm::vec3& n) {
    glm::vec3 an = glm::abs(n);
    if (an.y >= an.x && an.y >= an.z) return glm::bvec3(true,  false, true ); // XZ
    if (an.x >= an.y && an.x >= an.z) return glm::bvec3(false, true,  true ); // YZ
                                      return glm::bvec3(true,  true,  false); // XY
}
