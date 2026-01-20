#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

void ChunkManager::Update(void) {
    if (Engine.cameraController == nullptr || !world.doGenerateChunks)
        return;
    
    glm::vec3 playerPosition = Engine.cameraController->GetPosition();
    
    InitializePlayerHeight(playerPosition);
    UpdateFogSettings(playerPosition);
    DestroyChunks(playerPosition);
    GenerateChunks(playerPosition);
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

static glm::vec2 SafeNormalize2(const glm::vec2& v) {
    float len2 = glm::dot(v, v);
    if (len2 <= 0.000001f)
        return glm::vec2(0.0f, 0.0f);
    return v / std::sqrt(len2);
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
    
        camForwardXZ = SafeNormalize2(glm::vec2(camPtr->forward.x, camPtr->forward.z));
        camPosXZ     = glm::vec2(camPtr->transform.position.x, camPtr->transform.position.z);
    }
    
    // --- Old system constants ---
    const int chunkSizeSub = chunkSize;
    const float streamRadius = (renderDistance * (chunkSizeSub * 0.5f));
    const float inflateAabb  = (float)chunkSizeSub * 0.75f;
    
    // How “forward” must a chunk be to be considered?
    //  1.0 = dead ahead, 0.0 = sideways, -1.0 = directly behind
    // Set to 0.0 for “front hemisphere only” (more Minecraft-like).
    // Set to -0.2f if you want some behind/side padding while rotating.
    const float minForwardDot = 0.0f;
    
    // Optional budget so you don’t spawn tons of chunks in one frame.
    // Set to a big number if you want “old behavior speed”.
    const int maxCreatesPerTick = 4;
    int createsThisTick = 0;
    
    // --- Center chunk indices (matches your old rounding behavior) ---
    const float halfChunk = (chunkSizeSub * 0.5f);
    
    int centerChunkX = (int)Math.Round((playerPosition.x + halfChunk) / (float)chunkSizeSub);
    int centerChunkZ = (int)Math.Round((playerPosition.z + halfChunk) / (float)chunkSizeSub);
    
    // Old grid covered roughly +/- renderDistance/2 around center
    int maxRing = renderDistance / 2;
    if (maxRing < 1) maxRing = 1;
    
    glm::vec2 playerPos(playerPosition.x, playerPosition.z);
    
    // ---- Forward-ring scan ----
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
            
            glm::vec2 dir = SafeNormalize2(toChunk);
            float dotF = glm::dot(dir, camForwardXZ);
            
            if (dotF < minForwardDot)
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
            
            // Frustum test (optional)
            if (useFrustum) {
                if (!ChunkIntersectsFrustum(frustum, chunkPos.x, chunkPos.y, (float)chunkSizeSub, inflateAabb))
                    continue;
            }
            
            // ---- Your original “found chunk” block ----
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
                        
                        Engine.sceneMain->AddMeshRendererToSceneRoot(chunkRenderer,  RENDER_QUEUE_GEOMETRY);
                        Engine.sceneMain->AddMeshRendererToSceneRoot(staticRenderer, RENDER_QUEUE_GEOMETRY);
                    }
                }
                
                // Completion as-is
                if (chunk->isGenerated && !chunk->isComplete) {
                    chunk->isComplete = true;
                    
                    GenerateChunkBiomes(chunk);
                    
                    int chunkSZ = chunkSize + 1;
                    
                    Mesh* chunkMesh = chunk->gameObject->GetComponent<MeshRenderer>()->mesh;
                    AddHeightFieldToMesh(chunkMesh, chunk->heightField, chunk->colorField, chunkSZ, chunkSZ, 0, 0, 1, 1);
                    chunkMesh->Load();
                    
                    // Physics
                    chunk->rigidBody = Physics.world->createRigidBody(rp3d::Transform::identity());
                    
                    chunk->rigidBody->setAngularLockAxisFactor(rp3d::Vector3(0, 0, 0));
                    chunk->rigidBody->setLinearLockAxisFactor(rp3d::Vector3(0, 0, 0));
                    chunk->rigidBody->setType(rp3d::BodyType::STATIC);
                    
                    rp3d::Transform bodyTransform = rp3d::Transform::identity();
                    bodyTransform.setPosition(rp3d::Vector3(chunk->x, 0, chunk->y));
                    chunk->rigidBody->setTransform(bodyTransform);
                    
                    MeshCollider* meshCollider = Physics.CreateHeightFieldMap(chunk->heightField, chunkSZ, chunkSZ, 1, 1, 1);
                    
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

