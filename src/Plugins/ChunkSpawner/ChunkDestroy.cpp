#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::DestroyChunk(Chunk* chunk) {
    MeshRenderer* chunkRenderer = chunk->gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk->staticObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
    
    Engine.Destroy<GameObject>( chunk->gameObject );
    Engine.Destroy<GameObject>( chunk->staticObject );
    
    if (chunk->rigidBody != nullptr) {
        Physics.DestroyRigidBody( chunk->rigidBody );
        chunk->rigidBody = nullptr;
    }
    if (chunk->meshCollider != nullptr) {
        Physics.DestroyHeightFieldMap( chunk->meshCollider );
        chunk->meshCollider = nullptr;
    }
    
    chunks.Destroy(chunk);
    
    return true;
}

