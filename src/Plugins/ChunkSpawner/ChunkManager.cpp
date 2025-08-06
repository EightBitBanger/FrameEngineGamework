#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <GameEngineFramework/Timer/Timer.h>

void chunkGenerationThread(void);

ChunkManager::ChunkManager() : 
    isInitiated(false),
    isChunkGenerationActive(false),
    
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
    // Fire up the generation thread
    if (isChunkGenerationActive) 
        return;
    isChunkGenerationActive = true;
    threadTimer.SetRefreshRate(30);
    generationThread = new std::thread( chunkGenerationThread );
    
    Log.Write( " >> Starting thread chunk generator" );
    
    
    // Initiate AI
    AI.SetWaterLevel( world.waterLevel );
    
    // Source meshes for world construction
    
    glm::vec3 normalUp(0, 1, 0);
    
    Engine.meshes.wallHorizontal->SetNormals(normalUp);
    Engine.meshes.wallVertical->SetNormals(normalUp);
    Engine.meshes.grassHorz->SetNormals(normalUp);
    Engine.meshes.grassVert->SetNormals(normalUp);
    Engine.meshes.stemHorz->SetNormals(normalUp);
    Engine.meshes.stemVert->SetNormals(normalUp);
    
    Engine.meshes.wallHorizontal->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    Engine.meshes.wallVertical->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    Engine.meshes.grassHorz->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    Engine.meshes.grassVert->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    Engine.meshes.stemHorz->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    Engine.meshes.stemVert->ChangeSubMeshScale(0, 0.5f, 0.5f, 0.5f);
    
    Engine.meshes.log->ChangeSubMeshScale(2, 0.5f, 1.0f, 0.5f);
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
    
    // Underwater blue fog
    
    fogWater = Renderer.CreateFog();
    Engine.sceneMain->AddFogLayerToScene(fogWater);
    
    // Load definitions
    std::string definitionDirectory = "definitions\\";
    if (!fs.DirectoryExists(definitionDirectory)) 
        return;
    
    std::vector<std::string> dirList = fs.DirectoryGetList(definitionDirectory);
    for (unsigned int i=0; i < dirList.size(); i++) {
        std::string filename = dirList[i];
        
        std::ifstream file(definitionDirectory + filename);
        if (!file.is_open()) 
            continue;
        
        std::vector<std::string> nameParts = String.Explode(filename, '.');
        if (nameParts.size() != 3) 
            continue;
        
        std::string type = nameParts[0];
        std::string name = nameParts[1];
        
        std::string line;
        std::unordered_map<std::string, std::string> keys;
        while (std::getline(file, line)) {
            line = String.RemoveWhiteSpace(line);
            std::vector<std::string> split = String.Explode(line, '=');
            if (split.size() != 2) 
                continue;
            keys[split[0]] = split[1];
        }
        
        if (type == "grass") {
            // Extract the definition data from the file
            DefinitionTypeGrass definition;
            definition.name = name;
            
            if (keys["width"] != "") definition.width = String.ToFloat( keys["width"] );
            if (keys["height"] != "") definition.height = String.ToFloat( keys["height"] );
            
            if (keys["colorMin"] != "") definition.colorMin = GetColorByName(keys["colorMin"]);
            if (keys["colorMax"] != "") definition.colorMax = GetColorByName(keys["colorMax"]);
            
            world.definitionGrass[name] = definition;
            file.close();
            continue;
        }
        
        if (type == "tree") {
            // Extract the definition data from the file
            DefinitionTypeTree definition;
            definition.name = name;
            
            if (keys["heightMin"] != "") definition.heightMin = String.ToFloat( keys["heightMin"] );
            if (keys["heightMax"] != "") definition.heightMax = String.ToFloat( keys["heightMax"] );
            
            if (keys["leafCountMin"] != "") definition.leafCountMin = String.ToUint( keys["leafCountMin"] );
            if (keys["leafCountMax"] != "") definition.leafCountMax = String.ToUint( keys["leafCountMax"] );
            
            if (keys["leafSpreadArea"] != "") definition.leafSpreadArea = String.ToFloat( keys["leafSpreadArea"] );
            if (keys["leafSpreadHeight"] != "") definition.leafSpreadHeight = String.ToFloat( keys["leafSpreadHeight"] );
            if (keys["leafWidth"] != "") definition.leafWidth = String.ToFloat( keys["leafWidth"] );
            if (keys["leafHeight"] != "") definition.leafHeight = String.ToFloat( keys["leafHeight"] );
            if (keys["trunkSize"] != "") definition.trunkSize = String.ToFloat( keys["trunkSize"] );
            
            if (keys["trunkColorMin"] != "") definition.trunkColorMin = GetColorByName(keys["trunkColorMin"]);
            if (keys["trunkColorMax"] != "") definition.trunkColorMax = GetColorByName(keys["trunkColorMax"]);
            if (keys["leafColorMin"] != "") definition.leafColorMin = GetColorByName(keys["leafColorMin"]);
            if (keys["leafColorMax"] != "") definition.leafColorMax = GetColorByName(keys["leafColorMax"]);
            
            world.definitionTree[name] = definition;
            file.close();
            continue;
        }
        
        file.close();
    }
    
    return;
}

Actor* ChunkManager::SummonActor(glm::vec3 position) {
    Actor* actor = AI.CreateActor();
    actor->navigation.SetPosition(position);
    actor->navigation.SetTargetPoint(position);
    
    std::string homePosition = Float.ToString(position.x) + "," +
                               Float.ToString(position.y) + "," +
                               Float.ToString(position.z);
    
    actor->memories.Add("home", homePosition);
    
    return actor;
}

void ChunkManager::KillActor(Actor* actor) {
    AI.DestroyActor( actor );
}


Color ChunkManager::GetColorByName(std::string name) {
    float multiplier = 1.0f;
    if (name.find("*") != std::string::npos) {
        std::vector<std::string> muls = String.Explode(name, '*');
        for (unsigned int i=0; i < muls.size(); i++) {
            multiplier *= String.ToFloat(muls[1]);
        }
        name = muls[0];
    }
    Color result = Colors.white;
    if (name == "red")     result = Colors.red;
    if (name == "green")   result = Colors.green;
    if (name == "blue")    result = Colors.blue;
    if (name == "dkred")   result = Colors.dkred;
    if (name == "dkgreen") result = Colors.dkgreen;
    if (name == "dkblue")  result = Colors.dkblue;
    if (name == "yellow")  result = Colors.yellow;
    if (name == "orange")  result = Colors.orange;
    if (name == "purple")  result = Colors.purple;
    if (name == "gray")    result = Colors.gray;
    if (name == "ltgray")  result = Colors.ltgray;
    if (name == "dkgray")  result = Colors.dkgray;
    if (name == "white")   result = Colors.white;
    if (name == "black")   result = Colors.black;
    if (name == "brown")   result = Colors.brown;
    return result * multiplier;
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
    
    for (unsigned int c=0; c < chunks.Size(); c++) 
        DestroyChunk( chunks[c] );
    
    unsigned int numberOfActors = AI.GetNumberOfActors();
    for (unsigned int a=0; a < numberOfActors; a++) 
        KillActor( AI.GetActor(a) );
    
    mChunkCounterX = 0;
    mChunkCounterZ = 0;
    
    //chunks.clear();
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
    unsigned int numberOfChunks = chunks.Size();
    for (unsigned int i=0; i < numberOfChunks; i++) {
        Chunk* chunk = chunks[i];
        if ((chunk->x != x) | (chunk->y != y)) 
            continue;
        return chunk;
    }
    return nullptr;
}


extern ChunkManager GameWorld;

void chunkGenerationThread(void) {
    
    while (GameWorld.isChunkGenerationActive) {
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(10) );
        
        if (!GameWorld.threadTimer.Update()) 
            continue;
        std::lock_guard<std::mutex> lock(GameWorld.mux);
        float chunkSZ = GameWorld.chunkSize;
        
        unsigned int numberOfChunks = GameWorld.generating.size();
        if (numberOfChunks == 0) 
            continue;
        
        Chunk* chunk = GameWorld.generating[0];
        GameWorld.generating.erase( GameWorld.generating.begin() );
        
        GameWorld.GenerateChunkBlendMasks(chunk);
        
        chunk->isGenerated = true;
    }
    
    Log.Write( " >> Shutting down chunk generation" );
    
    return;
}

