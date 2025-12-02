#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

void Start() {
    
    // Key bindings
    Input.BindKeyPressToFunction(VK_F4, keyBindF4);
    Input.BindKeyPressToFunction(VK_F11, keyBindFullscreen);
    Input.BindKeyPressToFunction(VK_ESCAPE, keyBindEscape);
    
    // Load the required command functions
    Engine.console.Enable();
    
    Engine.console.RegisterCommand("summon",  FuncSummon);
    Engine.console.RegisterCommand("list",    FuncList);
    
    Engine.console.RegisterCommand("save",    FuncSave);
    Engine.console.RegisterCommand("load",    FuncLoad);
    Engine.console.RegisterCommand("remove",  FuncRemove);
    
    Engine.console.RegisterCommand("clear",   FuncClear);
    Engine.console.RegisterCommand("seed",    FuncSeed);
    
    Engine.console.RegisterCommand("time",    FuncTime);
    Engine.console.RegisterCommand("weather", FuncWeather);
    
    // Initiate platform
    //Platform.EnableVSync();
    Platform.ShowMouseCursor();
    Platform.isPaused = true;
    
    //Engine.EnablePhysicsDebugRenderer();
    
    // Event callback
    Platform.EventCallbackLoseFocus = EventLostFocus;
    Platform.EventCallbackResize = EventOnResize;
    
    // Initiate client version
    std::string version = "0.0.1";
    GameWorld.version = version;
    Inventory.version = version;
    
    // User plug-in initiation
    GameWorld.Initiate();
    Weather.Initiate();
    Particle.Initiate();
    Inventory.Initiate(8, Platform.windowArea.w, Platform.windowArea.h);
    
    // Create a camera controller
    
    // Create a new camera controller object and 
    // assign it as the engines main camera.
    Engine.cameraController = Engine.CreateCameraController(glm::vec3(0, 0, 0));
    
    // Assign the camera controller's camera for rendering the main scene.
    Engine.sceneMain->camera = Engine.cameraController->GetComponent<Camera>();
    
    // Disable mouse look on startup.
    Engine.sceneMain->camera->DisableMouseLook();
    
    // Create a box collider for collisions.
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 1, 1);
    
    // Add the collider to the camera controller game object.
    Engine.cameraController->AddColliderBox(boxShape, 0, 0, 0, LayerMask::Ground);
    
    
    // Weather system
    
    Weather.SetPlayerObject(Engine.cameraController);
    Weather.SetWorldMaterial(GameWorld.worldMaterial);
    Weather.SetStaticMaterial(GameWorld.staticMaterial);
    Weather.SetWaterMaterial(GameWorld.waterMaterial);
    
    
    // Biome region noise
    /*
    // Plains
    Biome biomeLayerPlains;
    biomeLayerPlains.colorHeight = GameWorld.world.waterLevel + 16;
    biomeLayerPlains.colorBias = 0.024f;
    biomeLayerPlains.colorLow = Colors.green * 0.09f;
    biomeLayerPlains.colorHigh = Colors.green * 0.1f;
    biomeLayerPlains.region.offsetX = 3200;
    biomeLayerPlains.region.offsetZ = 3200;
    biomeLayerPlains.region.noiseWidth  = 0.001f;
    biomeLayerPlains.region.noiseHeight = 0.001f;
    
    // Desert
    Biome biomeLayerDesert;
    Color desertLow, desertHigh;
    desertLow  = Colors.Lerp(Colors.yellow, Colors.dkgray, 0.8f);
    desertHigh = Colors.Lerp(desertLow, Colors.red, 0.3f);
    biomeLayerDesert.colorLow = Colors.Lerp(desertLow, Colors.dkgray, 0.1f) * 0.4f;
    biomeLayerDesert.colorHigh   = Colors.Lerp(desertLow, desertHigh, 0.5f) * 0.4f;
    biomeLayerDesert.colorHeight = -2.0f;
    biomeLayerDesert.colorBias   = 0.08f;
    biomeLayerDesert.region.offsetX = 1000;
    biomeLayerDesert.region.offsetZ = 1000;
    biomeLayerDesert.region.noiseWidth  = 0.001f;
    biomeLayerDesert.region.noiseHeight = 0.001f;
    
    // Forest
    Biome biomeLayerForest;
    biomeLayerForest.colorHeight = GameWorld.world.waterLevel + 16;
    biomeLayerForest.colorBias = 0.024f;
    biomeLayerForest.colorLow = Colors.green * 0.087f;
    biomeLayerForest.colorHigh = Colors.brown * 0.4f;
    biomeLayerForest.region.offsetX = 7600;
    biomeLayerForest.region.offsetZ = 7600;
    biomeLayerForest.region.noiseWidth  = 0.001f;
    biomeLayerForest.region.noiseHeight = 0.001f;
    
    
    // Terrain generation noise
    
    // Plains
    Perlin perlinPlains;
    perlinPlains.heightMultuplier = 1.1f;
    perlinPlains.heightMin        = GameWorld.world.waterLevel + 15;
    perlinPlains.noiseWidth       = 0.08;
    perlinPlains.noiseHeight      = 0.08;
    
    Perlin perlinPlainsHills;
    perlinPlainsHills.heightMultuplier = 10;
    perlinPlainsHills.heightMin        = GameWorld.world.waterLevel + 15;
    perlinPlainsHills.noiseWidth       = 0.01;
    perlinPlainsHills.noiseHeight      = 0.01;
    
    // Desert
    Perlin perlinDesert;
    perlinDesert.heightMultuplier = 1.1f;
    perlinDesert.heightMin        = GameWorld.world.waterLevel + 15;
    perlinDesert.noiseWidth       = 0.2;
    perlinDesert.noiseHeight      = 0.2;
    perlinDesert.offsetX = 100.0f;
    perlinDesert.offsetY = 100.0f;
    
    Perlin perlinDesertHills;
    perlinDesertHills.heightMultuplier = 20.0f;
    perlinDesertHills.heightMin        = GameWorld.world.waterLevel + 15;
    perlinDesertHills.heightMax        = GameWorld.world.waterLevel + 40;
    perlinDesertHills.noiseWidth       = 0.02;
    perlinDesertHills.noiseHeight      = 0.02;
    perlinDesertHills.heightBlowoutMul     = 20.0f;
    perlinDesertHills.heightBlowoutHeight  = GameWorld.world.waterLevel + 30;
    perlinDesertHills.offsetX = 50.0f;
    perlinDesertHills.offsetY = 50.0f;
    
    // Forest
    Perlin perlinForest;
    perlinForest.heightMultuplier = 1.2f;
    perlinForest.heightMin        = GameWorld.world.waterLevel + 20;
    perlinForest.noiseWidth       = 0.2;
    perlinForest.noiseHeight      = 0.2;
    
    Perlin perlinForestRipple;
    perlinForestRipple.heightMultuplier = 10.0f;
    perlinForestRipple.heightMin        = GameWorld.world.waterLevel + 20;
    perlinForestRipple.noiseWidth       = 0.05;
    perlinForestRipple.noiseHeight      = 0.05;
    
    Perlin perlinForestHills;
    perlinForestHills.heightMultuplier = 200.0f;
    perlinForestHills.heightMin        = GameWorld.world.waterLevel + 20;
    perlinForestHills.noiseWidth       = 0.008;
    perlinForestHills.noiseHeight      = 0.008;
    
    biomeLayerPlains.perlin.push_back(perlinPlainsHills);
    biomeLayerPlains.perlin.push_back(perlinPlains);
    
    biomeLayerDesert.perlin.push_back(perlinDesertHills);
    biomeLayerDesert.perlin.push_back(perlinDesert);
    
    biomeLayerForest.perlin.push_back(perlinForestHills);
    biomeLayerForest.perlin.push_back(perlinForestRipple);
    biomeLayerForest.perlin.push_back(perlinForest);
    
    
    
    //
    // Decoration generation
    
    DecorationSpecifier decorGrass;
    decorGrass.type = DecorationType::Grass;
    decorGrass.name = "grass";
    decorGrass.density = 1000;
    decorGrass.spawnHeightMaximum = 10;
    decorGrass.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrass.threshold = 0.0f;
    decorGrass.noise = 0.5f;
    
    DecorationSpecifier decorGrassDense;
    decorGrassDense.type = DecorationType::Grass;
    decorGrassDense.name = "grass";
    decorGrassDense.density = 4000;
    decorGrassDense.spawnHeightMaximum = 10;
    decorGrassDense.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrassDense.threshold = 0.0f;
    decorGrassDense.noise = 0.5f;
    
    DecorationSpecifier decorTrees;
    decorTrees.type = DecorationType::Tree;
    decorTrees.name = "oak";
    decorTrees.density = 10;
    decorTrees.spawnHeightMaximum = 20;
    decorTrees.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorTrees.threshold = -0.08f;
    decorTrees.noise     = 0.001f;
    
    DecorationSpecifier decorTreesDense;
    decorTreesDense.type = DecorationType::Tree;
    decorTreesDense.name = "pine";
    decorTreesDense.density = 2000;
    decorTreesDense.spawnHeightMaximum = 20;
    decorTreesDense.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorTreesDense.threshold = -0.08f;
    decorTreesDense.noise     = 0.001f;
    
    biomeLayerPlains.decorations.push_back(decorGrass);
    biomeLayerPlains.decorations.push_back(decorGrassDense);
    biomeLayerPlains.decorations.push_back(decorTrees);
    biomeLayerDesert.decorations.push_back(decorGrass);
    biomeLayerForest.decorations.push_back(decorTreesDense);
    biomeLayerForest.decorations.push_back(decorGrassDense);
    biomeLayerForest.decorations.push_back(decorGrassDense);
    
    
    //
    // Actors
    
    DecorationSpecifier decorSheep;
    decorSheep.type = DecorationType::Actor;
    decorSheep.name = "Sheep";
    decorSheep.density = 40;
    decorSheep.spawnHeightMaximum = GameWorld.world.waterLevel + 30;
    decorSheep.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorSheep.threshold = 0.0f;
    decorSheep.noise     = 0.3f;
    
    DecorationSpecifier decorHorse;
    decorHorse.type = DecorationType::Actor;
    decorHorse.name = "Horse";
    decorHorse.density = 20;
    decorHorse.spawnHeightMaximum = 10;
    decorHorse.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorHorse.threshold = 0.0f;
    decorHorse.noise     = 0.5;
    
    DecorationSpecifier decorBovine;
    decorBovine.type = DecorationType::Actor;
    decorBovine.name = "Bovine";
    decorBovine.density = 40;
    decorBovine.spawnHeightMaximum = GameWorld.world.waterLevel + 10;
    decorBovine.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorBovine.threshold = 0.0f;
    decorBovine.noise     = 0.5f;
    
    DecorationSpecifier decorBear;
    decorBear.type = DecorationType::Actor;
    decorBear.name = "Bear";
    decorBear.density = 3;
    decorBear.spawnHeightMaximum = GameWorld.world.waterLevel + 70;
    decorBear.spawnHeightMinimum = GameWorld.world.waterLevel + 20;
    decorBear.threshold = 0.0f;
    decorBear.noise     = 0.5f;
    
    DecorationSpecifier decorDog;
    decorDog.type = DecorationType::Actor;
    decorDog.name = "Dog";
    decorDog.density = 10;
    decorDog.spawnHeightMaximum = GameWorld.world.waterLevel + 60;
    decorDog.spawnHeightMinimum = GameWorld.world.waterLevel + 10;
    decorDog.threshold = 0.0f;
    decorDog.noise     = 0.1f;
    
    
    DecorationSpecifier decorVillage;
    decorVillage.type = DecorationType::Structure;
    decorVillage.name = "village";
    decorVillage.density = 100;
    decorVillage.noise = 0.1f;
    decorVillage.threshold = 0.1f;
    decorVillage.spawnHeightMaximum = GameWorld.world.waterLevel + 24;
    decorVillage.spawnHeightMinimum = GameWorld.world.waterLevel + 4;
    
    
    biomeLayerPlains.decorations.push_back(decorSheep);
    biomeLayerPlains.decorations.push_back(decorVillage);
    biomeLayerPlains.decorations.push_back(decorHorse);
    
    biomeLayerDesert.decorations.push_back(decorBovine);
    
    biomeLayerForest.decorations.push_back(decorBear);
    biomeLayerForest.decorations.push_back(decorDog);
    
    GameWorld.world.biomes.push_back(biomeLayerPlains);
    GameWorld.world.biomes.push_back(biomeLayerDesert);
    GameWorld.world.biomes.push_back(biomeLayerForest);
    */
    
    
    // Structures
    /*
    Structure village;
    village.name = "village";
    village.buildSpread = 25.0f;
    village.actorCountMin = 0;
    village.actorCountMax = 8;
    village.actorSpread = 4.0f;
    
    float height = 14.0f;
    float radius = 3.0f;
    float angle = 27.0f;
    int numPoles = 8;
    
    float stickThickness = 0.8f;
    Color stickColor = Colors.brown * 0.24f;
    
    for (int i = 0; i < numPoles; i++) {
        float ang = (360.0f / numPoles) * i;
        float rad = glm::radians(ang);
        
        float x = std::cos(rad) * radius;
        float z = std::sin(rad) * radius;
        
        glm::vec3 rotation(0.0f, angle, -ang);
        
        //StructurePart pole;
        //pole.offset = glm::ivec2((int)x + 3, (int)z + 3);
        //pole.rotation = rotation;
        //pole.scale = glm::vec3(stickThickness, height, stickThickness);
        //pole.color = stickColor;
        glm::vec3 position(0, 0, 0);
        
        GameWorld.build.StackAtAngle(village, position, glm::vec3(stickThickness, 1, stickThickness), rotation, 1, height, stickColor);
        
    }
    
    // Add to world structure list
    GameWorld.world.structures.push_back(village);
    */
    
    
    
    // Structure - house
    /*
    Structure village;
    village.name = "village";
    village.buildSpread = 30;
    village.actorCountMax = 5;
    village.actorSpread = 4.0f;
    
    glm::vec3 stackPos(0, 10, 0);
    
    float spread = 4.0f;
    float height = 4.0f;
    
    float thickness = 0.5f;
    Color color = Colors.brown * 0.1f;
    glm::vec3 position(0, 0, 0);
    
    float halfExtent = 1.0f / 2.0f;
    float halfSpread = spread / 2.0f;
    float extentOffset = halfSpread - halfExtent;
    float heightOffset = height - halfExtent;
    
    for (unsigned int x=0; x < 2; x++) {
        for (unsigned int z=0; z < 2; z++) {
            position.x = (x * spread) - halfSpread;
            position.z = (z * spread) - halfSpread;
            
            GameWorld.build.StackAtAngle(village, position, glm::vec3(thickness, 1.0f, thickness), glm::vec3(0, 90, 0), 1, height, color);
        }
    }
    
    position.x = halfSpread;
    position.y = heightOffset;
    position.z = -extentOffset;
    GameWorld.build.StackAtAngle(village, position, glm::vec3(thickness, 1, thickness), glm::vec3(0), 1, spread, color);
    
    position.x = -halfSpread;
    position.y = heightOffset;
    position.z = -extentOffset;
    GameWorld.build.StackAtAngle(village, position, glm::vec3(thickness, 1, thickness), glm::vec3(0), 1, spread, color);
    
    
    position.x = -extentOffset;
    position.y = heightOffset;
    position.z = -halfSpread;
    GameWorld.build.StackAtAngle(village, position, glm::vec3(thickness, 1, thickness), glm::vec3(90, 0, 0), 1, spread, color);
    
    position.x = -extentOffset;
    position.y = heightOffset;
    position.z = halfSpread;
    GameWorld.build.StackAtAngle(village, position, glm::vec3(thickness, 1, thickness), glm::vec3(90, 0, 0), 1, spread, color);
    float offsetMul = 1.24f;
    
    for (unsigned int i=0; i < 5; i++) {
        position.x = i - halfSpread;
        position.y = heightOffset - 0.25f;
        position.z = -(halfSpread * offsetMul);
        GameWorld.build.StackAtAngle(village, position, glm::vec3(thickness, 1, thickness), glm::vec3(0, 45, 45), 1, spread, color);
        
        position.x = i - halfSpread;
        position.y = heightOffset - 0.25f;
        position.z = (halfSpread * offsetMul);
        GameWorld.build.StackAtAngle(village, position, glm::vec3(thickness, 1, thickness), glm::vec3(180, 45, 45), 1, spread, color);
    }
    
    GameWorld.world.structures.push_back(village);
    */
    
    
    
    // Lighting levels
    //Weather.SetWorldLightLevel(0.0087f, 0.3f);
    
    GameWorld.world.staticColorLow  = Colors.MakeGrayScale(0.3f);
    GameWorld.world.actorColorLow   = Colors.MakeGrayScale(0.02f);
    
    GameWorld.world.staticColorHigh = Colors.MakeGrayScale(0.7f);
    GameWorld.world.actorColorHigh  = Colors.MakeGrayScale(0.7f);
    
    // World rendering
    GameWorld.chunkSize = 64;
    GameWorld.renderDistance = 10;
    GameWorld.staticDistance = 0.8f;
    GameWorld.actorDistance  = 0.4f;
    
    GameWorld.world.snowCapHeight = 20.0f;
    GameWorld.world.snowCapBias = 8.0f;
    GameWorld.world.waterLevel = -8.0f;
    
    
    // Load world
    
    if (!GameWorld.LoadWorld()) {
        GameWorld.worldSeed = Random.Range(100, 10000000) - Random.Range(100, 10000000);
        Weather.SetTime(9000);
        Weather.SetWeather(WeatherType::Clear);
        GameWorld.SaveWorld();
    }
    
}

