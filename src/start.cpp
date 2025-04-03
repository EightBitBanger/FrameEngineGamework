#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>



// Menu UI
GameObject* menuPanelObject;
GameObject* versionTextObject;

Button* loadWorldButton;
Button* saveWorldButton;
Button* clearWorldButton;
Button* quitButton;






#define SAMPLE_RATE 48000
#define TONE_HZ 550
#define DURATION_SECONDS 2
#define VOLUME 28000

void Start() {
    
    /*
    
    SDL_AudioSpec spec;
    spec.freq = SAMPLE_RATE;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;
    
    // Open an audio stream without a callback
    SDL_AudioStream* stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    
    // Generate a full sine wave buffer
    int total_samples = SAMPLE_RATE * DURATION_SECONDS;
    std::vector<int16_t> buffer(total_samples);
    for (int i = 0; i < total_samples; ++i) {
        double t = (double)i / SAMPLE_RATE;
        buffer[i] = (int16_t)(VOLUME * sin(2.0 * 3.14159f * TONE_HZ * t));
    }
    
    // Send buffer to stream
    SDL_PutAudioStreamData(stream, buffer.data(), total_samples * sizeof(int16_t));
    SDL_FlushAudioStream(stream);  // Let SDL know we’re done feeding data
    
    // Start playback
    SDL_ResumeAudioStreamDevice(stream);
    
    // Wait for the audio to finish playing
    SDL_Delay(DURATION_SECONDS * 1000 + 250);  // small buffer to ensure playback finishes
    
    // Cleanup
    SDL_DestroyAudioStream(stream);
    */
    
    
    
    
    
    // Load console functions
    Engine.ConsoleRegisterCommand("summon",  FuncSummon);
    Engine.ConsoleRegisterCommand("list",    FuncList);
    
    Engine.ConsoleRegisterCommand("save",    FuncSave);
    Engine.ConsoleRegisterCommand("load",    FuncLoad);
    Engine.ConsoleRegisterCommand("remove",  FuncRemove);
    
    Engine.ConsoleRegisterCommand("clear",   FuncClear);
    Engine.ConsoleRegisterCommand("seed",    FuncSeed);
    
    Engine.ConsoleRegisterCommand("time",    FuncTime);
    Engine.ConsoleRegisterCommand("weather", FuncWeather);
    
    
    Platform.ShowMouseCursor();
    Engine.DisableConsoleCloseOnReturn();
    
    Platform.isPaused = true;
    Engine.EnableConsole();
    Engine.EnableConsoleFadeOutTextElements();
    
    
    // User plug-in initiation
    GameWorld.Initiate();
    
    Weather.Initiate();
    
    Particle.Initiate();
    
    
    
    
    
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
    //Samples.RenderSweepingSineWave(sampleA, 120, 10, 2.0f);
    Samples.RenderSquareWave(sampleA, 400, 0.24);
    
    soundA->LoadSample(sampleA);
    
    //soundA->SetVolume(0.1f);
    
    soundA->Play();
    //while (soundA->IsSamplePlaying());
    
    
    
    
    
    //
    // Create a camera controller
    //
    
    // The position of the player in the world.
    Vector3 playerPosition = Vector3(0, 0, 0);
    
    // Create a new camera controller object
    Engine.cameraController = Engine.CreateCameraController(playerPosition);
    
    // Assign the camera controller's camera for rendering the main scene.
    Engine.sceneMain->camera = Engine.cameraController->GetComponent<Camera>();
    
    // Disable mouse look on startup.
    Engine.sceneMain->camera->DisableMouseLook();
    
    // Create a box collider for the player.
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 1, 1);
    
    // Add the collider to the camera controller game object.
    Engine.cameraController->AddColliderBox(boxShape, 0, 0, 0, LayerMask::Ground);
    
    // Weather system
    
    Weather.SetPlayerObject(Engine.cameraController);
    Weather.SetWorldMaterial(GameWorld.worldMaterial);
    Weather.SetStaticMaterial(GameWorld.staticMaterial);
    Weather.SetWaterMaterial(GameWorld.waterMaterial);
    
    
    
    
    //
    // Chunk generation
    //
    
    DecorationSpecifier decorGrass;
    decorGrass.type = DECORATION_GRASS;
    decorGrass.density = 2000;
    decorGrass.spawnHeightMaximum = 35;
    decorGrass.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrass.spawnStackHeightMin = 1;
    decorGrass.spawnStackHeightMax = 2;
    decorGrass.threshold = 0.2f;
    decorGrass.noise = 0.04f;
    
    DecorationSpecifier decorTrees;
    decorTrees.type = DECORATION_TREE;
    decorTrees.density = 8;
    decorTrees.spawnHeightMaximum = 20;
    decorTrees.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorTrees.spawnStackHeightMin = 4;
    decorTrees.spawnStackHeightMax = 8;
    decorTrees.threshold = 0.08f;
    decorTrees.noise = 0.3f;
    
    DecorationSpecifier decorTreeHights;
    decorTreeHights.type = DECORATION_TREE;
    decorTreeHights.density = 40;
    decorTreeHights.spawnHeightMaximum = 40;
    decorTreeHights.spawnHeightMinimum = 10;
    decorTreeHights.spawnStackHeightMin = 4;
    decorTreeHights.spawnStackHeightMax = 8;
    decorTreeHights.threshold = 0.8f;
    decorTreeHights.noise = 0.3f;
    
    DecorationSpecifier decorWaterPlants;
    decorWaterPlants.type = DECORATION_GRASS_THIN;
    decorWaterPlants.density = 40;
    decorWaterPlants.spawnHeightMaximum = GameWorld.world.waterLevel;
    decorWaterPlants.spawnHeightMinimum = -100;
    decorWaterPlants.spawnStackHeightMax = 4;
    decorWaterPlants.spawnStackHeightMin = 2;
    decorWaterPlants.threshold = 0.1f;
    decorWaterPlants.noise = 0.4f;
    
    // Actors
    
    DecorationSpecifier decorSheep;
    decorSheep.type = DECORATION_ACTOR;
    decorSheep.name = "Sheep";
    decorSheep.density = 13;
    decorSheep.spawnHeightMaximum = 10;
    decorSheep.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorSheep.threshold = 0.1f;
    decorSheep.noise = 0.4f;
    
    DecorationSpecifier decorBear;
    decorBear.type = DECORATION_ACTOR;
    decorBear.name = "Bear";
    decorBear.density = 5;
    decorBear.spawnHeightMaximum = 40;
    decorBear.spawnHeightMinimum = 5;
    decorBear.threshold = 0.1f;
    decorBear.noise = 0.4f;
    
    
    GameWorld.world.mDecorations.push_back(decorGrass);
    GameWorld.world.mDecorations.push_back(decorTrees);
    GameWorld.world.mDecorations.push_back(decorTreeHights);
    
    GameWorld.world.mDecorations.push_back(decorWaterPlants);
    
    //GameWorld.world.mDecorations.push_back(decorSheep);
    //GameWorld.world.mDecorations.push_back(decorBear);
    
    
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
    
    
    GameWorld.perlin.push_back(perlinMountainB);
    GameWorld.perlin.push_back(perlinMountainA);
    GameWorld.perlin.push_back(perlinBase);
    GameWorld.perlin.push_back(perlinLayerA);
    GameWorld.perlin.push_back(perlinLayerB);
    GameWorld.perlin.push_back(perlinFlatland);
    
    
    // Structure test
    Structure structure;
    structure.name = "";
    structure.rarity = 300000;
    
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 0, 0)) );
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 1, 0)) );
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 2, 0)) );
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 3, 0)) );
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 4, 0)) );
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 5, 0)) );
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 6, 0)) );
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 7, 0)) );
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 8, 0)) );
    structure.elements.push_back( DecorationElement(DECORATION_TREE, glm::vec3(0, 9, 0)) );
    
    GameWorld.world.mStructures.push_back(structure);
    
    
    
    // Lighting levels
    
    GameWorld.world.chunkColorLow   = Colors.MakeGrayScale(0.3f);
    GameWorld.world.staticColorLow  = Colors.MakeGrayScale(0.3f);
    GameWorld.world.actorColorLow   = Colors.MakeGrayScale(0.02f);
    
    GameWorld.world.chunkColorHigh  = Colors.MakeGrayScale(0.87f);
    GameWorld.world.staticColorHigh = Colors.MakeGrayScale(0.87f);
    GameWorld.world.actorColorHigh  = Colors.MakeGrayScale(0.87f);
    
    GameWorld.world.ambientLight = 0.87f;
    
    
    // World rendering
    GameWorld.chunkSize = 50;
    
    GameWorld.renderDistance = 13;
    GameWorld.staticDistance = GameWorld.renderDistance * 0.7f;
    GameWorld.actorDistance  = GameWorld.renderDistance * 0.8f;
    
    
    return;
    
    // Initiate UI
    
    // Menu overlay
    menuPanelObject = Engine.CreateOverlayPanelRenderer(0, 300, 230, 150, "panel_menu");
    Panel* panel = menuPanelObject->GetComponent<Panel>();
    panel->canvas.anchorCenterHorz = true;
    
    MeshRenderer* menuPanelRenderer = menuPanelObject->GetComponent<MeshRenderer>();
    Engine.sceneOverlay->AddMeshRendererToSceneRoot(menuPanelRenderer);
    menuPanelRenderer->material->ambient = Colors.MakeGrayScale(0.7f);
    
    // Version text
    Color versionColor = Colors.Lerp(Colors.black, Colors.green, 0.37f);
    versionTextObject = Engine.CreateOverlayTextRenderer(-15, -1, "0.1.0", 9, versionColor, "font");
    MeshRenderer* versionTextRenderer = versionTextObject->GetComponent<MeshRenderer>();
    Engine.sceneOverlay->AddMeshRendererToSceneRoot(versionTextRenderer);
    
    versionTextRenderer->material->diffuse = Colors.MakeGrayScale(0.24f);
    
    Text* versionText = versionTextObject->GetComponent<Text>();
    versionText->canvas.anchorTop = false;
    versionText->canvas.anchorRight = true;
    
    // Menu buttons
    loadWorldButton  = Engine.CreateButtonUI(0, 260, 115, 24,-4.1f, 1.25f, "Load world",  "button_blue", ButtonLoadWorld);
    saveWorldButton  = Engine.CreateButtonUI(0, 300, 115, 24,-4.1f, 1.25f, "Save world",  "button_blue", ButtonSaveWorld);
    clearWorldButton = Engine.CreateButtonUI(0, 340, 115, 24,-4.4f, 1.25f, "Clear world", "button_blue", ButtonClearWorld);
    quitButton       = Engine.CreateButtonUI(0, 380, 115, 24,-5.4f, 1.25f, "    Quit",    "button_blue", ButtonQuitApplication);
    
    loadWorldButton->panel->canvas.anchorCenterHorz = true;
    saveWorldButton->panel->canvas.anchorCenterHorz = true;
    clearWorldButton->panel->canvas.anchorCenterHorz = true;
    quitButton->panel->canvas.anchorCenterHorz = true;
    
    Text* loadWorldText = loadWorldButton->textObject->GetComponent<Text>();
    Text* saveWorldText = saveWorldButton->textObject->GetComponent<Text>();
    Text* clearWorldText = clearWorldButton->textObject->GetComponent<Text>();
    Text* quitWorldText = quitButton->textObject->GetComponent<Text>();
    
    loadWorldText->canvas.anchorCenterHorz = true;
    saveWorldText->canvas.anchorCenterHorz = true;
    clearWorldText->canvas.anchorCenterHorz = true;
    quitWorldText->canvas.anchorCenterHorz = true;
    
    //MainMenuDisable();
    
    
    
    
    
    
    
    
    
    // Fire emitter test
    
    Emitter* fireEmitter = Particle.CreateEmitter();
    
    fireEmitter->type = EmitterType::Point;
    fireEmitter->position = playerPosition;
    fireEmitter->direction = glm::vec3(0.0f, 0.0f, 0.0f);
    fireEmitter->scale = glm::vec3(0.08f, 0.01f, 0.08f);
    fireEmitter->scaleTo = glm::vec3(1.0008f, 1.0008f, 1.0008f);
    
    fireEmitter->velocity = glm::vec3(0.0f, 0.01f, 0.0f);
    
    fireEmitter->velocityBias = 0.008f;
    
    fireEmitter->width = 0.1f;
    fireEmitter->height = 0.1f;
    
    fireEmitter->angle = 0.18f;
    fireEmitter->spread = 0.4f;
    
    fireEmitter->colorBegin = Colors.red;
    fireEmitter->colorEnd = Colors.yellow;
    
    fireEmitter->maxParticles = 20;
    fireEmitter->spawnRate = 20;
    
    fireEmitter->heightMinimum = GameWorld.world.waterLevel;
    
    Material* fireEmitterMaterial = fireEmitter->GetMaterial();
    
    //fireEmitterMaterial->EnableBlending();
    //fireEmitterMaterial->shader = Engine.shaders.water;
    
    
    
    
    
    
    // Smoke emitter test
    
    Emitter* smokeEmitter = Particle.CreateEmitter();
    
    smokeEmitter->type = EmitterType::Point;
    
    smokeEmitter->position = playerPosition;
    playerPosition.y += 1.3f;
    
    smokeEmitter->direction = glm::vec3(0.0f, 0.0f, 0.0f);
    smokeEmitter->scale = glm::vec3(0.08f, 0.03f, 0.08f);
    smokeEmitter->scaleTo = glm::vec3(1.000f, 1.000f, 1.000f);
    
    smokeEmitter->velocity = glm::vec3(0.0f, 0.01f, 0.0f);
    
    smokeEmitter->velocityBias = 0.02f;
    
    smokeEmitter->width = 4;
    smokeEmitter->height = 8;
    
    smokeEmitter->angle = 0.3f;
    smokeEmitter->spread = 0.5f;
    
    smokeEmitter->colorBegin = Colors.dkgray;
    smokeEmitter->colorEnd = Colors.gray;
    
    smokeEmitter->maxParticles = 2000;
    smokeEmitter->spawnRate = 1;
    
    smokeEmitter->heightMinimum = GameWorld.world.waterLevel;
    
    Material* smokeEmitterMaterial = smokeEmitter->GetMaterial();
    
    //smokeEmitterMaterial->EnableBlending();
    //smokeEmitterMaterial->shader = Engine.shaders.water;
    
    return;
}

