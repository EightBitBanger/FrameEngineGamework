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




bool ChunkManager::BuildDecorStructure(Chunk* chunk, glm::vec3 position, const std::string& pattern, const std::string& name, const std::string& mesh) {
    
    float leafWidth = 1.0f;
    float leafHeight = 1.0f;
    
    float trunkSize = 1.0f;
    Color trunkColorMin = Colors.white;
    Color trunkColorMax = Colors.white;
    
    if (pattern == "pattern_oak_leaves") {
        unsigned int clustersMin = 5;
        unsigned int clustersMax = 7;
        unsigned int numberOfClusters = Random.Range(clustersMin, clustersMax);
        
        float leafSpreadArea  = 2.0f;
        float leafSpreadHeight = 1.5f;
        
        unsigned int leavesPerClusterMin = 4;
        unsigned int leavesPerClusterMax = 6;
        
        float baseLeafWidth  = leafWidth;
        float baseLeafHeight = leafHeight;
        
        // vertical center of the crown
        float crownY = position.y;
        
        for (unsigned int c = 0; c < numberOfClusters; c++) {
            // pick a random point on a sphere surface around the crown center
            float yaw   = Random.Range(0.0f, 360.0f);
            float pitch = Random.Range(-30.0f, 50.0f); // slight bias upward
            float radYaw   = glm::radians(yaw);
            float radPitch = glm::radians(pitch);
            
            float radius = Random.Range(0.7f, 1.2f);
            
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
                
                AddDecor(chunk, mesh, name, leafPos, leafRot);
            }
        }
        return true;
    }
    
    if (pattern == "pattern_pine_leaves") {
        // Tunable tilt controls
        float baseTilt        = 1.0f;
        float rateOfTilt      = 16.0f;
        float tiltOffsetMul   = 0.007f;
        
        // Optional shaping/jitter
        bool  useCurve        = false;
        float curveExp        = 1.0f;
        float curveBlend      = 1.0f;
        float ringJitterDeg   = 0.087f;
        float leafJitterDeg   = 0.087f;
        
        // Safety and direction
        bool  clampTilt       = true;
        float minTiltDeg      = 90.0f;
        float maxTiltDeg      = 60.0f;
        float directionSign   = 1.0f;
        
        unsigned int numberOfLeavesMin = 3;
        unsigned int numberOfLeavesMax = 8;
        float leafSpreadArea  = 2.0f;
        float leafSpreadHeight = 1.5f;
        
        float rateOfScale     = 0.08f;
        float rateOfHeight    = 1.0f;
        unsigned int numberOfRings = 5;
        
        // Geometry inputs
        float baseLeafWidth   = leafWidth;
        float baseLeafHeight  = leafHeight;
        float baseScale       = 0.7f;
        float baseHeight      = position.y - (rateOfHeight * numberOfRings);
        
        const auto Clamp01 = [](float v) -> float {
            if (v < 0.0f) return 0.0f;
            if (v > 1.0f) return 1.0f;
            return v;
        };
        
        const auto EvalRingT = [&](unsigned int ringIndex) -> float {
            float denom = (numberOfRings > 1) ? float(numberOfRings - 1) : 1.0f;
            float t = float(ringIndex) / denom;
            t = 1.0f - t;
            
            if (useCurve) {
                float curved = std::pow(Clamp01(t), curveExp);
                t = (t * (1.0f - curveBlend)) + (curved * curveBlend);
            }
            return Clamp01(t);
        };
        
        const auto EvalTilt = [&](unsigned int ringIndex) -> float {
            float r1 = float(ringIndex + 1);
            float inc = r1 * rateOfTilt;
            
            float tilt = baseTilt + inc;
            if (clampTilt) {
                if (tilt < minTiltDeg) tilt = minTiltDeg;
                if (tilt > maxTiltDeg) tilt = maxTiltDeg;
            }
            return tilt * directionSign;
        };
        
        for (unsigned int r = 0; r < numberOfRings; r++) {
            float t = EvalRingT(r);
            float leavesF = float(numberOfLeavesMin) + (float(numberOfLeavesMax) - float(numberOfLeavesMin)) * t;
            
            unsigned int leavesThisRing = (unsigned int)std::round(leavesF);
            if (leavesThisRing < 1) leavesThisRing = 1;
            
            float leafTiltRing = EvalTilt(r);
            
            if (ringJitterDeg > 0.0f) {
                float j = Random.Range(-ringJitterDeg, ringJitterDeg);
                leafTiltRing += j;
            }
            
            float scaleFactor = baseScale  - (float(r + 1) * rateOfScale);
            float ringHeight  = baseHeight + (float(r + 1) * rateOfHeight);
            
            float ringYawOffset = Random.Range(0.0f, 360.0f);
            
            for (unsigned int l = 0; l < leavesThisRing; l++) {
                glm::vec3 leafScale =
                    glm::vec3(baseLeafWidth, baseLeafHeight, baseLeafWidth) * scaleFactor;
                
                float leafTilt = leafTiltRing;
                if (leafJitterDeg > 0.0f) {
                    float lj = Random.Range(-leafJitterDeg, leafJitterDeg);
                    leafTilt += lj;
                }
                
                float radius = (leafSpreadArea * scaleFactor) - (leafTilt * tiltOffsetMul);
                if (radius < 0.0f) radius = 0.0f;
                
                // Even spacing with a single per-ring offset
                float angle = (360.0f / float(leavesThisRing)) * float(l) + ringYawOffset;
                float rad   = glm::radians(angle);
                
                float dx = std::cos(rad) * radius;
                float dz = std::sin(rad) * radius;
                glm::vec3 leafPosition(position.x + dx, ringHeight, position.z + dz);
                
                float yawDeg = glm::degrees(std::atan2(dx, dz));
                glm::vec3 leafRotation(leafTilt, yawDeg, 0.0f);
                
                AddDecor(chunk, mesh, name, leafPosition, leafRotation);
            }
        }
        
        return true;
    }
    
    if (pattern == "pattern_birch_leaves") {
        unsigned int clusters = Random.Range(5, 9);
        
        float leafSpreadArea  = 2.0f;
        float leafSpreadHeight = 1.5f;
        
        unsigned int leavesPerClusterMin = 4;
        unsigned int leavesPerClusterMax = 8;
        
        float baseLeafWidth  = leafWidth  * 0.75f;
        float baseLeafHeight = leafHeight * 0.75f;
        
        float radius = glm::max(leafSpreadArea * 0.1f, 0.3f);
        
        float canopyTopY = position.y;
        
        float halfTreeDrop = leafSpreadHeight * 4.0f;
        
        float canopyThickness = leafSpreadHeight * 1.6f;
        
        canopyThickness *= 1.20f;
        
        float canopyBotY = canopyTopY - halfTreeDrop;
        
        float trunkBaseY = position.y;
        float trunkFloorY = trunkBaseY;
        
        float denom = (clusters > 1) ? float(clusters - 1) : 1.0f;
        
        for (unsigned int c = 0; c < clusters; c++) {
            float t = float(c) / denom; // 0..1
            
            float ringY = canopyTopY + (canopyBotY - canopyTopY) * t;
            
            float yaw = Random.Range(0.0f, 360.0f);
            float rad = glm::radians(yaw);
            float rMul = (0.7f + Random.Range(0.0f, 0.4f));
            float dx  = std::cos(rad) * (radius * rMul);
            float dz  = std::sin(rad) * (radius * rMul);
            glm::vec3 clusterCenter(position.x + dx, ringY, position.z + dz);
            
            unsigned int leavesInCluster = Random.Range(leavesPerClusterMin, leavesPerClusterMax);
            for (unsigned int l = 0; l < leavesInCluster; l++) {
                
                float yJitter = Random.Range(-leafSpreadHeight * 0.35f, leafSpreadHeight * 0.15f);
                
                glm::vec3 leafPos = clusterCenter + glm::vec3(
                    Random.Range(-radius * 0.2f, radius * 0.2f),
                    yJitter,
                    Random.Range(-radius * 0.2f, radius * 0.2f)
                );
                
                float maxY = canopyTopY;
                float minY = canopyBotY - (leafSpreadHeight * 0.25f);
                if (leafPos.y > maxY) leafPos.y = maxY;
                if (leafPos.y < minY) leafPos.y = minY;
                
                glm::vec3 leafRot(Random.Range(0.0f, 360.0f), Random.Range(0.0f, 360.0f), Random.Range(0.0f, 360.0f));
                
                float scaleMul = 0.75f + Random.Range(0.0f, 0.25f);
                glm::vec3 leafScale(baseLeafWidth * scaleMul, baseLeafHeight * scaleMul, baseLeafWidth  * scaleMul);
                
                AddDecor(chunk, mesh, name, leafPos, leafRot);
            }
        }
        
        return true;
    }
    
    if (pattern == "pattern_willow_leaves") {
        unsigned int leavesMin = 10;
        unsigned int leavesMax = 16;
        
        float leafSpreadArea   = 1.3f;
        float leafSpreadHeight = 0.4f;
        
        float crownY = position.y;
        float radius = glm::max(leafSpreadArea, 0.6f);
        
        unsigned int leafCount = Random.Range(leavesMin, leavesMax);
        
        unsigned int chainLenMin = 10;
        unsigned int chainLenMax = 14;
        float chainDrop          = leafSpreadHeight;
        float chainOutwardPull   = 0.02f;
        float chainJitterXZ      = 0.2f;
        
        for (unsigned int i = 0; i < leafCount; i++) {
            
            float angleDeg = (360.0f / float(leafCount)) * float(i);
            
            angleDeg += Random.Range(-24.0f, 24.0f);
            float r = radius * Random.Range(0.7f, 0.9f);
            
            float angRad = glm::radians(angleDeg);
            float outX = std::cos(angRad);
            float outZ = std::sin(angRad);
            
            glm::vec3 basePos(position.x + outX * r, 
                              crownY + Random.Range(-0.3f, 0.5f), 
                              position.z + outZ * r);
            
            unsigned int chainLen = Random.Range(chainLenMin, chainLenMax);
            
            for (unsigned int s = 0; s < chainLen; s++) {
                float t = (chainLen <= 1) ? 0.0f : float(s) / float(chainLen - 1); // 0..1 down chain
                
                float y = basePos.y - (float(s) * chainDrop);
                
                float drift = chainOutwardPull * float(s);
                float dx = outX * drift + Random.Range(-chainJitterXZ, chainJitterXZ);
                float dz = outZ * drift + Random.Range(-chainJitterXZ, chainJitterXZ);
                
                glm::vec3 leafPos(basePos.x + dx, y, basePos.z + dz);
                
                float yawDeg = glm::degrees(std::atan2(outX, outZ));
                float pitchDeg = 20.0f + 55.0f * t;             // 20..75 degrees
                float rollDeg  = Random.Range(-10.0f, 10.0f);
                
                glm::vec3 leafRot(pitchDeg, yawDeg, rollDeg);
                
                AddDecor(chunk, mesh, name, leafPos, leafRot);
            }
        }
        
        return true;
    }
    if (pattern == "pattern_palm_leaves") {
        /*
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
            
            AddDecor(chunk, mesh, name, leafPos, leafRot);
        }
        */
        return true;
    }
    
    
    if (pattern == "pattern_acacia_leaves") {
        /*
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
                
                AddDecor(chunk, mesh, name, leafPos, leafRot);
            }
        }
        */
        return true;
    }
    
    if (pattern == "pattern_spruce_leaves") {
        /*
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
                
                AddDecor(chunk, mesh, name, pos, rot);
            }
        }
        */
        return true;
    }
    
    // No leaf type was selected
    //Log.Write("! Leaf type not defined - " + name);
    return false;
}



/*
bool ChunkManager::AddDecorGrass(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    float grassWidth = 0.0f;
    float grassHeight = 0.0f;
    
    Color colorMin = Colors.white;
    Color colorMax = Colors.white;
    
    for (std::unordered_map<std::string, ClassDefinitionGrass>::iterator it = world.definitionGrass.begin(); it != world.definitionGrass.end(); ++it) {
        if (it->first != name) 
            continue;
        ClassDefinitionGrass& definition = it->second;
        
        grassWidth  = definition.width;
        grassHeight = definition.height;
        colorMin    = definition.colorMin;
        colorMax    = definition.colorMax;
        break;
    }
    
    Color finalColor = Colors.Range(colorMin, colorMax);
    glm::vec3 scale(grassWidth, grassHeight, grassWidth);
    glm::vec3 rotation(0.0f, 0.0f, 0.0f);
    
    AddDecor(chunk, staticMesh, DecorationMesh::Grass, DecorationStatic::Grass, position + glm::vec3(0.0f, grassHeight, 0.0f), rotation, scale, finalColor);
    return true;
}

bool ChunkManager::AddDecorLog(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    float grassWidth = 0.0f;
    float grassHeight = 0.0f;
    
    Color colorMin = Colors.white;
    Color colorMax = Colors.white;
    
    for (std::unordered_map<std::string, ClassDefinitionLog>::iterator it = world.definitionLog.begin(); it != world.definitionLog.end(); ++it) {
        if (it->first != name) 
            continue;
        ClassDefinitionLog& definition = it->second;
        
        grassWidth  = definition.width;
        grassHeight = definition.height;
        colorMin    = definition.colorMin;
        colorMax    = definition.colorMax;
        break;
    }
    
    Color finalColor = Colors.Range(colorMin, colorMax);
    glm::vec3 scale(grassWidth, grassHeight, grassWidth);
    glm::vec3 rotation(0.0f, 0.0f, 0.0f);
    
    AddDecor(chunk, staticMesh, DecorationMesh::Log, DecorationStatic::LogOak, position + glm::vec3(0.0f, grassHeight, 0.0f), rotation, scale, finalColor);
    return true;
}


bool ChunkManager::AddDecorLeaf(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    float grassWidth = 0.0f;
    float grassHeight = 0.0f;
    
    Color colorMin = Colors.white;
    Color colorMax = Colors.white;
    
    for (std::unordered_map<std::string, ClassDefinitionLeaf>::iterator it = world.definitionLeaf.begin(); it != world.definitionLeaf.end(); ++it) {
        if (it->first != name) 
            continue;
        ClassDefinitionLeaf& definition = it->second;
        
        grassWidth  = definition.width;
        grassHeight = definition.height;
        colorMin    = definition.colorMin;
        colorMax    = definition.colorMax;
        break;
    }
    
    Color finalColor = Colors.Range(colorMin, colorMax);
    glm::vec3 scale(grassWidth, grassHeight, grassWidth);
    glm::vec3 rotation(0.0f, 0.0f, 0.0f);
    
    AddDecor(chunk, staticMesh, DecorationMesh::Leaf, DecorationStatic::LeafOak, position + glm::vec3(0.0f, grassHeight, 0.0f), rotation, scale, finalColor);
    return true;
}




bool ChunkManager::AddDecorStructure(Chunk* chunk, Mesh* staticMesh, glm::vec3 position, std::string name) {
    Structure* structure = nullptr;
    // Find the matching structure by name
    for (unsigned int i = 0; i < world.structures.size(); i++) {
        if (world.structures[i].name != name) 
            continue;
        structure = &world.structures[i];
        break;
    }
    if (structure == nullptr)
        return false;
    
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
            return false;
    }
    chunk->structureLocations.push_back(structurePosition);
    
    // Check chunk boundary
    glm::vec3 chunkPos(chunk->x, 0, chunk->y);
    if (glm::distance(chunkPos, structurePosition) > chunkSize * 0.6f) 
        return false;
    
    // Place structure parts
    for (unsigned int p = 0; p < structure->parts.size(); p++) {
        StructurePart part = structure->parts[p];
        
        glm::vec3 translation = position + glm::vec3(part.offset.x, part.offset.y, part.offset.z);
        glm::vec3 orientation = part.rotation;
        glm::vec3 scale = part.scale;
        Color color = part.color;
        
        AddDecor(chunk, staticMesh, DecorationMesh::Log, DecorationStatic::LogOak, translation, orientation, scale, color);
    }
    
    // Spawn actors
    if (structure->actorCountMin >= structure->actorCountMax) 
        return false;
    if (structure->actorCountMax == 0) 
        return false;
    
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
    return true;
}
*/
