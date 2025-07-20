#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>

ChunkManager::ChunkManager() : 
    isInitiated(false),
    
    renderDistance(16),
    staticDistance(renderDistance * 0.7f),
    actorDistance(renderDistance * 0.5f),
    
    chunkSize(50),
    
    worldSeed(100),
    
    waterMaterial(nullptr),
    
    worldMaterial(nullptr),
    staticMaterial(nullptr),
    
    mActorIndex(0),
    mChunkIndex(0),
    
    mChunkCounterX(0),
    mChunkCounterZ(0),
    
    mBreedingCoolDown(10),
    mDeathCoolDown(10),
    
    waterMesh(nullptr),
    
    fogWater(nullptr)
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
    
    Engine.meshes.grassHorz->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    Engine.meshes.grassVert->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    
    Engine.meshes.stemHorz->SetNormals(normalUp);
    Engine.meshes.stemVert->SetNormals(normalUp);
    
    Engine.meshes.stemHorz->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    Engine.meshes.stemVert->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    
    Engine.meshes.log->ChangeSubMeshScale(2, 0.5f, 1.0f, 0.5f);
    Engine.meshes.log->GetSubMesh(2, subMeshTree);
    
    Engine.meshes.wallHorizontal->GetSubMesh(0, subMeshWallHorz);
    Engine.meshes.wallVertical  ->GetSubMesh(0, subMeshWallVert);
    
    Engine.meshes.wallHorizontal->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    Engine.meshes.wallVertical->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    
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
    
    // Underwater blue fog
    
    fogWater = Renderer.CreateFog();
    
    Engine.sceneMain->AddFogLayerToScene(fogWater);
    
    return;
}

Actor* ChunkManager::SummonActor(glm::vec3 position) {
    Actor* actor = AI.CreateActor();
    actor->navigation.SetPosition(position);
    actor->navigation.SetTargetPoint(position);
    return actor;
}

void ChunkManager::KillActor(Actor* actor) {
    AI.DestroyActor( actor );
}


bool ChunkManager::WorldDirectoryInitiate(void) {
    std::string worldName   = "worlds\\" + world.name;
    std::string worldChunks = "worlds\\" + world.name + "\\chunks";
    std::string worldStatic = "worlds\\" + world.name + "\\static";
    
    // Check world directory structure exists
    if (!fs.DirectoryExists(worldName)) {
        fs.DirectoryCreate(worldName);
        
        if (!fs.DirectoryExists(worldChunks)) 
            fs.DirectoryCreate(worldChunks);
        
        if (!fs.DirectoryExists(worldStatic)) 
            fs.DirectoryCreate(worldStatic);
        
        return false;
    }
    return true;
}

void ChunkManager::ClearWorld(void) {
    world.doGenerateChunks = false;
    isInitiated = false;
    
    for (unsigned int c=0; c < chunks.size(); c++) 
        DestroyChunk( chunks[c] );
    
    unsigned int numberOfActors = AI.GetNumberOfActors();
    for (unsigned int a=0; a < numberOfActors; a++) 
        KillActor( AI.GetActor(a) );
    
    mChunkCounterX = 0;
    mChunkCounterZ = 0;
    
    chunks.clear();
    mWorldRules.clear();
    
    return;
}

bool ChunkManager::DestroyWorld(std::string worldname) {
    if (worldname == "") 
        return false;
    
    std::string worldPath = "worlds\\" + worldname;
    if (!fs.DirectoryExists(worldPath)) 
        return false;
    
    fs.DirectoryDelete( worldPath + "\\chunks" );
    fs.DirectoryDelete( worldPath + "\\static" );
    fs.DirectoryDelete( worldPath );
    return true;
}

void ChunkManager::AddWorldRule(std::string key, std::string value) {
    unsigned int numberOfRules = mWorldRules.size();
    for (unsigned int i=0; i < numberOfRules; i++) {
        if (mWorldRules[i].first != key) 
            continue;
        mWorldRules[i].second = value;
        ApplyWorldRule(key, value);
        return;
    }
    std::pair<std::string, std::string> keyPair(key, value);
    
    mWorldRules.push_back( keyPair );
    ApplyWorldRule(key, value);
    return;
}

bool ChunkManager::RemoveWorldRule(std::string key) {
    unsigned int numberOfRules = mWorldRules.size();
    for (unsigned int i=0; i < numberOfRules; i++) {
        if (mWorldRules[i].first != key) 
            continue;
        mWorldRules.erase(mWorldRules.begin() + i);
        return true;
    }
    return false;
}

std::string ChunkManager::GetWorldRule(std::string key) {
    unsigned int numberOfRules = mWorldRules.size();
    for (unsigned int i=0; i < numberOfRules; i++) {
        if (mWorldRules[i].first != key) 
            continue;
        return mWorldRules[i].second;
    }
    return "";
}

bool ChunkManager::SetWorldRule(std::string key, std::string value) {
    unsigned int numberOfRules = mWorldRules.size();
    for (unsigned int i=0; i < numberOfRules; i++) {
        if (mWorldRules[i].first != key) 
            continue;
        mWorldRules[i].second = value;
        ApplyWorldRule(key, value);
        return true;
    }
    return false;
}

bool ChunkManager::ApplyWorldRule(std::string key, std::string value) {
    if (key == "doAutoBreeding") {
        if (value == "true") {world.doAutoBreeding = true;} else {world.doAutoBreeding = false;}
        return true;
    }
    return false;
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


