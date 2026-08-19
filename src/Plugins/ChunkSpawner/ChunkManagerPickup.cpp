#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <algorithm>

bool RayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& boxMin, const glm::vec3& boxMax, float& outDistance);

bool ChunkManager::QueryPickup(glm::vec3 position, glm::vec3 direction, float maxDistance, std::string& queriedItem) {
    glm::vec3 rayDir = glm::normalize(direction);
    float closestHitDist = maxDistance + 1.0f;
    int bestChunkIndex  = -1;
    int bestPickupIndex = -1;
    
    // Iterate over active chunks in range
    for (unsigned int i = 0; i < chunks.Size(); i++) {
        Chunk& chunk = *chunks[i];
        
        // Fast 2D chunk distance pre-filter
        glm::vec3 chunkPos(chunk.x, 0.0f, chunk.y);
        glm::vec3 pos2D(position.x, 0.0f, position.z);
        if (glm::distance(chunkPos, pos2D) > (chunkSize * 0.707f + maxDistance)) 
            continue;
        
        // Check each pickup in candidate chunk
        for (size_t p = 0; p < chunk.pickups.size(); ++p) {
            const StaticPickup& pickup = chunk.pickups[p];
            
            // Convert chunk-local position to world space
            glm::vec3 worldPos = pickup.position + glm::vec3(chunk.x, 0.0f, chunk.y);
            
            if (glm::distance(worldPos, position) > maxDistance) 
                continue;
            
            // Construct bounding box around pickup based on scale
            glm::vec3 boxScale = glm::max(pickup.scale, glm::vec3(0.8f));
            glm::vec3 boxMin   = worldPos - (boxScale * 0.5f);
            glm::vec3 boxMax   = worldPos + (boxScale * 0.5f);
            
            float hitDistance = 0.0f;
            if (!RayIntersectsAABB(position, rayDir, boxMin, boxMax, hitDistance)) 
                continue;
            
            if (hitDistance > maxDistance) 
                continue;
            
            // Retain the closest hit object along the ray
            if (hitDistance < closestHitDist) {
                closestHitDist  = hitDistance;
                bestChunkIndex  = static_cast<int>(i);
                bestPickupIndex = static_cast<int>(p);
            }
        }
    }
    
    // Assign queried data without altering or deleting the pickup
    if (bestChunkIndex != -1 && bestPickupIndex != -1) {
        Chunk& targetChunk   = *chunks[bestChunkIndex];
        StaticPickup& pickup = targetChunk.pickups[bestPickupIndex];
        
        queriedItem = pickup.classification;
        return true;
    }
    
    return false;
}

bool ChunkManager::PlacePickup(glm::vec3 position, glm::vec3 direction, float maxDistance, const std::string& itemClassification) {
    if (itemClassification.empty())
        return false;
    
    glm::vec3 rayDir = glm::normalize(direction);
    Hit groundHit;
    
    // Raycast to find placement point on terrain
    if (!Physics.Raycast(position, rayDir, maxDistance, groundHit, LayerMask::Ground)) {
        return false;
    }
    
    glm::vec3 worldHitPos = groundHit.point;
    
    // 2. Locate target chunk
    float halfChunk = chunkSize * 0.5f;
    Chunk* targetChunk = nullptr;
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        Chunk* c = chunks[i];
        if (std::abs(worldHitPos.x - c->x) <= halfChunk &&
            std::abs(worldHitPos.z - c->y) <= halfChunk) {
            targetChunk = c;
            break;
        }
    }
    
    if (!targetChunk)
        return false;
    
    // Create renderer components
    MeshRenderer* pickupRenderer = Engine.Create<MeshRenderer>();
    pickupRenderer->mesh         = Engine.Create<Mesh>();
    pickupRenderer->material     = Engine.Create<Material>();
    
    pickupRenderer->material->shader  = Resources.shaders.color;
    pickupRenderer->material->ambient = Colors.white;
    pickupRenderer->material->diffuse = Colors.white;
    
    build.BuildItemMesh(pickupRenderer->mesh, itemClassification);
    
    glm::vec3 worldSpawnPos = worldHitPos + glm::vec3(0.0f, 1.0f, 0.0f);
    pickupRenderer->transform.position = worldSpawnPos;
    pickupRenderer->transform.UpdateMatrix();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(pickupRenderer, RENDER_QUEUE_GEOMETRY);
    
    // Convert world spawn position to CHUNK-LOCAL space
    glm::vec3 localPos = worldSpawnPos - glm::vec3(targetChunk->x, 0.0f, targetChunk->y);
    
    // Record pickup in target chunk
    StaticPickup pickup;
    pickup.position       = localPos; // Stored in Chunk-Local Space
    pickup.rotation       = glm::vec3(0.0f);
    pickup.scale          = glm::vec3(0.5f);
    pickup.classification = itemClassification;
    pickup.renderer       = pickupRenderer;
    
    pickupRenderer->isActive = true;
    
    targetChunk->pickups.push_back(pickup);
    return true;
}

bool ChunkManager::PlacePickupAt(const std::string& itemClassification, const glm::vec3& position) {
    if (itemClassification.empty())
        return false;
    
    // Locate the active chunk that contains this world position
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
    
    if (!targetChunk)
        return false;
    
    // Create and configure renderer components
    MeshRenderer* pickupRenderer = Engine.Create<MeshRenderer>();
    pickupRenderer->mesh         = Engine.Create<Mesh>();
    pickupRenderer->material     = Engine.Create<Material>();
    
    pickupRenderer->material->shader  = Resources.shaders.color;
    pickupRenderer->material->ambient = Colors.white;
    pickupRenderer->material->diffuse = Colors.white;
    
    // Construct procedural item mesh from classification data
    build.BuildItemMesh(pickupRenderer->mesh, itemClassification);
    
    // Position renderer in world space
    pickupRenderer->transform.position = position;
    pickupRenderer->transform.UpdateMatrix();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(pickupRenderer, RENDER_QUEUE_GEOMETRY);
    
    // Convert world position to chunk-local coordinates
    glm::vec3 localPos = position - glm::vec3(targetChunk->x, 0.0f, targetChunk->y);
    
    // Construct and store StaticPickup entry in the target chunk
    StaticPickup pickup;
    pickup.position       = localPos;
    pickup.rotation       = glm::vec3(0.0f);
    pickup.scale          = glm::vec3(0.5f);
    pickup.classification = itemClassification;
    pickup.renderer       = pickupRenderer;
    
    pickupRenderer->isActive = true;
    
    targetChunk->pickups.push_back(pickup);
    return true;
}

bool ChunkManager::RemovePickup(glm::vec3 position, glm::vec3 direction, float maxDistance, std::string& collectedItem) {
    glm::vec3 rayDir = glm::normalize(direction);
    float closestHitDist = maxDistance + 1.0f;
    int bestChunkIndex  = -1;
    int bestPickupIndex = -1;
    
    // Iterate over all active chunks in range
    for (unsigned int i = 0; i < chunks.Size(); i++) {
        Chunk& chunk = *chunks[i];
        
        // Fast 2D chunk distance pre-filter
        glm::vec3 chunkPos(chunk.x, 0.0f, chunk.y);
        glm::vec3 pos2D(position.x, 0.0f, position.z);
        if (glm::distance(chunkPos, pos2D) > (chunkSize * 0.707f + maxDistance)) 
            continue;
        
        // Check each pickup in candidate chunk
        for (size_t p = 0; p < chunk.pickups.size(); ++p) {
            const StaticPickup& pickup = chunk.pickups[p];
            
            // Convert chunk-local position to world space
            glm::vec3 worldPos = pickup.position + glm::vec3(chunk.x, 0.0f, chunk.y);
            
            if (glm::distance(worldPos, position) > maxDistance) 
                continue;
            
            // Construct bounding box around pickup based on scale
            glm::vec3 boxScale = glm::max(pickup.scale, glm::vec3(0.8f));
            glm::vec3 boxMin   = worldPos - (boxScale * 0.5f);
            glm::vec3 boxMax   = worldPos + (boxScale * 0.5f);
            
            float hitDistance = 0.0f;
            if (!RayIntersectsAABB(position, rayDir, boxMin, boxMax, hitDistance)) 
                continue;
            
            if (hitDistance > maxDistance) 
                continue;
            
            // Retain the closest hit object along the ray
            if (hitDistance < closestHitDist) {
                closestHitDist  = hitDistance;
                bestChunkIndex  = static_cast<int>(i);
                bestPickupIndex = static_cast<int>(p);
            }
        }
    }
    
    // Process closest targeted pickup
    if (bestChunkIndex != -1 && bestPickupIndex != -1) {
        Chunk& targetChunk   = *chunks[bestChunkIndex];
        StaticPickup& pickup = targetChunk.pickups[bestPickupIndex];
        
        // Output item classification string back to caller
        collectedItem = pickup.classification;
        
        // Remove MeshRenderer from scene and destroy component
        if (pickup.renderer != nullptr) {
            Engine.sceneMain->RemoveMeshRendererFromSceneRoot(pickup.renderer, RENDER_QUEUE_GEOMETRY);
            Engine.Destroy<MeshRenderer>(pickup.renderer);
            pickup.renderer = nullptr;
        }
        
        // Erase pickup entry from chunk
        targetChunk.pickups.erase(targetChunk.pickups.begin() + bestPickupIndex);
        return true;
    }
    
    return false;
}

bool ChunkManager::RemovePickupAt(const glm::vec3& position, float tolerance, std::string* collectedItem) {
    // Locate the active chunk containing the world position
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
    
    if (!targetChunk)
        return false;
    
    // Translate target position to chunk-local space
    glm::vec3 localTarget = position - glm::vec3(targetChunk->x, 0.0f, targetChunk->y);
    
    // Search pickups in the target chunk within tolerance distance
    for (size_t p = 0; p < targetChunk->pickups.size(); ++p) {
        StaticPickup& pickup = targetChunk->pickups[p];
        
        if (glm::distance(pickup.position, localTarget) <= tolerance) {
            // Optionally output the collected item classification string
            if (collectedItem != nullptr) {
                *collectedItem = pickup.classification;
            }
            
            // Clean up renderer component and scene graph registration
            if (pickup.renderer != nullptr) {
                Engine.sceneMain->RemoveMeshRendererFromSceneRoot(pickup.renderer, RENDER_QUEUE_GEOMETRY);
                Engine.Destroy<MeshRenderer>(pickup.renderer);
                pickup.renderer = nullptr;
            }
            
            // Remove pickup entry from chunk list
            targetChunk->pickups.erase(targetChunk->pickups.begin() + p);
            return true;
        }
    }
    
    return false;
}

std::vector<std::pair<std::string, glm::vec3>> ChunkManager::QueryPickupNames(const glm::vec3& position, float range) {
    std::lock_guard<std::mutex> lock(mux);
    std::vector<std::pair<std::string, glm::vec3>> results;
    
    // Center position projected onto XZ plane for chunk bounding pre-filter
    glm::vec3 center2D(position.x, 0.0f, position.z);
    
    // Maximum distance from center point to chunk origin before safely skipping
    const float maxChunkDist = (chunkSize * 0.7071f) + range;
    
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        Chunk& chunk = *chunks[i];
        glm::vec3 chunkPos(chunk.x, 0.0f, chunk.y);
        
        // Skip chunks outside maximum potential search boundary
        if (glm::distance(chunkPos, center2D) > maxChunkDist) 
            continue;
        
        // Iterate through item pickups in candidate chunk
        for (const StaticPickup& pickup : chunk.pickups) {
            glm::vec3 worldPos = pickup.position + glm::vec3(chunk.x, 0.0f, chunk.y);
            
            if (glm::distance(position, worldPos) <= range) {
                results.emplace_back(pickup.classification, worldPos);
            }
        }
    }
    
    return results;
}

std::vector<NearbyPickupInfo> ChunkManager::QueryPickupNearest(glm::vec3 position, float maxDistance, size_t count) {
    std::vector<NearbyPickupInfo> candidates;
    
    if (count == 0 || maxDistance <= 0.0f)
        return candidates;
    
    glm::vec3 pos2D(position.x, 0.0f, position.z);
    const float maxChunkDist = (chunkSize * 0.7071f) + maxDistance;
    
    // Iterate over active chunks and filter out those out of range
    for (unsigned int i = 0; i < chunks.Size(); i++) {
        Chunk& chunk = *chunks[i];
        
        glm::vec3 chunkPos(chunk.x, 0.0f, chunk.y);
        if (glm::distance(chunkPos, pos2D) > maxChunkDist)
            continue;
        
        // Check each pickup in candidate chunk
        for (const StaticPickup& pickup : chunk.pickups) {
            // Convert chunk-local position to world space
            glm::vec3 worldPos = pickup.position + glm::vec3(chunk.x, 0.0f, chunk.y);
            
            float dist = glm::distance(position, worldPos);
            if (dist <= maxDistance) {
                NearbyPickupInfo info;
                info.classification = pickup.classification;
                info.worldPosition  = worldPos;
                info.distance       = dist;
                candidates.push_back(info);
            }
        }
    }
    
    // Sort candidates by distance (closest first)
    std::sort(candidates.begin(), candidates.end(), [](const NearbyPickupInfo& a, const NearbyPickupInfo& b) {
        return a.distance < b.distance;
    });
    
    // Limit results to N items
    if (candidates.size() > count) {
        candidates.resize(count);
    }
    
    return candidates;
}
