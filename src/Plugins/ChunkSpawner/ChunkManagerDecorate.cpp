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
    
    AddDecor(chunk, staticMesh, DecorationMesh::Cross, position + glm::vec3(0.0f, grassHeight * 0.16f, 0.0f), rotation, scale, finalColor);
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
    
    Color trunkColor = Colors.Range(trunkColorMin, trunkColorMax);
    AddDecor(chunk, staticMesh, DecorationMesh::Log, glm::vec3(position.x, position.y + (height * 0.25f), position.z), rotation, scale, trunkColor);
    
    
    if (name == "oak") {
        unsigned int clustersMin = 4;
        unsigned int clustersMax = 8;
        unsigned int numberOfClusters = Random.Range(clustersMin, clustersMax);
        
        unsigned int leavesPerClusterMin = 3;
        unsigned int leavesPerClusterMax = 6;
        
        float baseLeafWidth  = leafWidth;
        float baseLeafHeight = leafHeight;
        
        // vertical center of the crown
        float crownY = position.y + (height * 0.75f);
        
        for (unsigned int c = 0; c < numberOfClusters; c++) {
            // pick a random point on a sphere surface around the crown center
            float yaw   = Random.Range(0.0f, 360.0f);
            float pitch = Random.Range(-30.0f, 50.0f); // slight bias upward
            float radYaw   = glm::radians(yaw);
            float radPitch = glm::radians(pitch);
            
            float radius = leafSpreadArea * (0.7f + Random.Range(0.0f, 0.3f));
            
            glm::vec3 clusterCenter(
                position.x + std::cos(radYaw) * std::cos(radPitch) * radius,
                crownY     + std::sin(radPitch) * radius * 0.6f,
                position.z + std::sin(radYaw) * std::cos(radPitch) * radius
            );
            
            unsigned int leavesInCluster = Random.Range(leavesPerClusterMin, leavesPerClusterMax);
            
            for (unsigned int l = 0; l < leavesInCluster; l++) {
                glm::vec3 leafOffset(
                    Random.Range(-leafSpreadArea * 0.15f, leafSpreadArea * 0.15f),
                    Random.Range(-leafSpreadHeight * 0.15f, leafSpreadHeight * 0.15f),
                    Random.Range(-leafSpreadArea * 0.15f, leafSpreadArea * 0.15f)
                );
                
                glm::vec3 leafPos = clusterCenter + leafOffset;
                
                glm::vec3 leafScale(
                    baseLeafWidth  * (0.85f + Random.Range(0.0f, 0.3f)),
                    baseLeafHeight * (0.85f + Random.Range(0.0f, 0.3f)),
                    baseLeafWidth  * (0.85f + Random.Range(0.0f, 0.3f))
                );
                
                glm::vec3 leafRot(
                    Random.Range(0, 360),
                    Random.Range(0, 360),
                    Random.Range(0, 360)
                );
                
                Color leafColor = Colors.Range(leafColorMin, leafColorMax);
                AddDecor(chunk, staticMesh, DecorationMesh::Leaf, leafPos, leafRot, leafScale, leafColor);
            }
        }
        return;
    }
    
    
    if (name == "pine") {
        unsigned int numberOfLeaves = Random.Range(numberOfLeavesMin, numberOfLeavesMax);
        
        // keep base sizes intact
        float baseLeafWidth  = leafWidth;
        float baseLeafHeight = leafHeight;
        float baseTilt    = 28.0f;
        float baseScale   = 0.7f;
        float baseHeight  = position.y + (height * 0.5f);
        
        float rateOfTilt    = 15.0f;
        float rateOfScale   = 0.08f;
        float rateOfHeight  = 0.7f;
        float tiltOffsetMul = 0.007f;
        
        unsigned int numberOfRings = 4;
        
        for (unsigned int r = 0; r < numberOfRings; r++) {
            
            float leafTilt    = baseTilt   + (r * rateOfTilt);
            float scaleFactor = baseScale  - (r * rateOfScale);
            float ringHeight  = baseHeight + (r * rateOfHeight);
            
            for (unsigned int l = 0; l < numberOfLeaves; l++) {
                glm::vec3 leafScale = glm::vec3(baseLeafWidth, baseLeafHeight, baseLeafWidth) * scaleFactor;
                float radius = (leafSpreadArea * scaleFactor) - (leafTilt * tiltOffsetMul);
                if (radius < 0.0f) radius = 0.0f;
                
                float someRandomYaw = Random.Range(1, 100) * 0.5f;
                
                float angle = ((360.0f / numberOfLeaves) * l) + someRandomYaw;
                float rad   = glm::radians(angle);
                
                float dx = std::cos(rad) * radius;
                float dz = std::sin(rad) * radius;
                glm::vec3 leafPosition(position.x + dx, ringHeight, position.z + dz);
                
                float yawDeg = glm::degrees(std::atan2(dx, dz));
                
                glm::vec3 leafRotation(leafTilt, 0.0f, yawDeg);
                
                Color leafColor = Colors.Range(leafColorMin, leafColorMax);
                AddDecor(chunk, staticMesh, DecorationMesh::Leaf, leafPosition, leafRotation, leafScale, leafColor);
            }
        }
        return;
    }
    
    
    if (name == "willow") {
        // overall canopy feel
        unsigned int curtainsMin = 6;
        unsigned int curtainsMax = 12;
        unsigned int numberOfCurtains = Random.Range(curtainsMin, curtainsMax);
        
        // leaves per hanging chain
        unsigned int segmentsMin = 5;
        unsigned int segmentsMax = 9;
        unsigned int leavesPerChain = Random.Range(segmentsMin, segmentsMax);
        
        // base geo from definition
        float baseLeafWidth  = leafWidth;
        float baseLeafHeight = leafHeight;
        
        // start canopy a bit above mid-trunk; willows hang down from there
        float crownY      = position.y + (height * 0.8f);
        float baseRadius  = leafSpreadArea * 0.9f;   // ring radius where chains start
        float maxHang     = glm::max(leafSpreadHeight, height * 0.35f); // how far chains can hang
        
        // sway/taper controls
        float swayAmplitude = leafSpreadArea * 0.15f;     // side-to-side
        float pitchStartDeg = 25.0f;                      // near crown
        float pitchEndDeg   = 85.0f;                      // near tip
        float scaleTaper    = 0.35f;                      // leaf size reduction from top to bottom
        float radiusInward  = 0.25f;                      // inward pull as chains descend
        
        for (unsigned int c = 0; c < numberOfCurtains; c++) {
            float jitterYaw   = Random.Range(-12.0f, 12.0f);
            float angleDeg    = ((360.0f / numberOfCurtains) * c) + jitterYaw;
            float angRad      = glm::radians(angleDeg);
            
            // base point around the trunk
            float dx0 = std::cos(angRad) * baseRadius;
            float dz0 = std::sin(angRad) * baseRadius;
            
            // randomize chain length & local sway phase
            unsigned int segCount = leavesPerChain + Random.Range(-1, 1);
            if (segCount < 3) segCount = 3;
            float swayPhase = Random.Range(0.0f, 6.283185f); // 0..2
            
            for (unsigned int s = 0; s < segCount; s++) {
                float t = (segCount <= 1) ? 0.0f : (float)s / (float)(segCount - 1); // 0..1 along chain
                
                // vertical placement: hang downward from crownY
                float y = crownY - (t * maxHang);
                
                // taper radius inward as it hangs, plus gentle sine sway
                float radius = baseRadius * (1.0f - radiusInward * t);
                float sway   = std::sin((t * 3.1415926f * 1.5f) + swayPhase) * (swayAmplitude * (0.5f + 0.5f * (1.0f - t)));
                float dx     = std::cos(angRad) * radius + std::cos(angRad + 1.570796f) * sway; // sway perpendicular to radial
                float dz     = std::sin(angRad) * radius + std::sin(angRad + 1.570796f) * sway;
                
                glm::vec3 leafPos(position.x + dx, y, position.z + dz);
                
                // orientation: face outward around trunk, pitched down more as it hangs
                float yawDeg   = glm::degrees(std::atan2(dx, dz));   // outward
                float pitchDeg = pitchStartDeg + (pitchEndDeg - pitchStartDeg) * t; // droop more at bottom
                float rollDeg  = Random.Range(-8.0f, 8.0f);          // a touch of disorder
                
                glm::vec3 leafRot(pitchDeg, rollDeg, yawDeg);
                
                // size tapers from top to bottom
                float sizeFactor = 1.0f - (scaleTaper * t);
                glm::vec3 leafScale = glm::vec3(baseLeafWidth, baseLeafHeight, baseLeafWidth) * sizeFactor;
                
                Color leafColor = Colors.Range(leafColorMin, leafColorMax);
                AddDecor(chunk, staticMesh, DecorationMesh::Leaf, leafPos, leafRot, leafScale, leafColor);
            }
        }
        return;
    }
    
    
    if (name == "palm") {
        // crown only — long fronds radiating from the top
        unsigned int frondsMin = 6;
        unsigned int frondsMax = 10;
        unsigned int fronds = Random.Range(frondsMin, frondsMax);
        
        float baseLeafWidth  = leafWidth * 2.0f;   // long + thin
        float baseLeafHeight = leafHeight * 0.75f;
        
        float crownY = position.y + (height * 0.78f);
        float radius = glm::max(leafSpreadArea * 0.25f, 0.1f);
        
        for (unsigned int i = 0; i < fronds; i++) {
            float yaw = ((360.0f / fronds) * i) + Random.Range(-10.0f, 10.0f);
            float rad = glm::radians(yaw);
            
            float dx = std::cos(rad) * radius;
            float dz = std::sin(rad) * radius;
            glm::vec3 leafPos(position.x + dx, crownY, position.z + dz);
            
            // tip downward a bit; vary pitch for wind-blown look
            float pitch = Random.Range(18.0f, 42.0f);
            float roll  = Random.Range(-8.0f, 8.0f);
            glm::vec3 leafRot(pitch, roll, yaw);
            
            float scaleMul = 0.85f + Random.Range(0.0f, 0.3f);
            glm::vec3 leafScale(baseLeafWidth * scaleMul, baseLeafHeight * scaleMul, baseLeafWidth * 0.45f * scaleMul);
            
            Color leafColor = Colors.Range(leafColorMin, leafColorMax);
            AddDecor(chunk, staticMesh, DecorationMesh::Leaf, leafPos, leafRot, leafScale, leafColor);
        }
        return;
    }
    
    
    if (name == "acacia") {
        // flat-topped discs with gaps
        unsigned int tiers = 2 + Random.Range(0, 2); // 2–3 levels
        unsigned int leavesPerRing = Random.Range(numberOfLeavesMin, numberOfLeavesMax);
        
        float baseLeafWidth  = leafWidth * 1.2f;
        float baseLeafHeight = leafHeight * 0.03f;
        float heightOffset   = 0.8f;
        
        float topY = position.y + (height * heightOffset);
        float ringDrop = glm::max(0.3f, leafSpreadHeight * 0.25f);
        
        for (unsigned int t = 0; t < tiers; t++) {
            float levelY = topY - (t * ringDrop);
            float spread = leafSpreadArea * (1.0f - 0.25f * (float)t);
            
            for (unsigned int i = 0; i < leavesPerRing; i++) {
                float yaw = ((360.0f / leavesPerRing) * i) + Random.Range(-6.0f, 6.0f);
                float rad = glm::radians(yaw);
                
                float jitter = Random.Range(-spread * 0.12f, spread * 0.12f);
                float dx = std::cos(rad) * (spread + jitter);
                float dz = std::sin(rad) * (spread + jitter);
                glm::vec3 leafPos(position.x + dx, levelY + Random.Range(-0.15f, 0.15f), position.z + dz);
                
                // fairly flat, slight outward tilt
                float pitch = Random.Range(5.0f, 18.0f);
                float roll  = Random.Range(-5.0f, 5.0f);
                glm::vec3 leafRot(pitch, roll, yaw);
                
                float scaleMul = 0.5f + Random.Range(0.0f, 0.35f);
                glm::vec3 leafScale = glm::vec3(baseLeafWidth, baseLeafHeight, baseLeafWidth) * scaleMul;
                
                Color leafColor = Colors.Range(leafColorMin, leafColorMax);
                AddDecor(chunk, staticMesh, DecorationMesh::Leaf, leafPos, leafRot, leafScale, leafColor);
            }
        }
        return;
    }
    
    
    if (name == "birch") {
        // vertical column of small, airy clusters
        unsigned int clusters = Random.Range(5, 9);
        unsigned int leavesPerClusterMin = 4;
        unsigned int leavesPerClusterMax = 8;
        
        float baseLeafWidth  = leafWidth * 0.75f;
        float baseLeafHeight = leafHeight * 0.75f;
        
        float trunkTopY = position.y + (height * 0.8f);
        float trunkBotY = position.y + (height * 0.3f);
        float radius    = glm::max(leafSpreadArea * 0.1f, 0.3f);
        
        for (unsigned int c = 0; c < clusters; c++) {
            float t = (float)c / (float)(clusters - 1);
            float ringY = trunkBotY + (trunkTopY - trunkBotY) * t;
            
            // place each cluster around the trunk with mild offset
            float yaw = Random.Range(0.0f, 360.0f);
            float rad = glm::radians(yaw);
            float dx  = std::cos(rad) * (radius * (0.7f + Random.Range(0.0f, 0.4f)));
            float dz  = std::sin(rad) * (radius * (0.7f + Random.Range(0.0f, 0.4f)));
            glm::vec3 clusterCenter(position.x + dx, ringY, position.z + dz);
            
            unsigned int leavesInCluster = Random.Range(leavesPerClusterMin, leavesPerClusterMax);
            for (unsigned int l = 0; l < leavesInCluster; l++) {
                glm::vec3 leafPos = clusterCenter + glm::vec3(
                    Random.Range(-radius * 0.2f, radius * 0.2f),
                    Random.Range(-leafSpreadHeight * 0.15f, leafSpreadHeight * 0.15f),
                    Random.Range(-radius * 0.2f, radius * 0.2f)
                );
                
                glm::vec3 leafRot(Random.Range(0.0f, 360.0f), Random.Range(0.0f, 360.0f), Random.Range(0.0f, 360.0f));
                
                float scaleMul = 0.75f + Random.Range(0.0f, 0.25f);
                glm::vec3 leafScale(baseLeafWidth * scaleMul, baseLeafHeight * scaleMul, baseLeafWidth * scaleMul);
                
                Color leafColor = Colors.Range(leafColorMin, leafColorMax); // lighter palette recommended in defs
                AddDecor(chunk, staticMesh, DecorationMesh::Leaf, leafPos, leafRot, leafScale, leafColor);
            }
        }
        return;
    }
    
    
    if (name == "spruce") {
        unsigned int rings = 5 + Random.Range(0u, 2u);
        unsigned int perRing = Random.Range(numberOfLeavesMin, numberOfLeavesMax);
        
        float baseLeafWidth  = leafWidth * 0.8f;
        float baseLeafHeight = leafHeight * 0.2f;
        
        float baseTilt   = 18.0f;
        float baseScale  = 0.75f;
        float baseY      = position.y + (height * 0.2f);
        
        float rateTilt   = 10.0f;
        float rateScale  = 0.08f;
        float rateY      = (height * 0.55f) / glm::max(1u, rings - 1);
        
        float tightness  = 0.8f;
        
        for (unsigned int r = 0; r < rings; r++) {
            float tilt    = baseTilt + (r * rateTilt);
            float scale   = baseScale - (r * rateScale);
            if (scale < 0.2f) scale = 0.2f;
            
            float ringY   = baseY + (r * rateY);
            float radius  = (leafSpreadArea * scale * tightness) - (tilt * 0.0065f);
            if (radius < 0.0f) radius = 0.0f;
            
            for (unsigned int i = 0; i < perRing; i++) {
                float yaw = ((360.0f / perRing) * i) + Random.Range(-5.0f, 5.0f);
                float rad = glm::radians(yaw);
                float dx  = std::cos(rad) * radius;
                float dz  = std::sin(rad) * radius;
                
                glm::vec3 pos(position.x + dx, ringY, position.z + dz);
                float faceYaw = glm::degrees(std::atan2(dx, dz));
                glm::vec3 rot(tilt, Random.Range(-4.0f, 4.0f), faceYaw);
                
                glm::vec3 scl(baseLeafWidth * scale, baseLeafHeight * scale, baseLeafWidth * scale);
                
                Color col = Colors.Range(leafColorMin, leafColorMax);
                AddDecor(chunk, staticMesh, DecorationMesh::Leaf, pos, rot, scl, col);
            }
        }
        return;
    }
    
    // No leaf type was selected
    Log.Write("! Leaf type not defined - " + name);
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
        
        AddDecor(chunk, staticMesh, DecorationMesh::Cross, translation, orientation, scale, color);
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
    staticMesh->ChangeSubMeshColor(indexMesh,   color);
    staticMesh->ChangeSubMeshScale(indexMesh,   scale.x, scale.y, scale.z);
    
    staticMesh->ChangeSubMeshRotation(indexMesh, rotation.x, glm::vec3(1.0f, 0, 0));
    staticMesh->ChangeSubMeshRotation(indexMesh, rotation.y, glm::vec3(0, 0, 1.0f));
    staticMesh->ChangeSubMeshRotation(indexMesh, rotation.z, glm::vec3(0, 1.0f, 0));
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

