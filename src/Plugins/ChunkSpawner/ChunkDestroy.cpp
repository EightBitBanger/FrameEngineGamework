#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::DestroyChunk(Chunk* chunk) {
    MeshRenderer* chunkRenderer  = chunk->gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk->staticObject->GetComponent<MeshRenderer>();
    MeshRenderer* pickupRenderer = chunk->pickupObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer,  RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( pickupRenderer, RENDER_QUEUE_GEOMETRY );
    
    Engine.Destroy<GameObject>( chunk->gameObject );
    Engine.Destroy<GameObject>( chunk->staticObject );
    Engine.Destroy<GameObject>( chunk->pickupObject );
    
    if (chunk->rigidBody != nullptr) {
        Physics.DestroyRigidBody( chunk->rigidBody );
        chunk->rigidBody = nullptr;
    }
    if (chunk->meshCollider != nullptr) {
        Physics.DestroyHeightFieldMap( chunk->meshCollider );
        chunk->meshCollider = nullptr;
    }
    
    // Destroy particle emitters
    for (Emitter* emitter : chunk->emitters) {
        Particle.DestroyEmitter(emitter);
    }
    chunk->emitters.clear();
    
    // Clear pickups
    chunk->pickups.clear();
    
    if (chunk->heightField != nullptr) {
        free(chunk->heightField);
        chunk->heightField = nullptr;
    }
    if (chunk->colorField != nullptr) {
        free(chunk->colorField);
        chunk->colorField = nullptr;
    }
    if (chunk->colorAdditive != nullptr) {
        free(chunk->colorAdditive);
        chunk->colorAdditive = nullptr;
    }
    
    chunks.Destroy(chunk);
    return true;
}
