#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::SetWorldColorAdditive(const glm::vec3& worldPosition, const Color& color) {
    float halfChunk = (float)chunkSize * 0.5f;
    Chunk* targetChunk = nullptr;
    
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        Chunk* c = chunks[i];
        if (std::abs(worldPosition.x - c->x) <= halfChunk &&
            std::abs(worldPosition.z - c->y) <= halfChunk) {
            targetChunk = c;
            break;
        }
    }
    
    if (targetChunk == nullptr || targetChunk->colorAdditive == nullptr)
        return false;
    
    unsigned int chunkSZ = chunkSize + 1;
    float localX = (worldPosition.x - targetChunk->x) + halfChunk;
    float localZ = (worldPosition.z - targetChunk->y) + halfChunk;
    
    int gridX = glm::clamp((int)std::round(localX), 0, (int)chunkSize);
    int gridZ = glm::clamp((int)std::round(localZ), 0, (int)chunkSize);
    
    targetChunk->colorAdditive[gridZ * chunkSZ + gridX] = glm::vec3(color.r, color.g, color.b);
    RebuildChunkMeshColors(targetChunk);
    return true;
}

bool ChunkManager::AddWorldColorAdditiveRadius(const glm::vec3& worldPosition, float radius, const glm::vec3& color, float intensity, bool additive) {
    unsigned int chunkSZ = chunkSize + 1;
    float halfChunk = (float)chunkSize * 0.5f;
    bool affectedAny = false;
    
    for (unsigned int i = 0; i < chunks.Size(); ++i) {
        Chunk* chunk = chunks[i];
        if (chunk->colorAdditive == nullptr) 
            continue;
        
        glm::vec3 chunkCenter(chunk->x, 0.0f, chunk->y);
        if (glm::distance(glm::vec3(worldPosition.x, 0.0f, worldPosition.z), chunkCenter) > (halfChunk * 1.414f + radius))
            continue;
        
        float localCenterX = (worldPosition.x - chunk->x) + halfChunk;
        float localCenterZ = (worldPosition.z - chunk->y) + halfChunk;
        
        generation.ApplyColorAdditiveBrush(chunk->colorAdditive, chunkSZ, chunkSZ, localCenterX, localCenterZ, radius, color, intensity, additive);
        RebuildChunkMeshColors(chunk);
        affectedAny = true;
    }
    
    return affectedAny;
}

void ChunkManager::DecayChunkColorAdditive(Chunk* chunk, float decayRate) {
    if (chunk == nullptr || chunk->colorAdditive == nullptr) 
        return;
    
    unsigned int chunkSZ = chunkSize + 1;
    generation.FadeColorAdditive(chunk->colorAdditive, chunkSZ, chunkSZ, decayRate);
    RebuildChunkMeshColors(chunk);
}

void ChunkManager::RebuildChunkMeshColors(Chunk* chunk) {
    if (chunk == nullptr || chunk->colorField == nullptr || chunk->colorAdditive == nullptr)
        return;
    
    MeshRenderer* chunkRenderer = chunk->gameObject->GetComponent<MeshRenderer>();
    if (chunkRenderer == nullptr || chunkRenderer->mesh == nullptr)
        return;
    
    Mesh* chunkMesh = chunkRenderer->mesh;
    unsigned int chunkSZ = chunkSize + 1;
    unsigned int fieldWidth = chunkSize;
    unsigned int fieldHeight = chunkSize;
    
    unsigned int subCount = chunkMesh->GetSubMeshCount();
    unsigned int quadIndex = 0;
    
    for (unsigned int x = 0; x < fieldWidth; x++) {
        for (unsigned int z = 0; z < fieldHeight; z++) {
            if (quadIndex >= subCount) break;
            
            unsigned int index = z * chunkSZ + x;
            glm::vec3 finalColor = glm::clamp(chunk->colorField[index] + chunk->colorAdditive[index], 0.0f, 1.0f);
            
            chunkMesh->ChangeSubMeshColor(quadIndex, Color(finalColor.x, finalColor.y, finalColor.z));
            quadIndex++;
        }
    }
    
    chunkMesh->Load();
}
