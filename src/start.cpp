#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>


void Start() {
    
    // Load console functions
    Engine.ConsoleRegisterCommand("summon",  FuncSummon);
    Engine.ConsoleRegisterCommand("list",    FuncList);
    
    Engine.ConsoleRegisterCommand("save",    FuncSave);
    Engine.ConsoleRegisterCommand("load",    FuncLoad);
    Engine.ConsoleRegisterCommand("remove",  FuncRemove);
    
    Engine.ConsoleRegisterCommand("clear",   FuncClear);
    Engine.ConsoleRegisterCommand("seed",    FuncSeed);
    
    
    Platform.HideMouseCursor();
    Engine.DisableConsoleCloseOnReturn();
    
    
    // User plug-in initiation
    chunkManager.Initiate();
    
    weather.Initiate();
    
    
    
    // Event callbacks
    
    Platform.EventCallbackLoseFocus = EventLostFocus;
    
    
    //Engine.EnableProfiler();
    
    //Engine.EnablePhysicsDebugRenderer();
    
    
    //
    // Audio test sample
    
    Sound* soundA = Audio.CreateSound();
    AudioSample* sampleA = Audio.CreateAudioSample();
    
    sampleA->sample_rate = 44100;
    
    //Samples.RenderBlankSpace(sampleA, 0.4f);
    //Samples.RenderSweepingSineWave(sampleA, 17000, 11000, 0.1f);
    //Samples.RenderBlankSpace(sampleA, 0.7f);
    //Samples.RenderSweepingSineWave(sampleA, 17000, 15000, 0.1f);
    //Samples.RenderBlankSpace(sampleA, 1.0f);
    //Samples.RenderSweepingSineWave(sampleA, 17000, 15000, 0.1f);
    //Samples.RenderBlankSpace(sampleA, 0.1f);
    //Samples.RenderSweepingSineWave(sampleA, 17000, 15000, 0.1f);
    
    
    soundA->LoadSample(sampleA);
    
    //soundA->SetVolume(0.4f);
    
    //soundA->Play();
    //while (soundA->IsSamplePlaying());
    
    
    
    //
    // Create a camera controller
    //
    
    // The position of the player in the world.
    Vector3 playerPosition = Vector3(0, 30, 0);
    
    // Create a new camera controller object
    Engine.cameraController = Engine.CreateCameraController(playerPosition);
    
    // Assign the camera controller's camera for rendering scene main.
    Engine.sceneMain->camera = Engine.cameraController->GetComponent<Camera>();
    
    // Use the mouse to look around.
    Engine.sceneMain->camera->EnableMouseLook();
    
    // Create a box collider for the player.
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 1, 1);
    
    // Add the collider to the camera controller game object.
    Engine.cameraController->AddColliderBox(boxShape, 0, 0, 0, LayerMask::Ground);
    
    // Attach the sky object to the camera controller to prevent 
    // the player from moving outside of the sky.
    weather.skyObject->GetComponent<Transform>()->parent = Engine.cameraController->GetComponent<Transform>();
    
    
    
    
    
    
    //
    // Chunk generation
    //
    
    Decoration decorGrass;
    decorGrass.type = DECORATION_GRASS;
    decorGrass.density = 1000000;
    decorGrass.spawnHeightMaximum = 35;
    decorGrass.spawnHeightMinimum = chunkManager.world.waterLevel;
    decorGrass.spawnStackHeightMin = 1;
    decorGrass.spawnStackHeightMax = 2;
    decorGrass.threshold = 0.001f;
    decorGrass.noise = 0.2f;
    
    Decoration decorTrees;
    decorTrees.type = DECORATION_TREE;
    decorTrees.density = 50;
    decorTrees.spawnHeightMaximum = 20;
    decorTrees.spawnHeightMinimum = chunkManager.world.waterLevel;
    decorTrees.spawnStackHeightMin = 4;
    decorTrees.spawnStackHeightMax = 8;
    decorTrees.threshold = 0.2f;
    decorTrees.noise = 0.007f;
    
    Decoration decorTreeHights;
    decorTreeHights.type = DECORATION_TREE;
    decorTreeHights.density = 1300;
    decorTreeHights.spawnHeightMaximum = 40;
    decorTreeHights.spawnHeightMinimum = 10;
    decorTreeHights.spawnStackHeightMin = 4;
    decorTreeHights.spawnStackHeightMax = 8;
    
    Decoration decorWaterPlants;
    decorWaterPlants.type = DECORATION_GRASS_THIN;
    decorWaterPlants.density = 800;
    decorWaterPlants.spawnHeightMaximum = chunkManager.world.waterLevel;
    decorWaterPlants.spawnHeightMinimum = -100;
    decorWaterPlants.spawnStackHeightMax = 4;
    decorWaterPlants.spawnStackHeightMin = 2;
    
    // Actors
    
    Decoration decorSheep;
    decorSheep.type = DECORATION_ACTOR;
    decorSheep.name = "Sheep";
    decorSheep.density = 10;
    decorSheep.spawnHeightMaximum = 10;
    decorSheep.spawnHeightMinimum = chunkManager.world.waterLevel;
    
    Decoration decorBear;
    decorBear.type = DECORATION_ACTOR;
    decorBear.name = "Bear";
    decorBear.density = 8;
    decorBear.spawnHeightMaximum = 40;
    decorBear.spawnHeightMinimum = 5;
    
    
    chunkManager.world.mDecorations.push_back(decorGrass);
    chunkManager.world.mDecorations.push_back(decorTrees);
    chunkManager.world.mDecorations.push_back(decorTreeHights);
    
    chunkManager.world.mDecorations.push_back(decorWaterPlants);
    
    chunkManager.world.mDecorations.push_back(decorSheep);
    chunkManager.world.mDecorations.push_back(decorBear);
    
    
    // Perlin layers
    
    Perlin perlinBase;
    perlinBase.equation = 0;
    perlinBase.heightMultuplier = 8;
    perlinBase.noiseWidth  = 0.07;
    perlinBase.noiseHeight = 0.07;
    
    Perlin perlinLayerA;
    perlinLayerA.equation = 0;
    perlinLayerA.heightMultuplier = 5;
    perlinLayerA.noiseWidth  = 0.03;
    perlinLayerA.noiseHeight = 0.03;
    
    Perlin perlinLayerB;
    perlinLayerB.equation = 0;
    perlinLayerB.heightMultuplier = 20;
    perlinLayerB.noiseWidth  = 0.02;
    perlinLayerB.noiseHeight = 0.02;
    
    Perlin perlinMountainA;
    perlinMountainA.equation = 0;
    perlinMountainA.heightMultuplier = 100;
    perlinMountainA.noiseWidth  = 0.009;
    perlinMountainA.noiseHeight = 0.009;
    
    Perlin perlinMountainB;
    perlinMountainB.equation = 0;
    perlinMountainB.heightMultuplier = 300;
    perlinMountainB.noiseWidth  = 0.0007;
    perlinMountainB.noiseHeight = 0.0007;
    
    Perlin perlinFlatland;
    perlinFlatland.equation = 0;
    perlinFlatland.heightMultuplier = 10;
    perlinFlatland.noiseWidth  = 0.007;
    perlinFlatland.noiseHeight = 0.007;
    
    
    chunkManager.perlin.push_back(perlinMountainB);
    chunkManager.perlin.push_back(perlinMountainA);
    chunkManager.perlin.push_back(perlinBase);
    chunkManager.perlin.push_back(perlinLayerA);
    chunkManager.perlin.push_back(perlinLayerB);
    chunkManager.perlin.push_back(perlinFlatland);
    
    // Structure test
    Structure structure;
    
    DecorationElement element;
    element.position = glm::vec3(0.0f, 0.0f, 0.0f);
    structure.elements.push_back(element);
    
    chunkManager.world.mStructures.push_back(structure);
    
    
    
    // Lighting levels
    
    chunkManager.world.chunkColorLow   = Colors.MakeGrayScale(0.3f);
    chunkManager.world.staticColorLow  = Colors.MakeGrayScale(0.3f);
    chunkManager.world.actorColorLow   = Colors.MakeGrayScale(0.02f);
    
    chunkManager.world.chunkColorHigh  = Colors.MakeGrayScale(0.87f);
    chunkManager.world.staticColorHigh = Colors.MakeGrayScale(0.87f);
    chunkManager.world.actorColorHigh  = Colors.MakeGrayScale(0.87f);
    
    chunkManager.world.ambientLight = 0.87f;
    
    
    // World rendering
    
    chunkManager.renderDistance = 10;
    chunkManager.staticDistance = 10;
    
    return;
}

