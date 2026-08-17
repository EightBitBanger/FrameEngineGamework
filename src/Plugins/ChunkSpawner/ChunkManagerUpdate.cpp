#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <unordered_set>

#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <GameEngineFramework/Engine/EngineSystems.h>

void ChunkManager::Update(float deltaTime) {
    if (Engine.cameraController == nullptr || !world.doGenerateChunks)
        return;
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    
    InitializePlayerHeight(playerPosition);
    
    UpdateFogSettings(playerPosition);
    
    DestroyChunks(playerPosition);
    
    GenerateChunks(playerPosition);
    
    UpdateStaticObjects(deltaTime);
}

static Frustum BuildStreamingFrustumFromCamera(Camera& cam) {
    float fovDeg = cam.fov + cam.frustumOverlap;
    float fovRad = glm::radians(fovDeg);

    glm::vec3 camPos = cam.transform.position - cam.forward * cam.frustumOffset;

    glm::mat4 view = glm::lookAt(camPos, camPos + cam.forward, cam.up);
    glm::mat4 proj = glm::perspective(fovRad, cam.aspect, cam.clipNear, cam.clipFar);

    glm::mat4 vp = proj * view;
    return Renderer.FrustumExtractPlanes(vp);
}

static bool ChunkIntersectsFrustum(Frustum& frustum,
                                  float chunkWorldX,
                                  float chunkWorldZ,
                                  float chunkSizeWorld,
                                  float inflateWorld)
{
    glm::vec3 min(chunkWorldX - inflateWorld, -10000.0f, chunkWorldZ - inflateWorld);
    glm::vec3 max(chunkWorldX + chunkSizeWorld + inflateWorld,  10000.0f, chunkWorldZ + chunkSizeWorld + inflateWorld);
    return Renderer.FrustumCheckAABB(frustum, min, max);
}

struct RingCandidate {
    int dx;
    int dz;
    float score;
    float dist2;
};

static glm::vec2 NormalizeSqrt(const glm::vec2& v) {
    float len2 = glm::dot(v, v);
    if (len2 <= 0.000001f)
        return glm::vec2(0.0f, 0.0f);
    return v / std::sqrt(len2);
}

void ChunkManager::UpdateStaticObjects(float deltaTime) {
    if (chunks.Size() == 0) 
        return;
    
    const float maxAllowedDelta = 0.01f;
    float growthDelta = std::min(deltaTime, maxAllowedDelta);
    
    const float ticksPerPlantPerSecond  = 90.3f;
    static float tickAccumulator        = 0.0f;
    
    std::vector<Chunk*> activeChunks;
    size_t totalActivePlants = 0;
    for (unsigned int c = 0; c < chunks.Size(); c++) {
        Chunk* chunk = chunks[c];
        if (chunk->isActive && !chunk->animatedStatics.empty()) {
            activeChunks.push_back(chunk);
            totalActivePlants += chunk->animatedStatics.size();
        }
    }
    
    if (totalActivePlants == 0)
        return;
    
    float ticksToRunFloat = (float)totalActivePlants * ticksPerPlantPerSecond * growthDelta + tickAccumulator;
    unsigned int numTicks = (unsigned int)ticksToRunFloat;
    tickAccumulator       = ticksToRunFloat - (float)numTicks;
    
    std::unordered_set<Chunk*> modifiedChunks;
    
    for (unsigned int t = 0; t < numTicks; t++) {
        size_t currentTotal = 0;
        for (Chunk* chunk : activeChunks) {
            currentTotal += chunk->animatedStatics.size();
        }
        
        if (currentTotal == 0)
            break;
        
        int globalPlantIdx = Random.Range(0, (int)currentTotal - 1);
        
        Chunk* targetChunk = nullptr;
        size_t localPlantIdx = 0;
        for (Chunk* chunk : activeChunks) {
            if ((size_t)globalPlantIdx < chunk->animatedStatics.size()) {
                targetChunk   = chunk;
                localPlantIdx = (size_t)globalPlantIdx;
                break;
            }
            globalPlantIdx -= (int)chunk->animatedStatics.size();
        }
        
        if (!targetChunk || localPlantIdx >= targetChunk->animatedStatics.size())
            continue;
        
        StaticAnimation& ref = targetChunk->animatedStatics[localPlantIdx];
        
        if (ref.staticIndex >= targetChunk->statics.size()) {
            targetChunk->animatedStatics.erase(targetChunk->animatedStatics.begin() + localPlantIdx);
            continue;
        }
        
        StaticObject& obj = targetChunk->statics[ref.staticIndex];
        std::string typeName = world.classIndexToName[obj.type];
        ClassDefinition& def = world.classDefinitions[typeName];
        
        // Target full height from ClassDefinition
        float maxScale = 1.0f;
        float baseScaleY = maxScale * 0.1f; // Sprout starting height
        
        // Store old scale before incrementing
        float oldScaleY = obj.scale.y;
        
        // Make growth step relative
        float growthStep = (maxScale - baseScaleY) * 0.1f; 
        obj.scale.y = std::min(obj.scale.y + growthStep, maxScale);
        
        // Calculate proportional factor to scale current vertices in mesh buffer
        float factorY = (oldScaleY > 0.0f) ? (obj.scale.y / oldScaleY) : 1.0f;
        
        // Update color progression
        float progress = glm::clamp((obj.scale.y - baseScaleY) / (maxScale - baseScaleY), 0.0f, 1.0f);
        Color currentColor = Colors.Lerp(def.colorMin, def.colorMax, progress);
        obj.color = currentColor.ToVec3();
        
        // Apply scale factor (X and Z unchanged at 1.0, Y scaled by factorY)
        Mesh* staticMesh = targetChunk->staticObject->GetComponent<MeshRenderer>()->mesh;
        staticMesh->ChangeSubMeshScale(ref.staticIndex, 1.0f, factorY, 1.0f);
        staticMesh->ChangeSubMeshColor(ref.staticIndex, currentColor);
        
        modifiedChunks.insert(targetChunk);
        
        // Handle full growth completion
        if (obj.scale.y >= maxScale) {
            obj.function = 0;
            targetChunk->animatedStatics.erase(targetChunk->animatedStatics.begin() + localPlantIdx);
        }
    }
    
    // Push updated mesh data to GPU
    for (Chunk* chunk : modifiedChunks) {
        Mesh* staticMesh = chunk->staticObject->GetComponent<MeshRenderer>()->mesh;
        staticMesh->Load();
    }
}

void ChunkManager::GenerateChunks(const glm::vec3& playerPosition) {
    if (!mux.try_lock())
        return;
    if (!world.doGenerateChunks) {
        mux.unlock();
        return;
    }
    
    Camera* camPtr = Engine.sceneMain->camera;
    bool useFrustum = (camPtr != nullptr);
    
    Frustum frustum;
    glm::vec2 camForwardXZ(0.0f, 1.0f);
    glm::vec2 camPosXZ(playerPosition.x, playerPosition.z);
    
    if (useFrustum) {
        frustum = BuildStreamingFrustumFromCamera(*camPtr);
    
        camForwardXZ = NormalizeSqrt(glm::vec2(camPtr->forward.x, camPtr->forward.z));
        camPosXZ     = glm::vec2(camPtr->transform.position.x, camPtr->transform.position.z);
    }
    
    const int chunkSizeSub        = chunkSize;
    const float streamRadius      = (renderDistance * (chunkSizeSub * 0.5f));
    const float inflateAabb       = (float)chunkSizeSub * 0.75f;
    const float minForwardDot     = 0.0f;
    const int maxCreatesPerTick   = 4;
    int createsThisTick           = 0;
    
    const int forceLoadGridSize   = 1;
    const int forceLoadRadius     = forceLoadGridSize / 2;
    
    // Center chunk indices
    const float halfChunk = (chunkSizeSub * 0.5f);
    int centerChunkX = (int)Math.Round(playerPosition.x / (float)chunkSizeSub);
    int centerChunkZ = (int)Math.Round(playerPosition.z / (float)chunkSizeSub);
    
    int maxRing = renderDistance / 2;
    if (maxRing < 1) maxRing = 1;
    
    glm::vec2 playerPos(playerPosition.x, playerPosition.z);
    
    // Forward-ring scan
    for (int ring = 0; ring <= maxRing; ++ring) {
        std::vector<RingCandidate> candidates;
        candidates.reserve((ring == 0) ? 1 : ring * 8);
        
        auto pushCandidate = [&](int dx, int dz) {
            int chunkIndexX = centerChunkX + dx;
            int chunkIndexZ = centerChunkZ + dz;
            
            float chunkWorldX = (float)chunkIndexX * (float)chunkSizeSub;
            float chunkWorldZ = (float)chunkIndexZ * (float)chunkSizeSub;
            
            // Distance check is cheap (do this before frustum)
            glm::vec2 chunkPos(chunkWorldX, chunkWorldZ);
            
            if (glm::distance(chunkPos, playerPos) > streamRadius)
                return;
            
            // Forward score (use chunk center)
            glm::vec2 chunkCenterXZ(chunkWorldX + halfChunk, chunkWorldZ + halfChunk);
            glm::vec2 toChunk = chunkCenterXZ - camPosXZ;
            
            glm::vec2 dir = NormalizeSqrt(toChunk);
            float dotF = glm::dot(dir, camForwardXZ);
            
            // Force load the chunks immediately around the player
            bool isCenterChunk = (std::abs(dx) <= forceLoadRadius && std::abs(dz) <= forceLoadRadius);
            
            if (!isCenterChunk && dotF < minForwardDot)
                return;
            
            float dist2 = glm::dot(toChunk, toChunk);
            
            RingCandidate cand;
            cand.dx = dx;
            cand.dz = dz;
            cand.score = dotF;
            cand.dist2 = dist2;
            candidates.push_back(cand);
        };
        
        // Ring perimeter: all (dx,dz) where max(|dx|,|dz|) == ring
        if (ring == 0) {
            pushCandidate(0, 0);
        } else {
            for (int dx = -ring; dx <= ring; ++dx) {
                pushCandidate(dx, -ring);
                pushCandidate(dx,  ring);
            }
            for (int dz = -ring + 1; dz <= ring - 1; ++dz) {
                pushCandidate(-ring, dz);
                pushCandidate( ring, dz);
            }
        }
        
        // Sort: most forward first, then nearest
        std::sort(candidates.begin(), candidates.end(),
                [](const RingCandidate& a, const RingCandidate& b) {
                    if (a.score != b.score) return a.score > b.score;
                    return a.dist2 < b.dist2;
                });
        
        // Process this ring’s candidates in forward order
        for (size_t i = 0; i < candidates.size(); ++i) {
            int dx = candidates[i].dx;
            int dz = candidates[i].dz;
            
            int chunkIndexX = centerChunkX + dx;
            int chunkIndexZ = centerChunkZ + dz;
            
            float chunkWorldX = (float)chunkIndexX * (float)chunkSizeSub;
            float chunkWorldZ = (float)chunkIndexZ * (float)chunkSizeSub;
            
            glm::vec2 chunkPos(chunkWorldX, chunkWorldZ);
            
            bool isCenterChunk = (std::abs(dx) <= forceLoadRadius && std::abs(dz) <= forceLoadRadius);
            
            if (useFrustum && !isCenterChunk) {
                if (!ChunkIntersectsFrustum(frustum, chunkPos.x, chunkPos.y, (float)chunkSizeSub, inflateAabb))
                    continue;
            }
            
            if (IsChunkFound(chunkPos)) {
                Chunk* chunk = FindChunk(chunkPos.x, chunkPos.y);
                if (chunk == nullptr)
                    continue;
                
                // Fade-in only when it’s wanted by view
                if (!chunk->isActive) {
                    chunk->fadeIn += 1;
                    
                    if (chunk->fadeIn > 10) {
                        chunk->isActive = true;
                        
                        MeshRenderer* chunkRenderer  = chunk->gameObject->GetComponent<MeshRenderer>();
                        MeshRenderer* staticRenderer = chunk->staticObject->GetComponent<MeshRenderer>();
                        
                        chunkRenderer->isActive = true;
                        staticRenderer->isActive = true;
                        
                        Engine.sceneMain->AddMeshRendererToSceneRoot(chunkRenderer,  RENDER_QUEUE_GEOMETRY);
                        Engine.sceneMain->AddMeshRendererToSceneRoot(staticRenderer, RENDER_QUEUE_GEOMETRY);
                    }
                }
                
                // Completion as-is
                if (chunk->isGenerated && !chunk->isComplete) {
                    chunk->isComplete = true;
                    
                    GenerateChunkBiomes(chunk);
                    
                    int chunkSZ = chunkSize + 1;
                    
                    // Main chunk mesh
                    
                    MeshRenderer* chunkRenderer = chunk->gameObject->GetComponent<MeshRenderer>();
                    Mesh* chunkMesh = chunkRenderer->mesh;
                    
                    generation.AddHeightFieldToMesh(chunkMesh, chunk->heightField, chunk->colorField, chunkSZ, chunkSZ, 0, 0, 1, 1);
                    chunkMesh->Load();
                    
                    // Level of detail
                    // Seems to not work
                    /*
                    LevelOfDetail lod;
                    lod.distance = 150.0f;
                    
                    SubMesh subMesh;
                    if (chunkMesh->GenerateSimplifiedLOD(0, 1.0f, subMesh)) {
                        lod.mesh = Engine.Create<Mesh>();
                        lod.mesh->AddSubMesh(0, 0, 0, subMesh, true);
                        chunkRenderer->AddLevelOfDetail(lod);
                        
                        lod.mesh->Load();
                    } else {
                        Engine.console.Print("No output senior");
                    }
                    */
                    
                    // Physics
                    chunk->rigidBody = Physics.world->createRigidBody(rp3d::Transform::identity());
                    
                    chunk->rigidBody->setAngularLockAxisFactor(rp3d::Vector3(0, 0, 0));
                    chunk->rigidBody->setLinearLockAxisFactor(rp3d::Vector3(0, 0, 0));
                    chunk->rigidBody->setType(rp3d::BodyType::STATIC);
                    
                    rp3d::Transform bodyTransform = rp3d::Transform::identity();
                    bodyTransform.setPosition(rp3d::Vector3(chunk->x, 0, chunk->y));
                    chunk->rigidBody->setTransform(bodyTransform);
                    
                    MeshCollider* meshCollider = Physics.CreateHeightFieldMap(chunk->heightField, chunkSZ, chunkSZ, glm::vec3(1, 1, 1));
                    
                    rp3d::Collider* bodyCollider =
                        chunk->rigidBody->addCollider(meshCollider->heightFieldShape, rp3d::Transform::identity());
                    
                    bodyCollider->setUserData((void*)chunk->gameObject);
                    bodyCollider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
                    bodyCollider->setCollideWithMaskBits((unsigned short)CollisionMask::Entity);
                    
                    chunk->bodyCollider = bodyCollider;
                    chunk->meshCollider = meshCollider;
                    
                    std::string filename       = Int.ToString(chunkPos.x) + "_" + Int.ToString(chunkPos.y);
                    std::string chunkFilename  = "worlds/" + world.name + "/chunks/" + filename;
                    std::string staticFilename = "worlds/" + world.name + "/static/" + filename;
                    
                    if (Serializer.CheckExists(chunkFilename) || Serializer.CheckExists(staticFilename)) {
                        LoadChunk(chunk);
                    } else {
                        chunk->seed = worldSeed + ((chunkPos.x * 2) + (chunkPos.y * 4) / 2);
                        Random.SetSeed(chunk->seed);
                        Decorate(chunk);
                    }
                    
                    free(chunk->heightField);
                    free(chunk->colorField);
                }
                
                continue;
            }
            
            if (createsThisTick >= maxCreatesPerTick) {
                mux.unlock();
                return;
            }
            
            Chunk* chunk = CreateChunk(chunkPos.x, chunkPos.y);
            generating.push_back(chunk);
            createsThisTick++;
        }
    }
    
    mux.unlock();
}

void ChunkManager::DestroyChunks(const glm::vec3 &playerPosition) {
    unsigned int numberOfChunks = chunks.Size();
    if (numberOfChunks == 0)
        return;
    
    for (unsigned int c = 0; c < numberOfChunks; c++) {
        Chunk* chunk = chunks[c];
        if (chunk->gameObject == nullptr)
            continue;
        
        glm::vec3 chunkPos(chunk->x, 0, chunk->y);
        glm::vec3 playerPos(playerPosition.x, 0, playerPosition.z);
        
        if (glm::distance(chunkPos, playerPos) > (renderDistance * chunkSize) * 1.5f) {
            SaveChunk(chunk, true);
            DestroyChunk(chunk);
        }
    }
}

bool ChunkManager::IsChunkFound(const glm::vec2 &chunkPosition) {
    unsigned int numberOfChunks = chunks.Size();
    for (unsigned int c = 0; c < numberOfChunks; c++) {
        Chunk& chunk = *chunks[c];
        if (glm::vec3(chunk.x, 0, chunk.y) == glm::vec3(chunkPosition.x, 0, chunkPosition.y))
            return true;
    }
    return false;
}


void ChunkManager::UpdateFogSettings(const glm::vec3 &playerPosition) {
    if (playerPosition.y < world.waterLevel) {
        fogWater->fogHeightCutoff = 1000.0f;
        fogWater->fogDensity = 0.8f;
        fogWater->fogBegin = 0.0f;
        fogWater->fogEnd = 24.0f;
        fogWater->fogColorBegin = Colors.blue;
        fogWater->fogColorEnd = Colors.blue;
    } else {
        fogWater->fogHeightCutoff = world.waterLevel;
        fogWater->fogDensity = 0.8f;
        fogWater->fogBegin = 0.0f;
        fogWater->fogEnd = 1.0f;
        fogWater->fogColorBegin = Colors.blue;
        fogWater->fogColorEnd = Colors.blue;
    }
}


void ChunkManager::InitializePlayerHeight(glm::vec3 &playerPosition) {
    if (isInitiated)
        return;
    playerPosition.y = 0;
    
    Hit hit;
    if (Physics.Raycast(playerPosition, glm::vec3(0, -1, 0), 100.0f, hit, LayerMask::Ground)) {
        playerPosition.y = hit.point.y + 3.5f;
        Engine.cameraController->SetPosition(playerPosition);
        isInitiated = true;
    }
}

