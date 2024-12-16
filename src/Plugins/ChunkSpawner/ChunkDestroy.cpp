#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

bool ChunkManager::DestroyChunk(Chunk& chunk) {
    
    MeshRenderer* chunkRenderer = chunk.gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunk.staticObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
    
    Engine.Destroy<GameObject>( chunk.gameObject );
    Engine.Destroy<GameObject>( chunk.staticObject );
    
    // Destroy water if it was generated
    if (chunk.waterObject != nullptr) {
        
        MeshRenderer* waterRenderer = chunk.waterObject->GetComponent<MeshRenderer>();
        
        Engine.sceneMain->RemoveMeshRendererFromSceneRoot( waterRenderer, RENDER_QUEUE_POSTGEOMETRY );
        
        Engine.Destroy<GameObject>( chunk.waterObject );
        
    }
    
    Physics.DestroyRigidBody( chunk.rigidBody );
    Physics.DestroyHeightFieldMap(chunk.meshCollider);
    
    return true;
}

