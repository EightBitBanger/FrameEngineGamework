#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>


void ChunkManager::BuildFunctions::StackAtAngle(Structure& structure, glm::vec3 position, glm::vec3 scale, glm::vec3 angle, float stepHeight, int length, Color color) {
    const glm::vec3 MESH_LONG_AXIS = glm::vec3(0.0f, 1.0f, 0.0f);
    
    float yaw   = glm::radians(angle.x);
    float pitch = glm::radians(angle.y);
    glm::vec3 stepDir(
        std::cos(pitch) * std::sin(yaw), 
        std::sin(pitch), 
        std::cos(pitch) * std::cos(yaw)
    );
    stepDir = glm::normalize(stepDir);
    glm::quat qAlign = glm::rotation(MESH_LONG_AXIS, stepDir);
    glm::quat qRoll  = glm::angleAxis(glm::radians(angle.z), stepDir);
    
    glm::quat qFinal = qAlign;
    glm::vec3 eulerAngles = glm::eulerAngles(qFinal);
    glm::vec3 eulerDeg = glm::degrees(eulerAngles);
    
    for (int h = 0; h < length; ++h) {
        glm::vec3 along = stepDir * (stepHeight * static_cast<float>(h));
        
        StructurePart pole;
        pole.offset   = position + along;
        pole.rotation = eulerDeg;
        pole.scale    = scale;
        pole.color    = color;
        
        structure.parts.push_back(pole);
    }
}

/*
void ChunkManager::BuildFunctions::LeafCluster(Structure& structure, glm::vec3 position, glm::vec3 scale, glm::vec3 angle, float stepHeight, int length, Color color) {
    unsigned int clustersMin = 5;
    unsigned int clustersMax = 7;
    unsigned int numberOfClusters = Random.Range(clustersMin, clustersMax);
    
    unsigned int leavesPerClusterMin = 4;
    unsigned int leavesPerClusterMax = 6;
    
    float baseLeafWidth  = leafWidth;
    float baseLeafHeight = leafHeight;
    
    // vertical center of the crown
    float crownY = position.y + (height * 0.7f);
    
    for (unsigned int c = 0; c < numberOfClusters; c++) {
        // pick a random point on a sphere surface around the crown center
        float yaw   = Random.Range(0.0f, 360.0f);
        float pitch = Random.Range(-30.0f, 50.0f); // slight bias upward
        float radYaw   = glm::radians(yaw);
        float radPitch = glm::radians(pitch);
        
        float radius = leafSpreadArea * (Random.Range(0.0f, 0.7f));
        
        glm::vec3 clusterCenter(
            position.x + std::cos(radYaw) * std::cos(radPitch) * radius,
            crownY     + std::sin(radPitch) * radius * 0.6f,
            position.z + std::sin(radYaw) * std::cos(radPitch) * radius
        );
        
        unsigned int leavesInCluster = Random.Range(leavesPerClusterMin, leavesPerClusterMax);
        
        for (unsigned int l = 0; l < leavesInCluster; l++) {
            glm::vec3 leafOffset(
                Random.Range(-leafSpreadArea * 0.3f, leafSpreadArea * 0.3f),
                Random.Range(-leafSpreadHeight * 0.15f, leafSpreadHeight * 0.15f),
                Random.Range(-leafSpreadArea * 0.3f, leafSpreadArea * 0.3f)
            );
            
            glm::vec3 leafPos = clusterCenter + leafOffset;
            float leafScaler = 0.85f;
            float leafMaxScale = 0.3f;
            
            glm::vec3 leafScale(
                baseLeafWidth  * (leafScaler + Random.Range(0.0f, leafMaxScale)),
                baseLeafHeight * (leafScaler + Random.Range(0.0f, leafMaxScale)),
                baseLeafWidth  * (leafScaler + Random.Range(0.0f, leafMaxScale))
            );
            
            glm::vec3 leafRot(Random.Range(0, 360), Random.Range(0, 360), Random.Range(0, 360));
            
            Color leafColor = Colors.Range(leafColorMin, leafColorMax);
            AddDecor(chunk, staticMesh, DecorationMesh::Leaf, leafPos, leafRot, leafScale, leafColor);
        }
    }
}
*/






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
    glm::vec3 rotation(0.0f, Random.Range(0, 360), 0.0f);
    
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
    
    //
    // Tree type generation
    
    if (name == "oak") {
        unsigned int clustersMin = 5;
        unsigned int clustersMax = 7;
        unsigned int numberOfClusters = Random.Range(clustersMin, clustersMax);
        
        unsigned int leavesPerClusterMin = 4;
        unsigned int leavesPerClusterMax = 6;
        
        float baseLeafWidth  = leafWidth;
        float baseLeafHeight = leafHeight;
        
        // vertical center of the crown
        float crownY = position.y + (height * 0.7f);
        
        for (unsigned int c = 0; c < numberOfClusters; c++) {
            // pick a random point on a sphere surface around the crown center
            float yaw   = Random.Range(0.0f, 360.0f);
            float pitch = Random.Range(-30.0f, 50.0f); // slight bias upward
            float radYaw   = glm::radians(yaw);
            float radPitch = glm::radians(pitch);
            
            float radius = leafSpreadArea * (Random.Range(0.0f, 0.7f));
            
            glm::vec3 clusterCenter(
                position.x + std::cos(radYaw) * std::cos(radPitch) * radius,
                crownY     + std::sin(radPitch) * radius * 0.6f,
                position.z + std::sin(radYaw) * std::cos(radPitch) * radius
            );
            
            unsigned int leavesInCluster = Random.Range(leavesPerClusterMin, leavesPerClusterMax);
            
            for (unsigned int l = 0; l < leavesInCluster; l++) {
                glm::vec3 leafOffset(
                    Random.Range(-leafSpreadArea * 0.3f, leafSpreadArea * 0.3f),
                    Random.Range(-leafSpreadHeight * 0.15f, leafSpreadHeight * 0.15f),
                    Random.Range(-leafSpreadArea * 0.3f, leafSpreadArea * 0.3f)
                );
                
                glm::vec3 leafPos = clusterCenter + leafOffset;
                float leafScaler = 0.85f;
                float leafMaxScale = 0.3f;
                
                glm::vec3 leafScale(
                    baseLeafWidth  * (leafScaler + Random.Range(0.0f, leafMaxScale)),
                    baseLeafHeight * (leafScaler + Random.Range(0.0f, leafMaxScale)),
                    baseLeafWidth  * (leafScaler + Random.Range(0.0f, leafMaxScale))
                );
                
                glm::vec3 leafRot(Random.Range(0, 360), Random.Range(0, 360), Random.Range(0, 360));
                
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
                
                glm::vec3 leafRotation(leafTilt, yawDeg, 0);
                
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
                
                glm::vec3 leafRot(pitchDeg, yawDeg, rollDeg);
                
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
            glm::vec3 leafRot(pitch, yaw, roll);
            
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
                glm::vec3 leafRot(pitch, yaw, roll);
                
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
                glm::vec3 rot(tilt, faceYaw, Random.Range(-4.0f, 4.0f));
                
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
    
    
    float RandomArea = 30.0f;
    glm::vec3 RandomPosition;
    RandomPosition.x = Random.Range(-RandomArea, RandomArea);
    RandomPosition.y = 0.0f;
    RandomPosition.z = Random.Range(-RandomArea, RandomArea);
    
    // Check distance to nearby structures
    glm::vec3 structurePosition = glm::vec3(position.x + chunk->x, position.y, position.z + chunk->y) + RandomPosition;
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
        
        glm::vec3 translation = position + glm::vec3(part.offset.x, part.offset.y, part.offset.z);
        glm::vec3 orientation = part.rotation;
        glm::vec3 scale = part.scale;
        Color color = part.color;
        
        AddDecor(chunk, staticMesh, DecorationMesh::Log, translation, orientation, scale, color);
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

