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
    float scaler = 0.5f;
    Engine.meshes.wallHorizontal->ChangeSubMeshScale(0, scaler, scaler, scaler);
    Engine.meshes.wallVertical->ChangeSubMeshScale(0, scaler, scaler, scaler);
    Engine.meshes.plain->ChangeSubMeshScale(0, scaler, scaler, scaler);
    
    Engine.meshes.grass->ChangeSubMeshScale(0, scaler, scaler, scaler);
    Engine.meshes.leaf->ChangeSubMeshScale(0, scaler, scaler, scaler);
    Engine.meshes.log->ChangeSubMeshScale(0, scaler, 1.0f, scaler);
    
    // Get sub meshes
    Engine.meshes.wallHorizontal->GetSubMesh(0, subMeshWallHorz);
    Engine.meshes.wallVertical->GetSubMesh(0, subMeshWallVert);
    Engine.meshes.plain->GetSubMesh(0, subMeshPlain);
    
    Engine.meshes.grass->GetSubMesh(0, subMeshCross);
    Engine.meshes.leaf->GetSubMesh(0, subMeshLeaf);
    Engine.meshes.log->GetSubMesh(0, subMeshLog);
    
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
    
    //
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
    std::vector<std::string> parts = String.Explode(name, '*');
    Color result = Colors.white;
    
    for (unsigned int i = 0; i < parts.size(); i++) {
        std::string token = parts[i];
        
        // Try to interpret as a number
        /*
        if (String.IsFloat(token)) {
            result *= String.ToFloat(token);
            continue;
        }
        */
        
        Color colorMul = Colors.white;
        if (token == "red")     {colorMul = Colors.red; continue;}
        if (token == "green")   {colorMul = Colors.green; continue;}
        if (token == "blue")    {colorMul = Colors.blue; continue;}
        if (token == "dkred")   {colorMul = Colors.dkred; continue;}
        if (token == "dkgreen") {colorMul = Colors.dkgreen; continue;}
        if (token == "dkblue")  {colorMul = Colors.dkblue; continue;}
        if (token == "yellow")  {colorMul = Colors.yellow; continue;}
        if (token == "orange")  {colorMul = Colors.orange; continue;}
        if (token == "purple")  {colorMul = Colors.purple; continue;}
        if (token == "gray")    {colorMul = Colors.gray; continue;}
        if (token == "ltgray")  {colorMul = Colors.ltgray; continue;}
        if (token == "dkgray")  {colorMul = Colors.dkgray; continue;}
        if (token == "white")   {colorMul = Colors.white; continue;}
        if (token == "black")   {colorMul = Colors.black; continue;}
        if (token == "brown")   {colorMul = Colors.brown; continue;}
        
        result = Colors.Lerp(result, colorMul, 0.5f);
    }
    return result;
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


float Snap1D(float v, float grid, float origin = 0.0f) {
    return origin + std::round((v - origin) / grid) * grid;
}

glm::vec3 SnapAxes(glm::vec3 p, glm::bvec3 axes, float grid, glm::vec3 origin = glm::vec3(0.0f)) {
    if (axes.x) p.x = Snap1D(p.x, grid, origin.x);
    if (axes.y) p.y = Snap1D(p.y, grid, origin.y);
    if (axes.z) p.z = Snap1D(p.z, grid, origin.z);
    return p;
}

// Choose which axes to snap based on the face normal we’re placing onto.
//  - Up/Down face => snap XZ (ground-like)
//  - +/-X face    => snap YZ (vertical wall)
//  - +/-Z face    => snap XY (vertical wall)
glm::bvec3 AxesForFace(const glm::vec3& n) {
    glm::vec3 an = glm::abs(n);
    if (an.y >= an.x && an.y >= an.z) return glm::bvec3(true,  false, true ); // XZ
    if (an.x >= an.y && an.x >= an.z) return glm::bvec3(false, true,  true ); // YZ
    return                                   glm::bvec3(true,  true,  false); // XY
}

