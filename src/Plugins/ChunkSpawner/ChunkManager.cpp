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
    
    waterMesh(nullptr),
    
    fogWater(nullptr) 
{}

WorldGeneration::WorldGeneration() : 
    name(""),
    
    doGenerateChunks(false),
    doAutoBreeding(true),
    
    snowCapHeight(60.0f),
    snowCapBias(2.0f),
    snowCapColor(0.7f, 0.85f, 1.1f, 1.0f),
    
    waterLevel(-21.0f),
    ambientLight(0.0f),
    
    waterColorLow(Colors.black),
    waterColorHigh(Colors.blue),
    
    staticColorLow(Colors.black),
    staticColorHigh(Colors.white),
    
    actorColorLow(Colors.black),
    actorColorHigh(Colors.white) 
{}

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
    //Resources.meshes.wallHorizontal->ChangeSubMeshScale(0, scaler, scaler, scaler);
    //Resources.meshes.wallVertical->ChangeSubMeshScale(0, scaler, scaler, scaler);
    //Resources.meshes.plain->ChangeSubMeshScale(0, scaler, scaler, scaler);
    
    //Resources.meshes.grass->ChangeSubMeshScale(0, scaler, scaler, scaler);
    Resources.meshes.leaf->ChangeSubMeshScale(0, scaler, scaler, scaler);
    //Resources.meshes.log->ChangeSubMeshScale(0, scaler, 1.0f, scaler);
    
    
    // Get sub meshes
    std::vector<std::pair<std::string, SubMesh>> staticMeshEntries;
    
    SubMesh subMeshWallHorz;
    SubMesh subMeshWallVert;
    SubMesh subMeshPlain;
    SubMesh subMeshCube;
    SubMesh subMeshGrass;
    SubMesh subMeshLeaf;
    SubMesh subMeshLog;
    
    Resources.meshes.wallHorizontal->GetSubMesh(0, subMeshWallHorz);
    Resources.meshes.wallVertical->GetSubMesh(0, subMeshWallVert);
    
    Resources.meshes.cube->GetSubMesh(0, subMeshCube);
    Resources.meshes.cube->GetSubMesh(1, subMeshCube);
    Resources.meshes.cube->GetSubMesh(2, subMeshCube);
    Resources.meshes.cube->GetSubMesh(3, subMeshCube);
    Resources.meshes.cube->GetSubMesh(4, subMeshCube);
    Resources.meshes.cube->GetSubMesh(5, subMeshCube);
    
    Resources.meshes.plain->GetSubMesh(0, subMeshPlain);
    Resources.meshes.grass->GetSubMesh(0, subMeshGrass);
    Resources.meshes.leaf->GetSubMesh(0, subMeshLeaf);
    Resources.meshes.log->GetSubMesh(0, subMeshLog);
    
    staticMeshEntries.push_back({"wallhorizontal", subMeshWallHorz});
    staticMeshEntries.push_back({"wallverticle", subMeshWallVert});
    staticMeshEntries.push_back({"cube", subMeshCube});
    staticMeshEntries.push_back({"plain", subMeshPlain});
    staticMeshEntries.push_back({"grass", subMeshGrass});
    staticMeshEntries.push_back({"leaf", subMeshLeaf});
    staticMeshEntries.push_back({"log", subMeshLog});
    
    Mesh* mesh = Engine.Create<Mesh>();
    mesh->AddCube(0, 0, 0, 0.5f, 0.5f - 0.25f, 0.5f, Colors.white);
    SubMesh subMesh;
    mesh->GetSubMesh(0, subMesh);
    
    staticMeshEntries.push_back({"slab", subMesh});
    
    unsigned int index=1;
    for (const auto& pair : staticMeshEntries) {
        mStaticMeshes[pair.first] = pair.second;
        mStaticMeshToIndex[pair.first] = index;
        mStaticIndexToMesh[index] = pair.first;
        index++;
    }
    
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
    worldMaterial->shader = Resources.shaders.color;
    
    staticMaterial->DisableCulling();
    staticMaterial->diffuse = Colors.gray;
    staticMaterial->ambient = Colors.MakeGrayScale(0.2f);
    staticMaterial->shader = Resources.shaders.color;
    
    waterMaterial->DisableCulling();
    waterMaterial->EnableBlending();
    waterMaterial->diffuse = Colors.blue * Colors.MakeGrayScale(0.4f);
    waterMaterial->shader = Resources.shaders.water;
    
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
                
                ClassStructure structure;
                
                for (unsigned int a=0; a < loader.rawData.size(); a++) {
                    std::string line = loader.rawData[a];
                    std::vector<std::string> params = String.Explode(line, ' ');
                    
                    for (unsigned int p=0; p < params.size(); p++) {
                        
                        if (params[p] == "stack") {
                            params.erase(params.begin());
                            if (params.size() < 9) 
                                continue;
                            
                            ClassStructure::SubStructureStack stack;
                            stack.name = params[p];
                            
                            stack.heightMin = String.ToFloat(params[p+1]);
                            stack.heightMax = String.ToFloat(params[p+2]);
                            
                            stack.position  = {String.ToFloat(params[p+3]),
                                               String.ToFloat(params[p+4]),
                                               String.ToFloat(params[p+5])};
                            
                            stack.rotation  = {String.ToFloat(params[p+6]),
                                               String.ToFloat(params[p+7]),
                                               String.ToFloat(params[p+8])};
                            
                            structure.stacks.push_back(stack);
                            
                            continue;
                        }
                        
                        if (params[p] == "place") {
                            params.erase(params.begin());
                            if (params.size() < 7) 
                                continue;
                            
                            ClassStructure::SubStructurePlace place;
                            place.name      = params[p];
                            place.position  = {String.ToFloat(params[p+1]),
                                               String.ToFloat(params[p+2]),
                                               String.ToFloat(params[p+3])};
                            place.rotation  = {String.ToFloat(params[p+4]),
                                               String.ToFloat(params[p+5]),
                                               String.ToFloat(params[p+6])};
                            
                            structure.places.push_back(place);
                            continue;
                        }
                        
                        if (params[p] == "fill") {
                            params.erase(params.begin());
                            if (params.size() < 7) 
                                continue;
                            
                            ClassStructure::SubStructureFill fill;
                            fill.name    = params[p];
                            fill.from    = {String.ToFloat(params[p+1]),
                                            String.ToFloat(params[p+2]),
                                            String.ToFloat(params[p+3])};
                            fill.to      = {String.ToFloat(params[p+4]),
                                            String.ToFloat(params[p+5]),
                                            String.ToFloat(params[p+6])};
                            
                            structure.fills.push_back(fill);
                            continue;
                        }
                        
                        if (params[p] == "pattern") {
                            params.erase(params.begin());
                            if (params.size() < 6) 
                                continue;
                            
                            ClassStructure::SubStructurePattern pattern;
                            pattern.name    = params[p];
                            pattern.mesh    = params[p+1];
                            pattern.pattern = params[p+2];
                            
                            pattern.position  = {String.ToFloat(params[p+3]),
                                                String.ToFloat(params[p+4]),
                                                String.ToFloat(params[p+5])};
                            
                            structure.patterns.push_back(pattern);
                            continue;
                        }
                        
                    }
                    
                    world.classStructures[nameParts[0]] = structure;
                }
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
            
            // Check 'id'
            if (loader.assetData.find("id") != loader.assetData.end() && !loader.assetData["id"].empty()) {
                definition.id = String.ToInt(loader.assetData["id"][0]);
            }
            
            // Check 'function'
            if (loader.assetData.find("function") != loader.assetData.end() && !loader.assetData["function"].empty()) {
                definition.function = String.ToUint(loader.assetData["function"][0]);
            }
            
            // Check 'mesh'
            if (loader.assetData.find("mesh") != loader.assetData.end() && !loader.assetData["mesh"].empty()) {
                std::string meshVal = loader.assetData["mesh"][0];
                definition.mesh = (meshVal == "none") ? "" : meshVal;
            } else {
                definition.mesh = "";
            }
            
            // Check 'width'
            if (loader.assetData.find("width") != loader.assetData.end() && !loader.assetData["width"].empty()) {
                definition.width = String.ToFloat(loader.assetData["width"][0]);
            }
            
            // Check 'height'
            if (loader.assetData.find("height") != loader.assetData.end() && !loader.assetData["height"].empty()) {
                definition.height = String.ToFloat(loader.assetData["height"][0]);
            }
            
            // Check 'colorMin'
            if (loader.assetData.find("colorMin") != loader.assetData.end() && !loader.assetData["colorMin"].empty()) {
                std::string colorMinStr = "";
                for (unsigned int a=0; a < loader.assetData["colorMin"].size(); a++) {
                    colorMinStr += loader.assetData["colorMin"][a];
                }
                definition.colorMin = GetColorByName(colorMinStr);
            }
            
            // Check 'colorMax'
            if (loader.assetData.find("colorMax") != loader.assetData.end() && !loader.assetData["colorMax"].empty()) {
                std::string colorMaxStr = "";
                for (unsigned int a=0; a < loader.assetData["colorMax"].size(); a++) {
                    colorMaxStr += loader.assetData["colorMax"][a];
                }
                definition.colorMax = GetColorByName(colorMaxStr);
            }
            
            // Check 'stackMax'
            if (loader.assetData.find("stackMax") != loader.assetData.end() && !loader.assetData["stackMax"].empty()) {
                definition.stackMax = String.ToUint(loader.assetData["stackMax"][0]);
            }
            
            // Check 'bitmap'
            if (loader.assetData.find("bitmap") != loader.assetData.end() && !loader.assetData["bitmap"].empty()) {
                definition.bitmap = loader.assetData["bitmap"][0];
            }
            
            // Check 'alignment'
            if (loader.assetData.find("alignment") != loader.assetData.end() && !loader.assetData["alignment"].empty()) {
                if (loader.assetData["alignment"][0] == "grid") {
                    definition.alignment = 1;
                } else {
                    definition.alignment = 0;
                }
            } else {
                definition.alignment = 0;
            }
            
            world.classDefinitions[name] = definition;
            
            world.classIndexToName[definition.id] = name;
            world.classNameToIndex[name] = definition.id;
        }
        
    }
    
    
    //
    // Load actor classes
    if (fs.DirectoryExists(classesPath)) {
        std::vector<std::string> dirList = fs.DirectoryGetList(classesPath);
        
        for (unsigned int i=0; i < dirList.size(); i++) {
            std::string filename = dirList[i];
            
            if (filename.find(".genome") == std::string::npos) 
                continue;
            
            std::vector<std::string> nameParts = String.Explode(filename, '.');
            if (nameParts.size() != 2)
                continue;
            
            FileLoader loader(classesPath + filename);
            std::string name = nameParts[0];
            
            ClassActor definition;
            
            if (loader.dataBlocks.find("genome") != loader.dataBlocks.end()) {
                std::string genome = loader.dataBlocks["genome"];
                genome.erase(std::remove(genome.begin(), genome.end(), '\n'), genome.end());
                definition.genome = genome;
            }
            
            world.classActors[name] = definition;
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
                std::string height       = loader.assetData["height"][0];
                std::string sharpness    = loader.assetData["sharpness"][0];
                
                for (unsigned int a=0; a < loader.assetData["colorLow"].size(); a++) colorLow += loader.assetData["colorLow"][a];
                for (unsigned int a=0; a < loader.assetData["colorHigh"].size(); a++) colorHigh += loader.assetData["colorHigh"][a];
                
                biome.colorLow           = GetColorByName(colorLow);
                biome.colorHigh          = GetColorByName(colorHigh);
                biome.colorBias          = String.ToFloat(colorBias);
                biome.colorHeight        = String.ToFloat(colorHeight);
                biome.height             = String.ToFloat(height);
                biome.sharpness          = String.ToFloat(sharpness);
                
                biome.region.noiseWidth  = String.ToFloat(noiseWidth);
                biome.region.noiseHeight = String.ToFloat(noiseHeight);
                biome.region.offsetX     = String.ToFloat(offsetX);
                biome.region.offsetZ     = String.ToFloat(offsetZ);
                
                for (unsigned int l=0; l < loader.rawData.size(); l++) {
                    std::string& line = loader.rawData[l];
                    
                    size_t firstChar = line.find_first_not_of(" \t\r\n");
                    if (firstChar == std::string::npos || line[firstChar] == ';') 
                        continue;
                    
                    if (line.find("perlin") != std::string::npos) {
                        line.erase(line.begin(), line.begin()+6);
                        std::vector<std::string> splitLines = String.Explode(line, ' ');
                        if (splitLines.size() == 7) {
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
                    }
                    
                    if (line.find("decoration") != std::string::npos) {
                        line.erase(line.begin(), line.begin()+10);
                        std::vector<std::string> splitLines = String.Explode(line, ' ');
                        if (splitLines.size() == 6) {
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
                    
                }
                
                world.biomes.push_back(biome);
            }
        }
    }
}

Actor* ChunkManager::SummonActor(glm::vec3 position) {
    Actor* actor = AI.CreateActor();
    actor->navigation.SetPosition(position);
    actor->navigation.SetTargetPoint(position);
    
    std::string homePosition = Float.ToString(position.x) + "," +
                               Float.ToString(position.y) + "," +
                               Float.ToString(position.z);
    
    actor->memories.Add("home", "home:" + homePosition);
    
    actor->memories.Add("sentience", "quota:0.3");
    actor->memories.Add("behavior",  "curiosity:0.14 ^ libido:0.04 ^ social:0.08");
    
    return actor;
}

void ChunkManager::KillActor(Actor* actor) {
    AI.DestroyActor( actor );
}


Color ChunkManager::GetColorByName(const std::string& name) {
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



bool ChunkManager::InitiateWorldDirectory(void) {
    std::string worldName   = "worlds\\" + world.name;
    std::string worldChunks = "worlds\\" + world.name + "\\chunks";
    std::string worldStatic = "worlds\\" + world.name + "\\static";
    std::string worldItems  = "worlds\\" + world.name + "\\items";
    
    if (!fs.DirectoryExists(worldName)) {
        fs.DirectoryCreate(worldName);
        if (!fs.DirectoryExists(worldChunks)) fs.DirectoryCreate(worldChunks);
        if (!fs.DirectoryExists(worldStatic)) fs.DirectoryCreate(worldStatic);
        if (!fs.DirectoryExists(worldItems))  fs.DirectoryCreate(worldItems);
        return false;
    }
    
    if (!fs.DirectoryExists(worldItems)) fs.DirectoryCreate(worldItems);
    return true;
}

void ChunkManager::ClearWorld(void) {
    std::lock_guard<std::mutex> lock(mux);
    
    world.doGenerateChunks = false;
    isInitiated = false;
    
    for (unsigned int c=chunks.Size(); c > 0; c--) 
        DestroyChunk( chunks[c-1] );
    
    unsigned int numberOfActors = AI.GetNumberOfActors();
    for (unsigned int a=numberOfActors; a > 0; a--)
        KillActor( AI.GetActor(a-1) );
    
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
    fs.DirectoryDelete( worldPath + "\\items" );
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
