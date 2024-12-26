#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

ChunkManager::ChunkManager() : 
    isInitiated(false),
    
    renderDistance(10),
    staticDistance(10),
    
    chunkSize(50),
    
    worldSeed(100),
    
    numberOfActiveActors(0),
    
    mActorIndex(0),
    mChunkIndex(0),
    
    mChunkCounterX(0),
    mChunkCounterZ(0),
    
    mBreedingCoolDown(8000),
    mDeathCoolDown(4000)
{
    
    return;
}

void ChunkManager::Initiate(void) {
    
    AI.SetWaterLevel( world.waterLevel );
    
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
    
    waterMesh      = Engine.Create<Mesh>();
    waterMaterial  = Engine.Create<Material>();
    worldMaterial  = Engine.Create<Material>();
    staticMaterial = Engine.Create<Material>();
    
    waterMesh->isShared     = true;
    waterMaterial->isShared = true;
    worldMaterial->isShared = true;
    staticMaterial->isShared = true;
    
    worldMaterial->diffuse = Colors.gray;
    worldMaterial->ambient = Colors.MakeGrayScale(0.2f);
    worldMaterial->shader = Engine.shaders.color;
    
    staticMaterial->DisableCulling();
    staticMaterial->diffuse = Colors.gray;
    staticMaterial->ambient = Colors.MakeGrayScale(0.2f);
    staticMaterial->shader = Engine.shaders.color;
    
    waterMaterial->DisableCulling();
    waterMaterial->EnableBlending();
    waterMaterial->diffuse = Colors.blue * Colors.MakeGrayScale(0.4f);
    waterMaterial->shader = Engine.shaders.water;
    
    return;
}

void ChunkManager::InitiateWorld(void) {
    
    std::string worldName   = "worlds/" + world.name;
    std::string worldChunks = "worlds/" + world.name + "/chunks";
    std::string worldStatic = "worlds/" + world.name + "/static";
    
    // Check world directory structure exists
    if (!fs.DirectoryExists(worldName)) {
        
        fs.DirectoryCreate(worldName);
        
        if (!fs.DirectoryExists(worldChunks)) 
            fs.DirectoryCreate(worldChunks);
        
        if (!fs.DirectoryExists(worldStatic)) 
            fs.DirectoryCreate(worldStatic);
        
    }
    
    return;
}

void ChunkManager::ClearWorld(void) {
    
    world.doGenerateChunks = false;
    
    isInitiated = false;
    
    for (unsigned int c=0; c < chunks.size(); c++) 
        DestroyChunk( chunks[c] );
    
    for (unsigned int a=0; a < actors.size(); a++) 
        KillActor(actors[a]);
    
    mChunkCounterX = 0;
    mChunkCounterZ = 0;
    
    chunks.clear();
    
    return;
}

bool ChunkManager::DestroyWorld(std::string worldname) {
    
    if (worldname == "") 
        return false;
    
    std::string worldPath = "worlds/" + worldname;
    
    if (!fs.DirectoryExists(worldPath)) 
        return false;
    
    fs.DirectoryDelete( worldPath + "/chunks" );
    fs.DirectoryDelete( worldPath + "/static" );
    
    fs.DirectoryDelete( worldPath );
    
    return true;
}


GameObject* ChunkManager::SpawnActor(float x, float y, float z) {
    
    GameObject* actorObject = nullptr;
    
    unsigned int numberOfActors = actors.size();
    
    for (unsigned int a=0; a < numberOfActors; a++) {
        
        if (actors[a]->isActive) 
            continue;
        
        actorObject = actors[a];
        actorObject->SetPosition(x, y, z);
        
        actorObject->Activate();
        
        break;
    }
    
    if (actorObject == nullptr) {
        
        actorObject = Engine.CreateAIActor( glm::vec3(x, y, z) );
        
        actorObject->renderDistance = renderDistance * chunkSize * 2.4f;
        
        actors.push_back( actorObject );
        
    }
    
    Actor* actorPtr = actorObject->GetComponent<Actor>();
    actorPtr->SetTargetPoint(glm::vec3(x, y, z));
    
    actorPtr->SetUserBitmask(0);
    
    if (actorPtr->GetHeightPreferenceMin() == 0.0f) 
        actorPtr->SetHeightPreferenceMin(world.waterLevel);
    
    numberOfActiveActors++;
    
    return actorObject;
}

bool ChunkManager::KillActor(GameObject* actorObject) {
    
    Actor* actorPtr = actorObject->GetComponent<Actor>();
    
    actorObject->Deactivate();
    
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
    
    numberOfActiveActors--;
    
    return true;
}

Chunk* ChunkManager::FindChunk(int x, int y) {
    
    unsigned int numberOfChunks = chunks.size();
    
    for (unsigned int i=0; i < numberOfChunks; i++) {
        
        Chunk* chunk = &chunks[i];
        
        if ((chunk->x != x) | (chunk->y != y)) 
            continue;
        
        return chunk;
    }
    
    return nullptr;
}


