#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
#include <GameEngineFramework/Timer/Timer.h>

void chunkGenerationThread(void);

ChunkManager::ChunkManager() :
    version("0.0.0"),
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
    
    fogWater(nullptr) {}


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
    //Engine.meshes.wallHorizontal->ChangeSubMeshScale(0, scaler, scaler, scaler);
    //Engine.meshes.wallVertical->ChangeSubMeshScale(0, scaler, scaler, scaler);
    //Engine.meshes.plain->ChangeSubMeshScale(0, scaler, scaler, scaler);
    
    //Engine.meshes.grass->ChangeSubMeshScale(0, scaler, scaler, scaler);
    Engine.meshes.leaf->ChangeSubMeshScale(0, scaler, scaler, scaler);
    //Engine.meshes.log->ChangeSubMeshScale(0, scaler, 1.0f, scaler);
    
    // Get sub meshes
    SubMesh subMeshWallHorz;
    SubMesh subMeshWallVert;
    SubMesh subMeshPlain;
    SubMesh subMeshGrass;
    SubMesh subMeshLeaf;
    SubMesh subMeshLog;
    
    Engine.meshes.wallHorizontal->GetSubMesh(0, subMeshWallHorz);
    Engine.meshes.wallVertical->GetSubMesh(0, subMeshWallVert);
    Engine.meshes.plain->GetSubMesh(0, subMeshPlain);
    Engine.meshes.grass->GetSubMesh(0, subMeshGrass);
    Engine.meshes.leaf->GetSubMesh(0, subMeshLeaf);
    Engine.meshes.log->GetSubMesh(0, subMeshLog);
    
    mStaticMeshes["wallhorizontal"]  = subMeshWallHorz;
    mStaticMeshes["wallverticle"]    = subMeshWallVert;
    mStaticMeshes["plain"]           = subMeshPlain;
    mStaticMeshes["grass"]           = subMeshGrass;
    mStaticMeshes["leaf"]            = subMeshLeaf;
    mStaticMeshes["log"]             = subMeshLog;
    
    mStaticMeshToIndex["wallhorizontal"]  = 1;
    mStaticMeshToIndex["wallverticle"]    = 2;
    mStaticMeshToIndex["plain"]           = 3;
    mStaticMeshToIndex["grass"]           = 4;
    mStaticMeshToIndex["leaf"]            = 5;
    mStaticMeshToIndex["log"]             = 6;
    
    mStaticIndexToMesh[1] = "wallhorizontal";
    mStaticIndexToMesh[2] = "wallverticle";
    mStaticIndexToMesh[3] = "plain";
    mStaticIndexToMesh[4] = "grass";
    mStaticIndexToMesh[5] = "leaf";
    mStaticIndexToMesh[6] = "log";
    
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
    
    std::string versionsDirectory    = "versions/";
    std::string classesDirectory     = "/classes/";
    std::string regionsDirectory     = "/regions/";
    std::string structuresDirectory  = "/structures/";
    
    //
    // Load generation structures
    std::string structuresPath = versionsDirectory + version + structuresDirectory;
    if (fs.DirectoryExists(structuresPath)) {
        
        std::vector<std::string> dirList = fs.DirectoryGetList(structuresPath);
        for (unsigned int i=0; i < dirList.size(); i++) {
            std::string filename = dirList[i];
            
            if (filename.find(".struct") != std::string::npos) {
                std::vector<std::string> nameParts = String.Explode(filename, '.');
                
                if (nameParts.size() != 2)
                    continue;
                FileLoader loader(structuresPath + filename);
                
                for (unsigned int a=0; a < loader.rawData.size(); a++) {
                    std::string line = loader.rawData[a];
                    
                    std::vector<std::string> params = String.Explode(line, ' ');
                    Engine.console.Print( params[0] );
                    
                }
                
                
                
            }
            
        }
        
    }
    
    
    //
    // Load biome regions
    std::string regionsPath = versionsDirectory + version + regionsDirectory;
    if (fs.DirectoryExists(regionsPath)) {
        std::vector<std::string> dirList = fs.DirectoryGetList(regionsPath);
        
        for (unsigned int i=0; i < dirList.size(); i++) {
            std::string filename = dirList[i];
            
            if (filename.find(".biome") != std::string::npos) {
                std::vector<std::string> nameParts = String.Explode(filename, '.');
                if (nameParts.size() != 2)
                    continue;
                
                FileLoader loader(regionsPath + filename);
                std::string name = nameParts[0];
                
                Biome biome;
                std::string colorLow;
                std::string colorHigh;
                std::string colorBias    = loader.assetData["colorBias"][0];
                std::string colorHeight  = loader.assetData["colorHeight"][0];
                std::string noiseWidth   = loader.assetData["noiseWidth"][0];
                std::string noiseHeight  = loader.assetData["noiseHeight"][0];
                std::string offsetX      = loader.assetData["offsetX"][0];
                std::string offsetZ      = loader.assetData["offsetZ"][0];
                
                for (unsigned int a=0; a < loader.assetData["colorLow"].size(); a++) colorLow += loader.assetData["colorLow"][a];
                for (unsigned int a=0; a < loader.assetData["colorHigh"].size(); a++) colorHigh += loader.assetData["colorHigh"][a];
                
                biome.colorLow           = GetColorByName(colorLow);
                biome.colorHigh          = GetColorByName(colorHigh);
                biome.colorBias          = String.ToFloat(colorBias);
                biome.colorHeight        = String.ToFloat(colorHeight);
                biome.region.noiseWidth  = String.ToFloat(noiseWidth);
                biome.region.noiseHeight = String.ToFloat(noiseHeight);
                biome.region.offsetX     = String.ToFloat(offsetX);
                biome.region.offsetZ     = String.ToFloat(offsetZ);
                
                for (unsigned int l=0; l < loader.rawData.size(); l++) {
                    std::string& line = loader.rawData[l];
                    
                    if (line.find("perlin") != std::string::npos) {
                        line.erase(line.begin(), line.begin()+6);
                        std::vector<std::string> splitLines = String.Explode(line, ' ');
                        if (splitLines.size() < 7) 
                            continue;
                        Perlin layer;
                        layer.heightMultuplier = String.ToFloat(splitLines[0]);
                        layer.heightMin        = String.ToFloat(splitLines[1]);
                        layer.heightMax        = String.ToFloat(splitLines[2]);
                        layer.offsetX          = String.ToInt(splitLines[3]);
                        layer.offsetY          = String.ToInt(splitLines[4]);
                        layer.noiseWidth       = String.ToFloat(splitLines[5]);
                        layer.noiseHeight      = String.ToFloat(splitLines[6]);
                        
                        biome.perlin.push_back(layer);
                        continue;
                    }
                    
                    if (line.find("decoration") != std::string::npos) {
                        line.erase(line.begin(), line.begin()+10);
                        std::vector<std::string> splitLines = String.Explode(line, ' ');
                        if (splitLines.size() < 6) 
                            continue;
                        DecorationSpecifier decoration;
                        decoration.name               = splitLines[0];
                        decoration.density            = String.ToUint(splitLines[1]);
                        decoration.noise              = String.ToFloat(splitLines[2]);
                        decoration.threshold          = String.ToFloat(splitLines[3]);
                        decoration.spawnHeightMinimum = String.ToFloat(splitLines[4]);
                        decoration.spawnHeightMaximum = String.ToFloat(splitLines[5]);
                        
                        biome.decorations.push_back(decoration);
                        continue;
                    }
                    
                    if (line.find("actor") != std::string::npos) {
                        line.erase(line.begin(), line.begin()+5);
                        std::vector<std::string> splitLines = String.Explode(line, ' ');
                        if (splitLines.size() < 6) 
                            continue;
                        DecorationSpecifier decoration;
                        decoration.name               = splitLines[0];
                        decoration.density            = String.ToUint(splitLines[1]);
                        decoration.noise              = String.ToFloat(splitLines[2]);
                        decoration.threshold          = String.ToFloat(splitLines[3]);
                        decoration.spawnHeightMinimum = String.ToFloat(splitLines[4]);
                        decoration.spawnHeightMaximum = String.ToFloat(splitLines[5]);
                        
                        biome.decorations.push_back(decoration);
                        continue;
                    }
                    
                }
                
                world.biomes.push_back(biome);
            }
            
        }
        
    }
    
    
    //
    // Load item classes
    std::string classesPath = versionsDirectory + version + classesDirectory;
    if (fs.DirectoryExists(classesPath)) {
        std::vector<std::string> dirList = fs.DirectoryGetList(classesPath);
        
        for (unsigned int i=0; i < dirList.size(); i++) {
            std::string filename = dirList[i];
            
            if (filename.find(".class") == std::string::npos) 
                continue;
            
            std::vector<std::string> nameParts = String.Explode(filename, '.');
            if (nameParts.size() != 2)
                continue;
            
            FileLoader loader(classesPath + filename);
            std::string name = nameParts[0];
            
            ClassDefinition definition;
            
            std::string index    = loader.assetData["id"][0];
            std::string mesh     = loader.assetData["mesh"][0];
            std::string width    = loader.assetData["width"][0];
            std::string height   = loader.assetData["height"][0];
            std::string colorMin;
            std::string colorMax;
            std::string stackMax = loader.assetData["stackMax"][0];
            std::string bitmap   = loader.assetData["bitmap"][0];
            
            for (unsigned int a=0; a < loader.assetData["colorMin"].size(); a++) colorMin += loader.assetData["colorMin"][a];
            for (unsigned int a=0; a < loader.assetData["colorMax"].size(); a++) colorMax += loader.assetData["colorMax"][a];
            
            if (loader.assetData.find("id") != loader.assetData.end())       definition.id     = String.ToInt(index);
            if (loader.assetData.find("mesh") != loader.assetData.end())     definition.mesh   = mesh;
            if (loader.assetData.find("width") != loader.assetData.end())    definition.width  = String.ToFloat(width);
            if (loader.assetData.find("height") != loader.assetData.end())   definition.height = String.ToFloat(height);
            
            if (loader.assetData.find("colorMin") != loader.assetData.end()) definition.colorMin = GetColorByName(colorMin);
            if (loader.assetData.find("colorMax") != loader.assetData.end()) definition.colorMax = GetColorByName(colorMax);
            
            if (loader.assetData.find("stackMax") != loader.assetData.end()) definition.stackMax = String.ToUint(stackMax);
            if (loader.assetData.find("bitmap") != loader.assetData.end())   definition.bitmap   = bitmap;
            
            world.classDefinitions[name] = definition;
            
            world.classIndexToName[definition.id] = name;
            world.classNameToIndex[name] = definition.id;
        }
        
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
    bool firstColor = true;
    
    for (unsigned int i = 0; i < parts.size(); i++) {
        std::string token = parts[i];
        
        // Try to interpret as a number
        if (String.IsFloat(token)) {
            result *= String.ToFloat(token);
            continue;
        }
        
        float damp = 0.87f;
        float bias = 0.5f;
        if (firstColor)
            bias = 1.0f;
        
        if (token == "red")     {result = Colors.Lerp(result, Colors.red,     bias) * damp; firstColor = false;}
        if (token == "green")   {result = Colors.Lerp(result, Colors.green,   bias) * damp; firstColor = false;}
        if (token == "blue")    {result = Colors.Lerp(result, Colors.blue,    bias) * damp; firstColor = false;}
        if (token == "dkred")   {result = Colors.Lerp(result, Colors.dkred,   bias) * damp; firstColor = false;}
        if (token == "dkgreen") {result = Colors.Lerp(result, Colors.dkgreen, bias) * damp; firstColor = false;}
        if (token == "dkblue")  {result = Colors.Lerp(result, Colors.dkblue,  bias) * damp; firstColor = false;}
        if (token == "yellow")  {result = Colors.Lerp(result, Colors.yellow,  bias) * damp; firstColor = false;}
        if (token == "orange")  {result = Colors.Lerp(result, Colors.orange,  bias) * damp; firstColor = false;}
        if (token == "purple")  {result = Colors.Lerp(result, Colors.purple,  bias) * damp; firstColor = false;}
        if (token == "gray")    {result = Colors.Lerp(result, Colors.gray,    bias) * damp; firstColor = false;}
        if (token == "ltgray")  {result = Colors.Lerp(result, Colors.ltgray,  bias) * damp; firstColor = false;}
        if (token == "dkgray")  {result = Colors.Lerp(result, Colors.dkgray,  bias) * damp; firstColor = false;}
        if (token == "white")   {result = Colors.Lerp(result, Colors.white,   bias) * damp; firstColor = false;}
        if (token == "black")   {result = Colors.Lerp(result, Colors.black,   bias) * damp; firstColor = false;}
        if (token == "brown")   {result = Colors.Lerp(result, Colors.brown,   bias) * damp; firstColor = false;}
    
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
    std::lock_guard<std::mutex> lock(mux);
    
    world.doGenerateChunks = false;
    isInitiated = false;
    
    for (unsigned int c=chunks.Size()-1; c >= 0; c--) {
        DestroyChunk( chunks[c] );
        if (c == 0) break;
    }
    
    unsigned int numberOfActors = AI.GetNumberOfActors();
    for (unsigned int a=0; a < numberOfActors; a++)
        KillActor( AI.GetActor(a) );
    
    mChunkCounterX = 0;
    mChunkCounterZ = 0;
    
    generating.clear();
    mWorldRules.clear();
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
        if (!GameWorld.world.doGenerateChunks)
            continue;
        if (GameWorld.generating.size() == 0)
            continue;
        
        std::lock_guard<std::mutex> lock(GameWorld.mux);
        
        Chunk* chunk = GameWorld.generating[0];
        GameWorld.generating.erase( GameWorld.generating.begin() );
        
        GameWorld.GenerateChunkBlendMasks(chunk);
        
        chunk->isGenerated = true;
    }
    Log.Write( " >> Shutting down chunk generation" );
}
