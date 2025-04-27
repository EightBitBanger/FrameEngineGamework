#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>

#include <vector>
#include <cmath>
#include <cstdint>
#include <vector>
#include <cmath>
#include <cstdint>
#include <random>

// Simple smooth interpolated noise (not Perlin, but deterministic)
float ValueNoise1D(int x, uint32_t seed) {
    std::mt19937 rng(seed + x);
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    return dist(rng);
}

float SmoothNoise(float x, uint32_t seed) {
    int xi = static_cast<int>(x);
    float frac = x - xi;
    
    float a = ValueNoise1D(xi, seed);
    float b = ValueNoise1D(xi + 1, seed);
    
    // Smooth step interpolation
    float t = frac * frac * (3.0f - 2.0f * frac);
    return a * (1.0f - t) + b * t;
}

std::vector<int32_t> GenerateProceduralAudio(uint32_t seed, double durationSeconds, double baseFreq, double sweepRange, float noiseScale, int sampleRate = 44100, int16_t volume = 24000) {
    int totalSamples = static_cast<int>(durationSeconds * sampleRate);
    std::vector<int32_t> samples(totalSamples);
    
    // Parameters for noise sampling
    float zOffset = seed * 0.137f;  // Seed variation via z-axis
    
    for (int i = 0; i < totalSamples; ++i) {
        double t = static_cast<double>(i) / sampleRate;
        
        // Use Perlin noise to modulate frequency
        float x = t * noiseScale;
        float noiseVal = Random.Perlin(x, 0.0f, zOffset, seed);
        noiseVal = (noiseVal + 1.0f) * 0.5f; // remap to range [0 - 1]
        
        double freq = baseFreq + sweepRange * noiseVal;
        double angle = 2.0f * glm::pi<float>() * freq * t;
        
        samples[i] = static_cast<int32_t>(volume * sin(angle));
    }
    
    return samples;
}


struct AudioGene {
    float basePitch;       // in Hz
    float pitchVariation;  // range of modulation
    float noiseSpeed;      // how fast it warbles
    float growliness;      // affects waveform shape or FM modulation
    float intensity;       // growl intensity
    float breathiness;     // adds filtered noise
    float duration;        // length of call
    float seed;            // noise seed
};

std::vector<int32_t> GenerateCreatureVoice(const AudioGene& gene, int sampleRate = 44100, int16_t volume = 24000) {
    std::vector<int32_t> samples = GenerateProceduralAudio(gene.seed, gene.duration, gene.basePitch, gene.pitchVariation, gene.noiseSpeed, sampleRate, volume);
    
    float growlRateHz = gene.growliness;
    float intensity = gene.intensity;
    
    for (size_t i = 0; i < samples.size(); ++i) {
        
        float slice = static_cast<double>(i) / sampleRate;
        
        float mod = (1.0f - intensity) + intensity * glm::sin(2.0f * glm::pi<float>() * growlRateHz * slice);
        
        samples[i] = static_cast<int16_t>(samples[i] * mod);
    }
    
    
    /*
    int totalSamples = static_cast<int>(gene.duration * sampleRate);
    for (int i = 0; i < totalSamples; ++i) {
        double t = static_cast<double>(i) / sampleRate;
        
        float noise = Random.Perlin(t * noiseScale, 0.0f, z, gene.seed);
        float pitchMod = (noise + 1.0f) * 0.5f * gene.pitchVariation;
        
        double freq = gene.basePitch + pitchMod;
        double angle = 2.0 * 3.14159f * freq * t;
        
        // Optional: add growliness or breathiness
        double wave = sin(angle);
        if (gene.growliness > 0.0f)
            wave *= sin(angle * gene.growliness); // crude FM modulation
        if (gene.breathiness > 0.0f)
            wave += gene.breathiness * ((rand() % 65536) / 32768.0 - 1.0);
        
        // Clamp
        //if (wave > 1.0) wave = 1.0;
        //if (wave < -1.0) wave = -1.0;
        
        samples[i] = static_cast<int16_t>(wave);
    }
    */
    
    return samples;
}




void Start() {
    
    // Initiate the command console and 
    // load the required command functions
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
    
    
    // Initiate platform state
    Platform.ShowMouseCursor();
    //Engine.DisableConsoleCloseOnReturn();
    
    Platform.isPaused = true;
    //Engine.EnableConsole();
    //Engine.EnableConsoleFadeOutTextElements();
    
    
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
    /*
    Sound* soundA = Audio.CreateSound();
    Sound* soundB = Audio.CreateSound();
    Sound* soundC = Audio.CreateSound();
    AudioSample* sampleA = Audio.CreateAudioSample();
    AudioSample* sampleB = Audio.CreateAudioSample();
    AudioSample* sampleC = Audio.CreateAudioSample();
    
    sampleA->sample_rate = 44100;
    sampleB->sample_rate = 44100;
    sampleC->sample_rate = 44100;
    
    Audio.Presets.RenderWhiteNoise(sampleA, 5.0f);
    Audio.Presets.RenderSweepingSineWave(sampleB, 800, 500, 5.0f);
    Audio.Presets.RenderSweepingSineWave(sampleC, 500, 800, 5.0f);
    
    soundA->sample = sampleA;
    soundB->sample = sampleB;
    soundC->sample = sampleC;
    soundA->SetVolume(0.1f);
    soundB->SetVolume(0.4f);
    soundC->SetVolume(0.4f);
    
    Audio.Play(soundA);
    Audio.Play(soundB);
    Audio.Play(soundC);
    */
    
    
    
    /*
    AudioGene voice;
    voice.basePitch       = 380;
    //voice.breathiness     = 0.1f;
    voice.duration        = 0.6f;
    voice.growliness      = 140.0f;   // Modulation rate
    voice.intensity       = 0.1f;     // Modulation multiplier
    voice.noiseSpeed      = 1.3f;
    voice.pitchVariation  = 80;
    voice.seed            = 68484;
    
    AudioSample* sample = Audio.CreateAudioSample();
    sample->sampleBuffer = GenerateCreatureVoice(voice);
    */
    
    
    /*
    uint32_t seed    = 777;
    double duration  = 1.0d;
    double base      = 200;
    double sweep     = 100;
    float mul = 8.0f;
    
    sampleA->sampleBuffer = GenerateProceduralAudio(seed, duration, base, sweep, mul, 48000, 24000);
    */
    
    
    
    
    
    //
    // Create a camera controller
    //
    
    // The position of the player in the world.
    Vector3 playerPosition;
    playerPosition.x = Random.Range(-10, 10);
    playerPosition.y = Random.Range(-10, 10);
    playerPosition.z = Random.Range(-10, 10);
    
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
    
    DecorationSpecifier decorGrassBase;
    decorGrassBase.type = DECORATION_GRASS;
    decorGrassBase.density = 2000;
    decorGrassBase.spawnHeightMaximum = 35;
    decorGrassBase.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrassBase.spawnStackHeightMin = 1;
    decorGrassBase.spawnStackHeightMax = 2;
    decorGrassBase.threshold = 0.0f;
    decorGrassBase.noise = 0.04f;
    
    DecorationSpecifier decorGrass;
    decorGrass.type = DECORATION_GRASS;
    decorGrass.density = 2000;
    decorGrass.spawnHeightMaximum = 35;
    decorGrass.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrass.spawnStackHeightMin = 1;
    decorGrass.spawnStackHeightMax = 3;
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
    decorSheep.density = 10;
    decorSheep.spawnHeightMaximum = 10;
    decorSheep.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorSheep.threshold = 0.1f;
    decorSheep.noise = 0.4f;
    
    DecorationSpecifier decorHorse;
    decorHorse.type = DECORATION_ACTOR;
    decorHorse.name = "Horse";
    decorHorse.density = 4;
    decorHorse.spawnHeightMaximum = 10;
    decorHorse.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorHorse.threshold = 0.1f;
    decorHorse.noise = 0.4f;
    
    DecorationSpecifier decorBovine;
    decorBovine.type = DECORATION_ACTOR;
    decorBovine.name = "Bovine";
    decorBovine.density = 4;
    decorBovine.spawnHeightMaximum = 10;
    decorBovine.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorBovine.threshold = 0.1f;
    decorBovine.noise = 0.4f;
    
    DecorationSpecifier decorBear;
    decorBear.type = DECORATION_ACTOR;
    decorBear.name = "Bear";
    decorBear.density = 4;
    decorBear.spawnHeightMaximum = 40;
    decorBear.spawnHeightMinimum = 5;
    decorBear.threshold = 0.1f;
    decorBear.noise = 0.4f;
    
    GameWorld.world.mDecorations.push_back(decorGrass);
    GameWorld.world.mDecorations.push_back(decorGrassBase);
    
    GameWorld.world.mDecorations.push_back(decorTrees);
    GameWorld.world.mDecorations.push_back(decorTreeHights);
    
    GameWorld.world.mDecorations.push_back(decorWaterPlants);
    
    GameWorld.world.mDecorations.push_back(decorSheep);
    GameWorld.world.mDecorations.push_back(decorBovine);
    GameWorld.world.mDecorations.push_back(decorHorse);
    GameWorld.world.mDecorations.push_back(decorBear);
    
    
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
    
    GameWorld.renderDistance = 16;
    GameWorld.staticDistance = GameWorld.renderDistance * 0.7f;
    GameWorld.actorDistance  = GameWorld.renderDistance * 0.8f;
    
    GameWorld.LoadWorld();
    
    
    
    
    
    //Text* text = UI.CreateText();
    //text->x = 0;
    //text->y = 0;
    //text->text = "test string of text";
    
    //Panel* panel = UI.CreatePanel();
    //panel->x = 10;
    //panel->y = 10;
    //panel->w = 100;
    //panel->h = 100;
    
    
    
    
    
    
    
    return;
    
    // Initiate UI
    
    /*
    
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
    */
    
    
    
    
    
    
    
    
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

