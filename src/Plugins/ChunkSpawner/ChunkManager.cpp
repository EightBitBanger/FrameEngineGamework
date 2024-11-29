#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>


ChunkManager::ChunkManager() : 
    
    renderDistance(8),
    
    chunkSize(32),
    
    worldSeed(100),
    
    ChunkCounterX(0),
    ChunkCounterZ(0)
{
    
    return;
}

void ChunkManager::Initiate(void) {
    
    // Source meshes for world construction
    
    glm::vec3 normalUp(0, 1, 0);
    
    Engine.meshes.wallHorizontal->SetNormals(normalUp);
    Engine.meshes.wallVertical->SetNormals(normalUp);
    
    Engine.meshes.grassHorz->SetNormals(normalUp);
    Engine.meshes.grassVert->SetNormals(normalUp);
    
    Engine.meshes.stemHorz->SetNormals(normalUp);
    Engine.meshes.stemVert->SetNormals(normalUp);
    
    Engine.meshes.log->GetSubMesh(2, subMeshTree);
    
    Engine.meshes.wallHorizontal->GetSubMesh(0, subMeshWallHorz);
    Engine.meshes.wallVertical  ->GetSubMesh(0, subMeshWallVert);
    
    Engine.meshes.grassHorz->GetSubMesh(0, subMeshGrassHorz);
    Engine.meshes.grassVert->GetSubMesh(0, subMeshGrassVert);
    
    Engine.meshes.stemHorz->GetSubMesh(0, subMeshStemHorz);
    Engine.meshes.stemVert->GetSubMesh(0, subMeshStemVert);
    
    watermesh     = Engine.Create<Mesh>();
    watermaterial = Engine.Create<Material>();
    
    watermesh->isShared     = true;
    watermaterial->isShared = true;
    
    return;
}


Chunk ChunkManager::CreateChunk(float x, float y) {
    Chunk newChunk;
    
    newChunk.x = x;
    newChunk.y = y;
    
    chunkList.push_back( newChunk );
    
    return newChunk;
}

bool ChunkManager::DestroyChunk(Chunk& chunkPtr) {
    
    MeshRenderer* chunkRenderer = chunkPtr.gameObject->GetComponent<MeshRenderer>();
    MeshRenderer* staticRenderer = chunkPtr.staticObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
    Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
    
    for (unsigned int a=0; a < chunkPtr.actorList.size(); a++) {
        
        GameObject* actorObject = chunkPtr.actorList[a];
        
        Engine.Destroy<GameObject>( actorObject );
    }
    
    Engine.Destroy<GameObject>( chunkPtr.gameObject );
    Engine.Destroy<GameObject>( chunkPtr.staticObject );
    
    Physics.DestroyRigidBody( chunkPtr.rigidBody );
    Physics.DestroyHeightFieldMap(chunkPtr.meshCollider);
    
    chunkPtr.actorList.clear();
    
    return false;
}



void ChunkManager::SaveWorld(void) {
    
    
    
    
    
    return;
}

void ChunkManager::LoadWorld(void) {
    
    
    
    
    
    return;
}

void ChunkManager::ClearWorld(void) {
    
    unsigned int numberOfChunks = chunkList.size();
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        Chunk chunkPtr = chunkList[i];
        
        MeshRenderer* chunkRenderer = chunkPtr.gameObject->GetComponent<MeshRenderer>();
        MeshRenderer* staticRenderer = chunkPtr.staticObject->GetComponent<MeshRenderer>();
        
        Engine.sceneMain->RemoveMeshRendererFromSceneRoot( chunkRenderer, RENDER_QUEUE_GEOMETRY );
        Engine.sceneMain->RemoveMeshRendererFromSceneRoot( staticRenderer, RENDER_QUEUE_GEOMETRY );
        
        Engine.Destroy<GameObject>( chunkPtr.gameObject );
        Engine.Destroy<GameObject>( chunkPtr.staticObject );
        
        Physics.DestroyRigidBody( chunkPtr.rigidBody );
        Physics.DestroyHeightFieldMap(chunkPtr.meshCollider);
        
        if (chunkPtr.actorList.size() > 0) 
            for (unsigned int a=0; a < chunkPtr.actorList.size(); a++) 
                Engine.Destroy<GameObject>( chunkPtr.actorList[a] );
        
    }
    
    chunkList.clear();
    
    ChunkCounterX = 0;
    ChunkCounterZ = 0;
    
    return;
}






