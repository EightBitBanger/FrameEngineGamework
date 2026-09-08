#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <unordered_set>

#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <GameEngineFramework/Engine/EngineSystems.h>

void ChunkManager::Update(float deltaTime) {
    if (Engine.cameraController == nullptr || !world.doGenerateChunks)
        return;
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    
    KeepPlayerAboveGround(playerPosition, 0.5f);
    InitializePlayerHeight(playerPosition);
    
    UpdateFogSettings(playerPosition);
    
    DestroyChunks(playerPosition);
    GenerateChunks(playerPosition);
    
    UpdateStaticObjects(deltaTime);
    
    UpdatePickupObjects(deltaTime);
    
    ProcessPendingRequests();
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

void ChunkManager::UpdatePickupObjects(float deltaTime) {
    if (chunks.Size() == 0) 
        return;
    
    for (unsigned int c = 0; c < chunks.Size(); c++) {
        Chunk* chunk = chunks[c];
        if (!(chunk->flags & CHUNK_IS_ACTIVE)) 
            continue;
        
        if (chunk->doUpdate) {
            chunk->doUpdate = false;
            RebuildPickupMesh(chunk);
        }
    }
}

void ChunkManager::UpdateStaticObjects(float deltaTime) {
    if (chunks.Size() == 0) 
        return;
    
    const float maxAllowedDelta = 0.01f;
    float growthDelta = std::min(deltaTime, maxAllowedDelta);
    
    const float plantGrowthMul = 1.0f;
    float baseGrowthStep = growthDelta * plantGrowthMul * AI.GetTimeScale();
    
    std::unordered_set<Chunk*> modifiedChunks;
    
    for (unsigned int c = 0; c < chunks.Size(); c++) {
        Chunk* chunk = chunks[c];
        if (!(chunk->flags & CHUNK_IS_ACTIVE) || chunk->animatedStatics.empty()) 
            continue;
        
        bool chunkModified = false;
        Mesh* staticMesh = chunk->staticObject->GetComponent<MeshRenderer>()->mesh;
        
        for (int i = (int)chunk->animatedStatics.size() - 1; i >= 0; i--) {
            StaticAnimation& ref = chunk->animatedStatics[i];
            
            if (ref.staticIndex >= chunk->statics.size()) {
                chunk->animatedStatics.erase(chunk->animatedStatics.begin() + i);
                continue;
            }
            
            StaticObject& obj = chunk->statics[ref.staticIndex];
            std::string typeName = world.classIndexToName[obj.type];
            ClassDefinition& def = world.classDefinitions[typeName];
            
            // Retain original growth scaling factors
            float maxScale = 1.0f;
            float baseScaleY = maxScale * 0.1f;
            float oldScaleY = obj.scale.y;
            
            float randomVariance = (float)Random.Range(1, 1000) * 0.001f;
            float appliedGrowthStep = baseGrowthStep * randomVariance;
            
            obj.scale.y = std::min(obj.scale.y + appliedGrowthStep, maxScale);
            float factorY = (oldScaleY > 0.0f) ? (obj.scale.y / oldScaleY) : 1.0f;
            
            float progress = glm::clamp((obj.scale.y - baseScaleY) / (maxScale - baseScaleY), 0.0f, 1.0f);
            Color currentColor = Colors.Lerp(def.colorMin, def.colorMax, progress);
            obj.color = currentColor.ToVec3();
            
            staticMesh->ChangeSubMeshScale(ref.staticIndex, 1.0f, factorY, 1.0f);
            staticMesh->ChangeSubMeshColor(ref.staticIndex, currentColor);
            
            chunkModified = true;
            
            if (obj.scale.y >= maxScale) {
                const float saturationValue = 0.3f;
                
                std::string red   = Float.ToString(def.colorMax.r);
                std::string green = Float.ToString(def.colorMax.g);
                std::string blue  = Float.ToString(def.colorMax.b);
                
                std::string meshType = def.mesh;
                
                std::string stackMax    = UInt.ToString(def.stackMax);
                std::string saturation  = Float.ToString(saturationValue);
                
                
                // Use the exact dimensions the static object reached upon completion
                float itemWidth  = obj.scale.x;
                float itemHeight = obj.scale.y;
                
                std::string buildPart = "build: " + meshType + ": 0.0,0.0,0.0: " + 
                Float.ToString(itemWidth) + "," + Float.ToString(itemHeight) + "," + Float.ToString(itemWidth) +
                ": " + red + "," + green + "," + blue;
                
                std::string itemData  = "name:"+typeName+";"+
                                        "crop;"+
                                        "stackMax:"+stackMax+";"+
                                        "saturation:"+saturation+";"+
                                        buildPart;
                
                glm::vec3 position = obj.position + glm::vec3(chunk->x, 0.0f, chunk->y);
                
                if (RemoveDecorByIndex(chunk, ref.staticIndex, false)) {
                    // Determine yield count
                    int dropCount = Random.Range(4, 8);
                    const float minRadius = 0.25f;
                    const float maxRadius = 0.3f;
                    
                    for (int d = 0; d < dropCount; d++) {
                        // Generate a random angle and distance around the plant
                        float angleDeg = Random.Range(0.0f, 360.0f);
                        float distance = Random.Range(minRadius, maxRadius);
                        float rad = glm::radians(angleDeg);
                        
                        glm::vec3 dropPos = position;
                        dropPos.x += std::cos(rad) * distance;
                        dropPos.z += std::sin(rad) * distance;
                        
                        // Raycast down to prevent pickups from floating or sinking into slopes
                        Hit groundHit;
                        glm::vec3 rayOrigin = dropPos + glm::vec3(0.0f, 2.0f, 0.0f);
                        if (Physics.Raycast(rayOrigin, glm::vec3(0.0f, -1.0f, 0.0f), 5.0f, groundHit, LayerMask::Ground)) {
                            dropPos.y = groundHit.point.y;
                        }
                        
                        // Add randomized yaw rotation for visual variety
                        glm::vec3 dropRot(0.0f, Random.Range(0.0f, 360.0f), 0.0f);
                        
                        PlacePickupAt(itemData, dropPos, dropRot);
                    }
                }
            }
        }
        
        if (chunkModified) {
            modifiedChunks.insert(chunk);
        }
    }
    
    // Push updated mesh data to GPU only once per affected chunk
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
                
                // Fade-in
                if (!(chunk->flags & CHUNK_IS_ACTIVE)) {
                    chunk->fadeIn += 1.0f;
                    
                    if (chunk->fadeIn > 10.0f) {
                        chunk->flags |= CHUNK_IS_ACTIVE;
                        
                        MeshRenderer* chunkRenderer  = chunk->gameObject->GetComponent<MeshRenderer>();
                        MeshRenderer* staticRenderer = chunk->staticObject->GetComponent<MeshRenderer>();
                        MeshRenderer* pickupRenderer = chunk->pickupObject->GetComponent<MeshRenderer>();
                        
                        chunkRenderer->isActive  = true;
                        staticRenderer->isActive = true;
                        pickupRenderer->isActive = true;
                        
                        Engine.sceneMain->AddMeshRendererToSceneRoot(chunkRenderer,  RENDER_QUEUE_GEOMETRY);
                        Engine.sceneMain->AddMeshRendererToSceneRoot(staticRenderer, RENDER_QUEUE_GEOMETRY);
                        Engine.sceneMain->AddMeshRendererToSceneRoot(pickupRenderer, RENDER_QUEUE_GEOMETRY);
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

void ChunkManager::KeepPlayerAboveGround(glm::vec3 &playerPosition, float playerHeightOffset) {
    Hit hit;
    // Cast downwards from slightly above the player's current position
    glm::vec3 rayOrigin = playerPosition + glm::vec3(0.0f, 2.0f, 0.0f);
    
    if (Physics.Raycast(rayOrigin, glm::vec3(0.0f, -1.0f, 0.0f), 10000.0f, hit, LayerMask::Ground)) {
        float minHeight = hit.point.y + playerHeightOffset;
        if (playerPosition.y < minHeight) {
            playerPosition.y = minHeight;
            Engine.cameraController->SetPosition(playerPosition);
        }
    }
}

void ChunkManager::ProcessPendingRequests() {
    // Process Pickups
    std::vector<PendingRequest> localPickups;
    {
        std::lock_guard<std::mutex> lock(mPendingRequestsMutex);
        if (!mPendingRequests.empty()) {
            localPickups.swap(mPendingRequests);
        }
    }
    for (const auto& pending : localPickups) {
        PlacePickupAt(pending.classification, pending.position, pending.rotation);
    }
    
    // Process static objects
    std::vector<PendingRequest> localStatics;
    {
        std::lock_guard<std::mutex> lock(mPendingStaticRequestsMutex);
        if (!mPendingStaticRequests.empty()) {
            localStatics.swap(mPendingStaticRequests);
        }
    }
    for (const auto& pending : localStatics) {
        PlaceDecorAt(pending.classification, pending.position, pending.rotation);
    }
}
