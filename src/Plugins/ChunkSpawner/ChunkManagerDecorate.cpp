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
                
                // Use world-space for noise, not chunk-local loop coords
                float wx = staticX;   // world X
                float wz = staticZ;   // world Z
                
                auto hash2D = [](int x, int z, uint32_t seed) -> uint32_t {
                    uint32_t h = (uint32_t)x * 374761393u + (uint32_t)z * 668265263u + seed * 1274126177u;
                    h ^= h >> 13; h *= 1274126177u; h ^= h >> 16;
                    return h;
                };
                uint32_t h = hash2D((int)std::floor(wx), (int)std::floor(wz), (uint32_t)chunk->seed);
                
                if ((h % 100000u) > decor.density) continue;
                
                if (Random.Perlin(wx * decor.noise, 0.0f, wz * decor.noise, chunk->seed) < decor.threshold) 
                    continue;
                
                if (Random.Perlin(wx * decor.noise * 0.5f, 0.0f, wz * decor.noise * 0.5f, chunk->seed) < decor.threshold) 
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
                    
                }
                
            }
            
            
            
        }
        
    }
    
    staticMesh->Load();
    return;
}



void ChunkManager::AddDecor(Chunk* chunk, Mesh* staticMesh, DecorationMesh type, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Color color) {
    AddDecorMesh(staticMesh, type, position, rotation, scale, color);
    
    StaticObject staticObject;
    staticObject.position = position;
    staticObject.rotation = rotation;
    staticObject.scale    = scale;
    staticObject.color    = color.ToVec3();
    staticObject.type     = static_cast<unsigned int>(type);
    
    chunk->statics.push_back(staticObject);
}


void ChunkManager::AddDecorMesh(Mesh* staticMesh, DecorationMesh type, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale, Color color) {
    switch (type) {
        case DecorationMesh::WallHorizontal: staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallHorz, false); break;
        case DecorationMesh::WallVerticle:   staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshWallVert, false); break;
        case DecorationMesh::Plain:          staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshPlain, false); break;
        case DecorationMesh::Cross:          staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshCross, false); break;
        case DecorationMesh::Leaf:           staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshLeaf, false); break;
        case DecorationMesh::Log:            staticMesh->AddSubMesh(position.x, position.y, position.z, subMeshLog, false); break;
    }
    
    unsigned int indexMesh = staticMesh->GetSubMeshCount() - 1;
    staticMesh->ChangeSubMeshColor(indexMesh, color);
    
    staticMesh->ChangeSubMeshScale(indexMesh, scale.x, scale.y, scale.z);
    
    staticMesh->ChangeSubMeshRotation(indexMesh, rotation.x, glm::vec3(1, 0, 0));
    staticMesh->ChangeSubMeshRotation(indexMesh, rotation.y, glm::vec3(0, 1, 0));
    staticMesh->ChangeSubMeshRotation(indexMesh, rotation.z, glm::vec3(0, 0, 1));
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


bool ChunkManager::PlaceDecor(glm::vec3 position, glm::vec3 direction, DecorationType type, const std::string& name) {
    direction = glm::normalize(direction);
    
    const float threshold   = 0.7f;
    const float maxDistance = 10.0f;
    const float epsPush     = 0.02f;
    
    float bestDot = -1.0f;
    int bestChunkIndex = -1;
    int bestSubMeshIndex = -1;
    glm::vec3 bestLocalPos(0.0f);
    glm::vec3 bestHitPoint(0.0f);
    glm::vec3 bestHitNormal(0.0f);
    
    for (unsigned int i = 0; i < chunks.Size(); i++) {
        Chunk& c = *chunks[i];
        
        // Skip far chunks
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
            
            // Work out which face we hit
            glm::vec3 hitPoint = position + direction * hitT;
            glm::vec3 n(0.0f);
            const float faceEps = 0.001f;
            if      (std::abs(hitPoint.x - boxMin.x) < faceEps) n = glm::vec3(-1, 0, 0);
            else if (std::abs(hitPoint.x - boxMax.x) < faceEps) n = glm::vec3( 1, 0, 0);
            else if (std::abs(hitPoint.y - boxMin.y) < faceEps) n = glm::vec3( 0,-1, 0);
            else if (std::abs(hitPoint.y - boxMax.y) < faceEps) n = glm::vec3( 0, 1, 0);
            else if (std::abs(hitPoint.z - boxMin.z) < faceEps) n = glm::vec3( 0, 0,-1);
            else if (std::abs(hitPoint.z - boxMax.z) < faceEps) n = glm::vec3( 0, 0, 1);
            
            // Pick the best-aligned thing in front of us
            if (dot > bestDot) {
                bestDot = dot;
                bestChunkIndex = (int)i;
                bestSubMeshIndex = (int)s;
                bestLocalPos = sub.position;
                bestHitPoint = hitPoint;
                bestHitNormal = (glm::length(n) > 0.0f) ? glm::normalize(n) : glm::vec3(0,1,0);
            }
        }
    }
    
    if (bestChunkIndex != -1) {
        // We hit a static; place on that face
        Chunk& chunk = *chunks[bestChunkIndex];
        Mesh* staticMesh = chunk.staticObject->GetComponent<MeshRenderer>()->mesh;
        
        const float grid = 1.0f;
        const glm::vec3 gridOrigin(0.0f);
        
        glm::vec3 mountWorld = bestHitPoint;
        mountWorld = SnapAxes(mountWorld, AxesForFace(bestHitNormal), grid, gridOrigin);
        mountWorld += bestHitNormal * epsPush;
        
        glm::vec3 localPos(mountWorld.x - chunk.x, mountWorld.y, mountWorld.z - chunk.y);
        
        switch (type) {
            case DecorationType::Grass:
                AddDecorGrass(&chunk, staticMesh, localPos, name);
                staticMesh->Load();
                return true;
            
            case DecorationType::Tree:
                AddDecorTree(&chunk, staticMesh, localPos, name);
                staticMesh->Load();
                return true;
            
            case DecorationType::Structure:
                AddDecorStructure(&chunk, staticMesh, localPos, name);
                staticMesh->Load();
                return true;
            
            case DecorationType::Actor:
                AddDecoreActor(mountWorld, name);
                return true;
        }
        return false;
    } else {
        
        // Place on ground where we’re looking at
        Hit groundHit;
        const float kMaxPlaceDistance = 2000.0f;
        if (!Physics.Raycast(position, direction, kMaxPlaceDistance, groundHit, LayerMask::Ground)) {
            glm::vec3 probe = position + direction * 10.0f;
            if (!Physics.Raycast(glm::vec3(probe.x, probe.y + 1000.0f, probe.z),
                                 glm::vec3(0, -1, 0), 2000.0f, groundHit, LayerMask::Ground)) {
                return false;
            }
        }
        
        const float grid = 1.0f;
        const glm::vec3 gridOrigin(0.0f);
        
        glm::vec3 worldHit = groundHit.point;
        // Snap on XZ for ground; keep Y from the raycast
        worldHit = SnapAxes(worldHit, glm::bvec3(true, false, true), grid, gridOrigin);
        
        // Pick the nearest chunk to the hit point (tighten scope versus camera position)
        int bestIdx = -1;
        float bestDist = 1e9f;
        for (unsigned int i = 0; i < chunks.Size(); i++) {
            glm::vec3 cpos(chunks[i]->x, 0.0f, chunks[i]->y);
            float d = glm::distance(cpos, worldHit);
            if (d < bestDist && d <= chunkSize) {
                bestDist = d;
                bestIdx = (int)i;
            }
        }
        if (bestIdx == -1) return false;
        
        Chunk& chunk = *chunks[bestIdx];
        Mesh* staticMesh = chunk.staticObject->GetComponent<MeshRenderer>()->mesh;
        
        glm::vec3 localPos(worldHit.x - chunk.x, worldHit.y, worldHit.z - chunk.y);
        
        switch (type) {
            case DecorationType::Grass:
                AddDecorGrass(&chunk, staticMesh, localPos, name);
                staticMesh->Load();
                return true;
            case DecorationType::Tree:
                AddDecorTree(&chunk, staticMesh, localPos, name);
                staticMesh->Load();
                return true;
            case DecorationType::Structure:
                AddDecorStructure(&chunk, staticMesh, localPos, name);
                staticMesh->Load();
                return true;
            case DecorationType::Actor:
                AddDecoreActor(worldHit, name);
                return true;
        }
    }
    
    return false;
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
            
            mesh->RemoveSubMesh(bestSubMeshIndex);
            break;
        }
        
        mesh->Load();
    }
}

DecorationHitInfo ChunkManager::QueryDecor(glm::vec3 position, glm::vec3 direction, float maxDistance) {
    DecorationHitInfo result;
    direction = glm::normalize(direction);
    
    const float threshold = 0.7f;
    float bestDot = -1.0f;
    
    for (unsigned int i = 0; i < chunks.Size(); i++) {
        Chunk& c = *chunks[i];
        
        // Small optimization: skip chunks far from camera
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
            
            glm::vec3 aabbScale = glm::vec3(2.0f); // match your RemoveDecor hitbox size
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
                    
                    result.didHit = true;
                    result.worldPosition = worldPos;
                    result.hitPoint = hitPoint;
                    result.normal = n;
                    
                    result.type = static_cast<DecorationMesh>(object.type);
                    break;
                }
            }
        }
    }
    
    return result;
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

