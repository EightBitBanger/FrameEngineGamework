#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

ChunkManager::ChunkManager() : 
    
    renderDistance(16),
    staticDistance(10),
    
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


void ChunkManager::ClearWorld(void) {
    
    world.doGenerateChunks = false;
    
    SaveWorld();
    
    for (unsigned int i=0; i < chunks.size(); i++) 
        DestroyChunk( chunks[i] );
    
    for (unsigned int a=0; a < actors.size(); a++) 
        KillActor(actors[a]);
    
    ChunkCounterX = 0;
    ChunkCounterZ = 0;
    
    return;
}

bool ChunkManager::DestroyWorld(std::string worldname) {
    
    if (worldname == "") 
        return false;
    
    std::string worldPath = "worlds/" + worldname;
    
    if (!fs.DirectoryExists(worldPath)) 
        return false;
    
    fs.DirectoryDelete( worldPath + "/chunks" );
    
    fs.DirectoryDelete( worldPath );
    
    return true;
}


GameObject* ChunkManager::SpawnActor(float x, float y, float z) {
    
    GameObject* actorObject = nullptr;
    
    for (unsigned int a=0; a < actors.size(); a++) {
        
        if (actors[a]->isActive) 
            continue;
        
        actorObject = actors[a];
        actorObject->SetPosition(x, y, z);
        
        actorObject->Activate();
        
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        actorPtr->SetTargetPoint(glm::vec3(x, y, z));
        
        break;
    }
    
    if (actorObject == nullptr) {
        
        actorObject = Engine.CreateAIActor( glm::vec3(x, y, z) );
        
        actors.push_back( actorObject );
        
        Actor* actorPtr = actorObject->GetComponent<Actor>();
        actorPtr->SetTargetPoint(glm::vec3(x, y, z));
        
    }
    
    return actorObject;
}

bool ChunkManager::KillActor(GameObject* actorObject) {
    
    Actor* actorPtr = actorObject->GetComponent<Actor>();
    
    actorObject->Deactivate();
    
    actorPtr->ClearGenome();
    
    if (actorPtr == nullptr) 
        return false;
    
    actorPtr->SetName("");
    actorPtr->SetChanceToChangeDirection(0);
    actorPtr->SetChanceToFocusOnActor(0);
    actorPtr->SetChanceToStopWalking(0);
    actorPtr->SetChanceToWalk(0);
    actorPtr->SetTargetPoint(glm::vec3(0, 0, 0));
    
    actorPtr->SetSpeed(0);
    actorPtr->SetSpeedMultiplier(0);
    
    actorPtr->SetYouthScale(0);
    actorPtr->SetAdultScale(0);
    
    actorPtr->ClearGenome();
    actorPtr->ClearMemories();
    actorPtr->ClearWeightedLayers();
    
    return true;
}
    
