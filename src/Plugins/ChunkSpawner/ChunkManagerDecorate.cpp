#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

float Snap1D(float v, float grid, float origin = 0.0f);
glm::bvec3 AxesForFace(const glm::vec3& n);

void ChunkManager::Decorate(Chunk* chunk) {
    if (chunk->heightField == nullptr) 
        return;
    
    unsigned int numberOfBiomes = world.biomes.size();
    if (numberOfBiomes == 0 || chunk->biomeMap.empty())
        return;
    
    Mesh* staticMesh = chunk->staticObject->GetComponent<MeshRenderer>()->mesh;
    chunk->statics.clear();
    staticMesh->ClearSubMeshes();
    
    const float grid         = 1.0f;
    const glm::vec3 gridOrigin(0.5f, 0.5f, 0.5f);
    
    const int cs = this->chunkSize;
    unsigned int chunkSZ = (unsigned int)cs + 1;
    
    // Placement tracking
    std::vector<uint8_t> placementGrid((size_t)cs * (size_t)cs, 0);
    
    auto GridAt = [&placementGrid, cs](int gx, int gz) -> uint8_t& {
        return placementGrid[(size_t)gz * (size_t)cs + (size_t)gx];
    };
    
    auto MarkPlacement = [&GridAt, cs](const glm::vec3& localPos) {
        const float half = (float)cs * 0.5f;
        const float eps  = 0.0001f;
        int gx = (int)std::floor(localPos.x + half + eps);
        int gz = (int)std::floor(localPos.z + half + eps);
        if (gx < 0 || gx >= cs) return;
        if (gz < 0 || gz >= cs) return;
        GridAt(gx, gz) = 0xff;
    };
    
    for (int xx = 0; xx < chunkSize; xx++) {
        // Local X increases from min (-cs/2) to max (+cs/2)
        float localX = (float)xx - ((float)chunkSize * 0.5f) + 0.5f;
        float staticX = chunk->x + localX;
        
        for (int zz = 0; zz < chunkSize; zz++) {
            // Local Z increases from min (-cs/2) to max (+cs/2)
            float localZ = (float)zz - ((float)chunkSize * 0.5f) + 0.5f;
            float staticZ = chunk->y + localZ;
            int index = zz * chunkSZ + xx;
            
            // Find which biome dominates here
            int biomeIndex = chunk->biomeMap[index];
            if (biomeIndex < 0 || biomeIndex >= (int)world.biomes.size())
                continue;
            Biome& biome = world.biomes[biomeIndex];
            
            // Generate biome decorations
            for (unsigned int d = 0; d < biome.decorations.size(); d++) {
                DecorationSpecifier& decoration = biome.decorations[d];
                if (GridAt(xx, zz) == 0xff) 
                    continue;
                
                // Use world-space for noise
                float wx = Snap1D(staticX, grid, gridOrigin.x);
                float wz = Snap1D(staticZ, grid, gridOrigin.z);
                
                if (Random.Range(1, 1000) > decoration.density) 
                    continue;
                
                if (Random.Perlin(wx * decoration.noise, 0.0f, wz * decoration.noise, chunk->seed) < decoration.threshold) 
                    continue;
                
                if (Random.Perlin(wx * decoration.noise * 0.5f, 0.0f, wz * decoration.noise * 0.5f, chunk->seed) < decoration.threshold) 
                    continue;
                
                // Direct HeightField Lookup
                float h00 = chunk->heightField[zz * chunkSZ + xx];
                float h10 = chunk->heightField[zz * chunkSZ + (xx + 1)];
                float h01 = chunk->heightField[(zz + 1) * chunkSZ + xx];
                float h11 = chunk->heightField[(zz + 1) * chunkSZ + (xx + 1)];
                float height = (h00 + h10 + h01 + h11) * 0.25f;
                
                if (height < decoration.spawnHeightMinimum || height > decoration.spawnHeightMaximum) 
                    continue;
                
                glm::vec3 position(localX, height, localZ);
                
                // Check actor
                if (world.classActors.find(decoration.name) != world.classActors.end()) {
                    const ClassActor& definition = world.classActors[decoration.name];
                    if (definition.genome == "") 
                        continue;
                    
                    Actor* actor = SummonActor(glm::vec3(chunk->x, 0.0f, chunk->y) + position);
                    
                    AI.genomes.InjectGenome(actor, definition.genome);
                    
                    if (Random.Range(0, 100) > 50) {
                        actor->physical.SetSexualOrientation(false); // Female
                    } else {
                        actor->physical.SetSexualOrientation(true);  // Male
                    }
                    
                    // Set default age to adult
                    actor->physical.SetAge( actor->physical.GetAdultAge() );
                    
                    actor->RebuildGeneticExpression();
                    actor->isActive = true;
                    
                    MarkPlacement(position);
                    continue;
                }
                
                // Check structure object
                float structuralHeightMax = 0;
                
                if (world.classStructures.find(decoration.name) != world.classStructures.end()) {
                    const ClassStructure& structure = world.classStructures[decoration.name];
                    
                    // Stack
                    for (unsigned int s=0; s < structure.stacks.size(); s++) {
                        const ClassStructure::SubStructureStack& stack = structure.stacks[s];
                        ClassDefinition& definition = world.classDefinitions[stack.name];
                        if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
                            continue;
                        
                        unsigned int range = 0;
                        if (stack.heightMax <= stack.heightMin) {
                            range = stack.heightMax;
                        } else {
                            range = Random.Range(stack.heightMin, stack.heightMax);
                        }
                        
                        for (unsigned int h=0; h < range; h++) {
                            glm::vec3 offset = position + stack.position + glm::vec3(0, h, 0);
                            
                            if (h > structuralHeightMax) 
                                structuralHeightMax = h;
                            
                            AddDecor(chunk, definition.mesh, stack.name, offset, stack.rotation);
                            MarkPlacement(position);
                        }
                    }
                    
                    // Place
                    for (unsigned int s=0; s < structure.places.size(); s++) {
                        const ClassStructure::SubStructurePlace& place = structure.places[s];
                        ClassDefinition& definition = world.classDefinitions[place.name];
                        if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
                            continue;
                        
                        glm::vec3 offset = position + place.position;
                        
                        if (definition.alignment == 1) 
                            offset.y = Snap1D(offset.y, grid, gridOrigin.y);
                        
                        AddDecor(chunk, definition.mesh, place.name, offset, place.position);
                        
                        MarkPlacement(position);
                    }
                    
                    // Fill
                    for (unsigned int s = 0; s < structure.fills.size(); ++s) {
                        const ClassStructure::SubStructureFill& fill = structure.fills[s];
                        
                        std::unordered_map<std::string, ClassDefinition>::iterator itDef =
                            world.classDefinitions.find(fill.name);
                        if (itDef == world.classDefinitions.end())
                            continue;
                        
                        ClassDefinition& definition = itDef->second;
                        if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end())
                            continue;
                        
                        glm::vec3 from = position + fill.from;
                        glm::vec3 to   = position + fill.to;
                        
                        glm::vec3 mn(glm::min(from.x, to.x), glm::min(from.y, to.y), glm::min(from.z, to.z));
                        glm::vec3 mx(glm::max(from.x, to.x), glm::max(from.y, to.y), glm::max(from.z, to.z));
                        
                        // Inclusive integer grid bounds
                        int x0 = (int)std::floor(mn.x);
                        int y0 = (int)std::floor(mn.y);
                        int z0 = (int)std::floor(mn.z);
                        
                        int x1 = (int)std::floor(mx.x);
                        int y1 = (int)std::floor(mx.y);
                        int z1 = (int)std::floor(mx.z);
                        
                        for (int z = z0; z <= z1; ++z) {
                            for (int y = y0; y <= y1; ++y) {
                                for (int x = x0; x <= x1; ++x) {
                                    glm::vec3 offset((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f);
                                    AddDecor(chunk, definition.mesh, fill.name, offset, glm::vec3(0.0f));
                                    
                                    MarkPlacement(offset);
                                }
                            }
                        }
                    }
                    
                    
                    // Pattern
                    for (unsigned int s=0; s < structure.patterns.size(); s++) {
                        const ClassStructure::SubStructurePattern& pattern = structure.patterns[s];
                        glm::vec3 offset = position + pattern.position + glm::vec3(0, structuralHeightMax, 0);
                        
                        BuildDecorStructure(chunk, offset, pattern.pattern, pattern.name, pattern.mesh);
                    }
                    
                    GridAt(xx, zz) = 0xff;
                }
                
                // Check static object
                if (world.classDefinitions.find(decoration.name) != world.classDefinitions.end()) {
                    ClassDefinition& definition = world.classDefinitions[decoration.name];
                    
                    // Allow meshless objects (empty or "none") to pass through
                    bool isMeshless = definition.mesh.empty() || definition.mesh == "none";
                    if (!isMeshless && mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
                        continue;
                    
                    glm::vec3 offset = position;
                    
                    if (definition.alignment == 1) 
                        offset.y = Snap1D(offset.y, grid, gridOrigin.y);
                    
                    AddDecor(chunk, definition.mesh, decoration.name, offset, glm::vec3(0.0f));
                    GridAt(xx, zz) = 0xff;
                    
                    continue;
                }
                
            }
            
        }
        
    }
    
    staticMesh->Load();
}

void ChunkManager::AddDecor(Chunk* chunk, const std::string& mesh, const std::string& type, 
                           const glm::vec3& position, const glm::vec3& rotation, glm::vec3 scale, glm::vec3 color, int function) {
    ClassDefinition definition = world.classDefinitions[type];
    unsigned short finalFunction = (function != -1) ? static_cast<unsigned int>(function) : definition.function;
    
    bool isNewSpawn = (scale == glm::vec3(0.0f));
    
    glm::vec3 finalScale;
    if (!isNewSpawn) {
        finalScale = scale;
    } else {
        finalScale = glm::vec3(definition.width, definition.height, definition.width);
        if (finalFunction != 0 && finalFunction != 5) {
            finalScale.y = definition.height * 0.25f; 
        }
    }
    
    Color finalColor;
    if (color.x >= 0.0f) {
        finalColor = Color(color.x, color.y, color.z);
    } else if (finalFunction != 0 && finalFunction != 5) {
        finalColor = definition.colorMin;
    } else {
        finalColor = Colors.Range(definition.colorMin, definition.colorMax);
    }
    
    // Check if object has a valid mesh
    bool hasMesh = !mesh.empty() && mesh != "none" && mStaticMeshes.find(mesh) != mStaticMeshes.end();
    
    if (hasMesh) {
        Mesh* staticMesh = chunk->staticObject->GetComponent<MeshRenderer>()->mesh;
        SubMesh& subMesh = mStaticMeshes[mesh];
        staticMesh->AddSubMesh(position.x, position.y, position.z, subMesh, false);
        
        unsigned int indexMesh = staticMesh->GetSubMeshCount() - 1;
        staticMesh->ChangeSubMeshColor(indexMesh, finalColor);
        staticMesh->ChangeSubMeshScale(indexMesh, finalScale.x, finalScale.y, finalScale.z);
        staticMesh->ChangeSubMeshRotation(indexMesh, rotation.x, glm::vec3(1, 0, 0));
        staticMesh->ChangeSubMeshRotation(indexMesh, rotation.y, glm::vec3(0, 1, 0));
        staticMesh->ChangeSubMeshRotation(indexMesh, rotation.z, glm::vec3(0, 0, 1));
        
        staticMesh->ChangeSubMeshNormalsAdditive(indexMesh, glm::vec3(0.0f, 0.5f, 0.0f));
    }
    
    StaticObject staticObject;
    staticObject.position = position;
    staticObject.rotation = rotation;
    staticObject.scale    = finalScale;
    staticObject.color    = finalColor.ToVec3();
    staticObject.mesh     = hasMesh ? mStaticMeshToIndex[mesh] : 0;
    staticObject.type     = world.classNameToIndex[type];
    staticObject.function = finalFunction;
    
    chunk->statics.push_back(staticObject);
    
    if (staticObject.function != 0 && staticObject.function != 5) {
        StaticAnimation ref;
        ref.staticIndex = chunk->statics.size() - 1;
        chunk->animatedStatics.push_back(ref);
    }
    
    //
    // TODO split this off elsewhere
    //
    // Spawn fire and smoke particle system emitters for function 5
    if (finalFunction == 5) {
        glm::vec3 worldPos = position + glm::vec3(chunk->x, 0.0f, chunk->y);
        
        Emitter* fire = Particle.CreateEmitter();
        fire->type = EmitterType::Point;
        fire->position = worldPos;
        fire->direction = glm::vec3(0.0f, 0.001f, 0.0f);
        fire->velocity = glm::vec3(0.0f, 0.12f, 0.0f);
        fire->velocityBias = 0.001f;
        fire->scale = glm::vec3(0.04f);
        fire->scaleTo = glm::vec3(1.006f);
        fire->colorBegin = Color(1.0f, 0.7f, 0.1f);
        fire->colorEnd = Color(0.8f, 0.1f, 0.0f);
        fire->colorBias = 0.04f;
        fire->spread = 0.0f;
        fire->angle = 0.3f;
        fire->width = 1.3f;
        fire->height = 0.4f;
        fire->maxParticles = 5;
        fire->spawnRate = 30.0f;
        chunk->emitters.push_back(fire);
        
        Emitter* smoke = Particle.CreateEmitter();
        smoke->type = EmitterType::Point;
        smoke->position = worldPos + glm::vec3(0.0f, 0.2f, 0.0f);
        smoke->direction = glm::vec3(0.0f, 0.002f, 0.0f);
        smoke->velocity = glm::vec3(0.0f, 0.04f, 0.0f);
        smoke->velocityBias = 0.002f;
        smoke->scale = glm::vec3(0.08f);
        smoke->scaleTo = glm::vec3(1.004f);
        smoke->colorBegin = Color(0.25f, 0.25f, 0.25f, 0.1f);
        smoke->colorEnd = Color(0.65f, 0.65f, 0.65f, 0.0f);
        smoke->colorBias = 0.015f;
        smoke->spread = 0.0f;
        smoke->angle = 0.2f;
        smoke->width = 2.5f;
        smoke->height = 13.0f;
        smoke->maxParticles = 20;
        smoke->spawnRate = 80.0f;
        chunk->emitters.push_back(smoke);
    }
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

std::vector<NearbyStaticInfo> ChunkManager::QueryRadius(const glm::vec3& position, float range) {
    std::lock_guard<std::mutex> lock(mux);
    std::vector<NearbyStaticInfo> results;
    
    // Center position projected onto the XZ plane for chunk distance check
    glm::vec3 center2D(position.x, 0.0f, position.z);
    
    // Maximum distance from actor to chunk center before we can safely ignore the chunk.
    // Chunk radius on XZ plane is approx chunkSize * sqrt(0.5) (~0.7071)
    const float maxChunkDist = (chunkSize * 0.7071f) + range;
    
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        Chunk& chunk = *chunks[i];
        glm::vec3 chunkPos(chunk.x, 0.0f, chunk.y);
        
        // Fast early-exit: Skip chunks that are completely outside the range
        if (glm::distance(chunkPos, center2D) > maxChunkDist) 
            continue;
        
        // Iterate over all static objects in candidate chunk
        for (const StaticObject& obj : chunk.statics) {
            // Convert chunk-local static position to world space
            glm::vec3 worldPos = obj.position + glm::vec3(chunk.x, 0.0f, chunk.y);
            
            float dist = glm::distance(position, worldPos);
            if (dist > range) 
                continue;
            
            NearbyStaticInfo info;
            info.worldPosition = worldPos;
            info.rotation      = obj.rotation;
            info.scale         = obj.scale;
            info.distance      = dist;
            
            // Resolve object type string from index
            std::unordered_map<unsigned int, std::string>::iterator typeIt = world.classIndexToName.find(obj.type);
            if (typeIt != world.classIndexToName.end()) {
                info.type = typeIt->second;
            }
            
            // Resolve mesh string from index
            std::unordered_map<unsigned int, std::string>::iterator meshIt = mStaticIndexToMesh.find(obj.mesh);
            if (meshIt != mStaticIndexToMesh.end()) {
                info.mesh = meshIt->second;
            }
            
            results.push_back(info);
        }
    }
    
    return results;
}

std::vector<std::pair<std::string, glm::vec3>> ChunkManager::QueryDecorNames(const glm::vec3& position, float range) {
    std::lock_guard<std::mutex> lock(mux);
    std::vector<std::pair<std::string, glm::vec3>> results;
    
    // Center position projected onto XZ plane for chunk bounding pre-filter
    glm::vec3 center2D(position.x, 0.0f, position.z);
    
    // Maximum distance from center point to chunk origin before safely skipping
    const float maxChunkDist = (chunkSize * 0.7071f) + range;
    
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        Chunk& chunk = *chunks[i];
        glm::vec3 chunkPos(chunk.x, 0.0f, chunk.y);
        
        if (glm::distance(chunkPos, center2D) > maxChunkDist) 
            continue;
        
        // Iterate through static objects in candidate chunk
        for (const StaticObject& obj : chunk.statics) {
            glm::vec3 worldPos = obj.position + glm::vec3(chunk.x, 0.0f, chunk.y);
            
            if (glm::distance(position, worldPos) <= range) {
                std::unordered_map<unsigned int, std::string>::iterator typeIt = world.classIndexToName.find(obj.type);
                if (typeIt != world.classIndexToName.end()) {
                    results.emplace_back(typeIt->second, worldPos);
                }
            }
        }
    }
    
    return results;
}

void ChunkManager::QueueDecorAt(const std::string& type, const glm::vec3& position, const glm::vec3& rotation) {
    if (type.empty())
        return;
    
    std::lock_guard<std::mutex> lock(mPendingStaticRequestsMutex);
    mPendingStaticRequests.push_back({ type, position, rotation });
}

DecorationHitInfo ChunkManager::QueryDecor(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold) {
    std::lock_guard<std::mutex> lock(mux);
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

std::string ChunkManager::QueryWorld(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold) {
    DecorationHitInfo info = QueryDecor(position, direction, maxDistance, threshold);
    
    if (!info.didHit) 
        return "";
    
    return info.type + "," + Float.ToString(info.hitPoint.x) + ", " + 
                             Float.ToString(info.hitPoint.y) + ", " + 
                             Float.ToString(info.hitPoint.z);
}

bool ChunkManager::PlaceDecor(glm::vec3 position, glm::vec3 direction, const std::string& name, float maxDistance, float threshold) {
    std::unordered_map<std::string, ClassDefinition>::iterator itClass = world.classDefinitions.find(name);
    if (itClass == world.classDefinitions.end())
        return false;
    
    const ClassDefinition& definition = itClass->second;
    bool isMeshless = definition.mesh.empty() || definition.mesh == "none";
    if (!isMeshless && mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
        return false;
    
    glm::vec3 cameraDir = glm::normalize(direction);
    
    const float grid = 0.25f;
    const glm::vec3 gridOrigin(0.5f, 0.5f, 0.5f);

    const int cs = this->chunkSize;
    
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
        
        glm::vec3 forward = cameraDir;
        
        if (glm::length2(forward) < 1e-6f) {
            forward = glm::vec3(0.0f, 0.0f, 1.0f);
        } else {
            forward = glm::normalize(forward);
        }
        
        float horizLen = std::sqrt(forward.x * forward.x + forward.z * forward.z);
        float yawRad   = std::atan2(forward.x, forward.z);
        float pitchRad = 0.0f;
        
        if (horizLen > 1e-6f) {
            pitchRad = std::atan2(-forward.y, horizLen);
        }
        
        float yawDeg   = glm::degrees(yawRad);
        float pitchDeg = glm::degrees(pitchRad);
        float rollDeg  = 0.0f;
        
        glm::vec3 rotation(pitchDeg, yawDeg, rollDeg);
        
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
        
        return placeStatic(chunk, position);
    }
    
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

bool ChunkManager::PlaceStructure(glm::vec3 position, glm::vec3 direction, const std::string& name, float maxDistance, float threshold) {
    // Verify structure definition exists
    auto itStruct = world.classStructures.find(name);
    if (itStruct == world.classStructures.end()) {
        return false;
    }
    
    const ClassStructure& structure = itStruct->second;
    glm::vec3 cameraDir = glm::normalize(direction);
    
    const float grid = 1.0f;
    const glm::vec3 gridOrigin(0.5f, 0.5f, 0.5f);
    
    float bestDot = -1.0f;
    int bestChunkIndex = -1;
    glm::vec3 bestHitPoint(0.0f);
    glm::vec3 bestHitNormal(0.0f);
    
    // Perform raycast against existing static mesh bounding boxes in loaded chunks
    for (unsigned int ci = 0; ci < chunks.Size(); ++ci) {
        Chunk& chunk = *chunks[ci];
        glm::vec3 chunkWorld(chunk.x, 0.0f, chunk.y);
        
        if (glm::distance(chunkWorld, position) > chunkSize)
            continue;
        
        Mesh* mesh = chunk.staticObject->GetComponent<MeshRenderer>()->mesh;
        const unsigned subCount = mesh->GetSubMeshCount();
        
        for (unsigned s = 0; s < subCount; ++s) {
            SubMesh sub;
            mesh->GetSubMesh(s, sub);
            
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
                bestHitPoint     = hitPoint;
                bestHitNormal    = (glm::length(n) > 0.0f) ? glm::normalize(n) : glm::vec3(0, 1, 0);
            }
        }
    }
    
    glm::vec3 buildWorldPos(0.0f);
    Chunk* targetChunk = nullptr;
    
    // Resolve target position and chunk (either from static hit or ground terrain)
    if (bestChunkIndex != -1) {
        targetChunk = chunks[bestChunkIndex];
        glm::vec3 placeWorld = bestHitPoint + bestHitNormal * grid;
        glm::bvec3 axes = AxesForFace(bestHitNormal);
        buildWorldPos = SnapAxes(placeWorld, axes, grid, gridOrigin);
    } else {
        Hit groundHit;
        if (!Physics.Raycast(position, cameraDir, maxDistance, groundHit, LayerMask::Ground)) 
            return false;
        
        buildWorldPos = groundHit.point;
        buildWorldPos = SnapAxes(buildWorldPos, glm::bvec3(true, false, true), grid, gridOrigin);
        
        int bestIdx = -1;
        float bestDist = 1e9f;
        for (unsigned int i = 0; i < chunks.Size(); ++i) {
            glm::vec3 cpos(chunks[i]->x, 0.0f, chunks[i]->y);
            float d = glm::distance(cpos, buildWorldPos);
            if (d < bestDist && d <= chunkSize) {
                bestDist = d;
                bestIdx = (int)i;
            }
        }
        
        if (bestIdx == -1)
            return false;
        
        targetChunk = chunks[bestIdx];
    }
    
    // Convert world position to local chunk coordinates
    glm::vec3 baseLocalPos(buildWorldPos.x - targetChunk->x, buildWorldPos.y, buildWorldPos.z - targetChunk->y);
    float structuralHeightMax = 0.0f;
    
    for (unsigned int s = 0; s < structure.stacks.size(); s++) {
        const ClassStructure::SubStructureStack& stack = structure.stacks[s];
        auto itDef = world.classDefinitions.find(stack.name);
        if (itDef == world.classDefinitions.end()) 
            continue;
        
        ClassDefinition& definition = itDef->second;
        if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
            continue;
        
        unsigned int range = (stack.heightMax <= stack.heightMin) 
            ? stack.heightMax 
            : Random.Range(stack.heightMin, stack.heightMax);
        
        glm::mat4 rotMat(1.0f);
        rotMat = glm::rotate(rotMat, glm::radians(stack.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotMat = glm::rotate(rotMat, glm::radians(stack.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        rotMat = glm::rotate(rotMat, glm::radians(stack.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        
        glm::vec3 stepDir = glm::vec3(rotMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
        if (glm::length2(stepDir) > 1e-6f) {
            stepDir = glm::normalize(stepDir);
        } else {
            stepDir = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        
        float stepDistance = 1.0f; 
        
        for (unsigned int h = 0; h < range; h++) {
            glm::vec3 along = stepDir * (stepDistance * static_cast<float>(h));
            glm::vec3 offset = baseLocalPos + stack.position + along;
            
            if (offset.y > structuralHeightMax) 
                structuralHeightMax = offset.y;
            
            AddDecor(targetChunk, definition.mesh, stack.name, offset, stack.rotation);
        }
    }
    
    // Build Structure Places
    for (unsigned int s = 0; s < structure.places.size(); s++) {
        const ClassStructure::SubStructurePlace& place = structure.places[s];
        auto itDef = world.classDefinitions.find(place.name);
        if (itDef == world.classDefinitions.end()) 
            continue;
        
        ClassDefinition& definition = itDef->second;
        if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
            continue;
        
        glm::vec3 offset = baseLocalPos + place.position;
        if (definition.alignment == 1) {
            offset.y = Snap1D(offset.y, grid, gridOrigin.y);
        }
        
        AddDecor(targetChunk, definition.mesh, place.name, offset, place.rotation);
    }
    
    // Build Structure Fills
    for (unsigned int s = 0; s < structure.fills.size(); s++) {
        const ClassStructure::SubStructureFill& fill = structure.fills[s];
        auto itDef = world.classDefinitions.find(fill.name);
        if (itDef == world.classDefinitions.end()) 
            continue;
    
        ClassDefinition& definition = itDef->second;
        if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
            continue;
    
        glm::vec3 from = baseLocalPos + fill.from;
        glm::vec3 to   = baseLocalPos + fill.to;
        glm::vec3 mn(glm::min(from.x, to.x), glm::min(from.y, to.y), glm::min(from.z, to.z));
        glm::vec3 mx(glm::max(from.x, to.x), glm::max(from.y, to.y), glm::max(from.z, to.z));
    
        int x0 = (int)std::floor(mn.x), x1 = (int)std::floor(mx.x);
        int y0 = (int)std::floor(mn.y), y1 = (int)std::floor(mx.y);
        int z0 = (int)std::floor(mn.z), z1 = (int)std::floor(mx.z);
    
        for (int z = z0; z <= z1; ++z) {
            for (int y = y0; y <= y1; ++y) {
                for (int x = x0; x <= x1; ++x) {
                    glm::vec3 offset((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f);
                    AddDecor(targetChunk, definition.mesh, fill.name, offset, glm::vec3(0.0f));
                }
            }
        }
    }
    
    // Build Structure Patterns (Leaves / Canopy procedural meshes)
    for (unsigned int s = 0; s < structure.patterns.size(); s++) {
        const ClassStructure::SubStructurePattern& pattern = structure.patterns[s];
        glm::vec3 offset = baseLocalPos + pattern.position + glm::vec3(0, structuralHeightMax, 0);
    
        BuildDecorStructure(targetChunk, offset, pattern.pattern, pattern.name, pattern.mesh);
    }
    
    // Re-upload mesh buffers to update rendering instantly
    Mesh* staticMesh = targetChunk->staticObject->GetComponent<MeshRenderer>()->mesh;
    staticMesh->Load();
    
    return true;
}

bool ChunkManager::PlaceDecorAt(const std::string& type, const glm::vec3& position, const glm::vec3& rotation) {
    std::unordered_map<std::string, ClassDefinition>::iterator itClass = world.classDefinitions.find(type);
    if (itClass == world.classDefinitions.end()) {
        return false;
    }
    
    const ClassDefinition& definition = itClass->second;
    bool isMeshless = definition.mesh.empty() || definition.mesh == "none";
    if (!isMeshless && mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) {
        return false;
    }
    
    float halfChunk = chunkSize * 0.5f;
    Chunk* targetChunk = nullptr;
    
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        Chunk* c = chunks[i];
        if (std::abs(position.x - c->x) <= halfChunk &&
            std::abs(position.z - c->y) <= halfChunk) {
            targetChunk = c;
            break;
        }
    }
    
    if (!targetChunk) return false;
    
    glm::vec3 localPos = position - glm::vec3(targetChunk->x, 0.0f, targetChunk->y);
    
    AddDecor(targetChunk, definition.mesh, type, localPos, rotation);
    
    Mesh* staticMesh = targetChunk->staticObject->GetComponent<MeshRenderer>()->mesh;
    staticMesh->Load();
    
    return true;
}

bool ChunkManager::PlaceStructureAt(const std::string& name, const glm::vec3& position) {
    // Verify structure definition exists
    auto itStruct = world.classStructures.find(name);
    if (itStruct == world.classStructures.end()) {
        return false;
    }
    
    const ClassStructure& structure = itStruct->second;
    
    // Locate the chunk that owns this world position
    float halfChunk = chunkSize * 0.5f;
    Chunk* targetChunk = nullptr;
    
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        Chunk* c = chunks[i];
        if (std::abs(position.x - c->x) <= halfChunk &&
            std::abs(position.z - c->y) <= halfChunk) {
            targetChunk = c;
            break;
        }
    }
    
    if (!targetChunk) return false;
    
    // Convert world position to chunk-local coordinates
    glm::vec3 baseLocalPos(position.x - targetChunk->x, position.y, position.z - targetChunk->y);
    float structuralHeightMax = 0.0f;
    
    const float grid = 1.0f;
    const glm::vec3 gridOrigin(0.5f, 0.5f, 0.5f);
    
    // Build Structure Stacks
    for (unsigned int s = 0; s < structure.stacks.size(); s++) {
        const ClassStructure::SubStructureStack& stack = structure.stacks[s];
        auto itDef = world.classDefinitions.find(stack.name);
        if (itDef == world.classDefinitions.end()) 
            continue;
        
        ClassDefinition& definition = itDef->second;
        if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
            continue;
        
        unsigned int range = (stack.heightMax <= stack.heightMin) 
            ? stack.heightMax 
            : Random.Range(stack.heightMin, stack.heightMax);
        
        for (unsigned int h = 0; h < range; h++) {
            glm::vec3 offset = baseLocalPos + stack.position + glm::vec3(0, (float)h, 0);
            if ((float)h > structuralHeightMax) {
                structuralHeightMax = (float)h;
            }
            AddDecor(targetChunk, definition.mesh, stack.name, offset, glm::vec3(0.0f));
        }
    }
    
    // Build Structure Places
    for (unsigned int s = 0; s < structure.places.size(); s++) {
        const ClassStructure::SubStructurePlace& place = structure.places[s];
        auto itDef = world.classDefinitions.find(place.name);
        if (itDef == world.classDefinitions.end()) 
            continue;
        
        ClassDefinition& definition = itDef->second;
        if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
            continue;
        
        glm::vec3 offset = baseLocalPos + place.position;
        if (definition.alignment == 1) {
            offset.y = Snap1D(offset.y, grid, gridOrigin.y);
        }
        
        AddDecor(targetChunk, definition.mesh, place.name, offset, glm::vec3(0.0f));
    }
    
    // Build Structure Fills
    for (unsigned int s = 0; s < structure.fills.size(); s++) {
        const ClassStructure::SubStructureFill& fill = structure.fills[s];
        auto itDef = world.classDefinitions.find(fill.name);
        if (itDef == world.classDefinitions.end()) 
            continue;
        
        ClassDefinition& definition = itDef->second;
        if (mStaticMeshes.find(definition.mesh) == mStaticMeshes.end()) 
            continue;
        
        glm::vec3 from = baseLocalPos + fill.from;
        glm::vec3 to   = baseLocalPos + fill.to;
        glm::vec3 mn(glm::min(from.x, to.x), glm::min(from.y, to.y), glm::min(from.z, to.z));
        glm::vec3 mx(glm::max(from.x, to.x), glm::max(from.y, to.y), glm::max(from.z, to.z));
        
        int x0 = (int)std::floor(mn.x), x1 = (int)std::floor(mx.x);
        int y0 = (int)std::floor(mn.y), y1 = (int)std::floor(mx.y);
        int z0 = (int)std::floor(mn.z), z1 = (int)std::floor(mx.z);
        
        for (int z = z0; z <= z1; ++z) {
            for (int y = y0; y <= y1; ++y) {
                for (int x = x0; x <= x1; ++x) {
                    glm::vec3 offset((float)x + 0.5f, (float)y + 0.5f, (float)z + 0.5f);
                    AddDecor(targetChunk, definition.mesh, fill.name, offset, glm::vec3(0.0f));
                }
            }
        }
    }
    
    // Build Structure Patterns
    for (unsigned int s = 0; s < structure.patterns.size(); s++) {
        const ClassStructure::SubStructurePattern& pattern = structure.patterns[s];
        glm::vec3 offset = baseLocalPos + pattern.position + glm::vec3(0, structuralHeightMax, 0);
        
        BuildDecorStructure(targetChunk, offset, pattern.pattern, pattern.name, pattern.mesh);
    }
    
    // Re-upload mesh buffers to update rendering
    Mesh* staticMesh = targetChunk->staticObject->GetComponent<MeshRenderer>()->mesh;
    staticMesh->Load();
    
    return true;
}

bool ChunkManager::RemoveDecor(glm::vec3 position, glm::vec3 direction, float maxDistance, float threshold) {
    std::lock_guard<std::mutex> lock(mux);
    float closestHitDist = maxDistance + 1.0f;
    int bestChunkIndex = -1;
    int bestSubMeshIndex = -1;
    glm::vec3 bestLocalPos(0.0f);
    glm::vec3 rayDir = glm::normalize(direction);
    
    for (unsigned int i = 0; i < chunks.Size(); i++) {
        Chunk& chunk = *chunks[i];
        glm::vec3 chunkPos = glm::vec3(chunk.x, 0.0f, chunk.y);
        
        glm::vec3 pos2D(position.x, 0.0f, position.z);
        if (glm::distance(chunkPos, pos2D) > (chunkSize * 0.707f + maxDistance)) 
            continue;
            
        Mesh* chunkMesh = chunk.staticObject->GetComponent<MeshRenderer>()->mesh;
        unsigned int subCount = chunkMesh->GetSubMeshCount();
        
        SubMesh subMesh;
        for (unsigned int s = 0; s < subCount; s++) {
            chunkMesh->GetSubMesh(s, subMesh);
            glm::vec3 worldPos = subMesh.position + glm::vec3(chunk.x, 0.0f, chunk.y);
            
            if (glm::distance(worldPos, position) > maxDistance) 
                continue;
                
            glm::vec3 boxScale(2.0f);
            if (s < chunk.statics.size()) {
                boxScale = glm::max(chunk.statics[s].scale, glm::vec3(1.0f));
            }
            
            glm::vec3 min = worldPos - (boxScale * 0.5f);
            glm::vec3 max = worldPos + (boxScale * 0.5f);
            float hitDistance = 0.0f;
            if (!RayIntersectsAABB(position, rayDir, min, max, hitDistance)) 
                continue;
            if (hitDistance > maxDistance) 
                continue;
                
            if (hitDistance < closestHitDist) {
                closestHitDist = hitDistance;
                bestChunkIndex = (int)i;
                bestSubMeshIndex = (int)s;
                bestLocalPos = subMesh.position;
            }
        }
    }
    
    // Remove candidate, destroy particle emitters, and synchronize animatedStatics
    if (bestChunkIndex != -1) {
        Chunk& chunk = *chunks[bestChunkIndex];
        Mesh* mesh = chunk.staticObject->GetComponent<MeshRenderer>()->mesh;
        
        auto removeStaticAt = [&](size_t idx) {
            StaticObject& obj = chunk.statics[idx];
            glm::vec3 worldPos = obj.position + glm::vec3(chunk.x, 0.0f, chunk.y);
            
            // Destroy associated particle emitters (e.g. function 5 fire and smoke)
            if (obj.function == 5) {
                for (int e = (int)chunk.emitters.size() - 1; e >= 0; e--) {
                    Emitter* emitter = chunk.emitters[e];
                    if (glm::distance(emitter->position, worldPos) < 0.5f ||
                        glm::distance(emitter->position, worldPos + glm::vec3(0.0f, 0.2f, 0.0f)) < 0.5f) {
                        Particle.DestroyEmitter(emitter);
                        chunk.emitters.erase(chunk.emitters.begin() + e);
                    }
                }
            }
            
            chunk.statics.erase(chunk.statics.begin() + idx);
            for (int a = (int)chunk.animatedStatics.size() - 1; a >= 0; a--) {
                if (chunk.animatedStatics[a].staticIndex == idx) {
                    chunk.animatedStatics.erase(chunk.animatedStatics.begin() + a);
                } else if (chunk.animatedStatics[a].staticIndex > idx) {
                    chunk.animatedStatics[a].staticIndex--;
                }
            }
        };
        
        const float epsilon = 0.01f;
        for (size_t i = 0; i < chunk.statics.size(); ++i) {
            if (glm::distance(chunk.statics[i].position, bestLocalPos) > epsilon) 
                continue;
            removeStaticAt(i);
            mesh->RemoveSubMesh(bestSubMeshIndex);
            mesh->Load();
            return true;
        }
        
        if (bestSubMeshIndex < (int)chunk.statics.size()) {
            removeStaticAt(bestSubMeshIndex);
            mesh->RemoveSubMesh(bestSubMeshIndex);
            mesh->Load();
            return true;
        }
    }
    
    return false;
}

bool ChunkManager::RemoveDecorAt(const glm::vec3& position, float tolerance) {
    float halfChunk = chunkSize * 0.5f;
    Chunk* targetChunk = nullptr;
    
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        Chunk* c = chunks[i];
        if (std::abs(position.x - c->x) <= halfChunk &&
            std::abs(position.z - c->y) <= halfChunk) {
            targetChunk = c;
            break;
        }
    }
    
    if (!targetChunk) return false;
    
    glm::vec3 localTarget = position - glm::vec3(targetChunk->x, 0.0f, targetChunk->y);
    bool objectsRemoved = false;
    
    // Iterate backward to safely erase multiple elements
    for (int i = (int)targetChunk->statics.size() - 1; i >= 0; --i) {
        if (glm::distance(targetChunk->statics[i].position, localTarget) <= tolerance) {
            StaticObject& obj = targetChunk->statics[i];
            glm::vec3 worldPos = obj.position + glm::vec3(targetChunk->x, 0.0f, targetChunk->y);
            
            // Destroy associated particle emitters
            if (obj.function == 5) {
                for (int e = (int)targetChunk->emitters.size() - 1; e >= 0; e--) {
                    Emitter* emitter = targetChunk->emitters[e];
                    if (glm::distance(emitter->position, worldPos) < 0.5f ||
                        glm::distance(emitter->position, worldPos + glm::vec3(0.0f, 0.2f, 0.0f)) < 0.5f) {
                        Particle.DestroyEmitter(emitter);
                        targetChunk->emitters.erase(targetChunk->emitters.begin() + e);
                    }
                }
            }
            
            targetChunk->statics.erase(targetChunk->statics.begin() + i);
            
            // Synchronize animatedStatics tracking indices
            for (int a = (int)targetChunk->animatedStatics.size() - 1; a >= 0; a--) {
                if (targetChunk->animatedStatics[a].staticIndex == i) {
                    targetChunk->animatedStatics.erase(targetChunk->animatedStatics.begin() + a);
                } else if (targetChunk->animatedStatics[a].staticIndex > i) {
                    targetChunk->animatedStatics[a].staticIndex--;
                }
            }
            
            Mesh* mesh = targetChunk->staticObject->GetComponent<MeshRenderer>()->mesh;
            mesh->RemoveSubMesh(static_cast<unsigned int>(i));
            
            objectsRemoved = true;
        }
    }
    
    // Load the mesh once after all overlapping objects are removed
    if (objectsRemoved) {
        Mesh* mesh = targetChunk->staticObject->GetComponent<MeshRenderer>()->mesh;
        mesh->Load();
        return true;
    }
    
    return false;
}

bool ChunkManager::RemoveDecorByIndex(Chunk* targetChunk, size_t index, bool rebuildMesh) {
    if (!targetChunk || index >= targetChunk->statics.size()) {
        return false;
    }
    
    StaticObject& obj = targetChunk->statics[index];
    glm::vec3 worldPos = obj.position + glm::vec3(targetChunk->x, 0.0f, targetChunk->y);
    
    // Destroy associated particle emitters
    if (obj.function == 5) {
        for (int e = (int)targetChunk->emitters.size() - 1; e >= 0; e--) {
            Emitter* emitter = targetChunk->emitters[e];
            if (glm::distance(emitter->position, worldPos) < 0.5f ||
                glm::distance(emitter->position, worldPos + glm::vec3(0.0f, 0.2f, 0.0f)) < 0.5f) {
                Particle.DestroyEmitter(emitter);
                targetChunk->emitters.erase(targetChunk->emitters.begin() + e);
            }
        }
    }
    
    // Remove the object from the primary array
    targetChunk->statics.erase(targetChunk->statics.begin() + index);
    
    // Synchronize animatedStatics tracking indices
    for (int a = (int)targetChunk->animatedStatics.size() - 1; a >= 0; a--) {
        if (targetChunk->animatedStatics[a].staticIndex == index) {
            targetChunk->animatedStatics.erase(targetChunk->animatedStatics.begin() + a);
        } else if (targetChunk->animatedStatics[a].staticIndex > index) {
            targetChunk->animatedStatics[a].staticIndex--;
        }
    }
    
    // Remove mesh data
    Mesh* mesh = targetChunk->staticObject->GetComponent<MeshRenderer>()->mesh;
    mesh->RemoveSubMesh(static_cast<unsigned int>(index));
    
    if (rebuildMesh) {
        mesh->Load();
    }
    
    return true;
}

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
