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





// Key binding callbacks

bool isFullScreen = false;
void keyBindFullscreen(void) {
    isFullScreen = !isFullScreen;
    if (isFullScreen) {
        Platform.WindowEnableFullscreen();
    } else {
        Platform.WindowDisableFullscreen();
    }
}

void keyBindEscape(void) {
    Platform.Pause();
    if (Platform.isPaused == true) {
        MainMenuEnable();
        return;
    }
    MainMenuDisable();
}

void keyBindF4() {
    extern bool isProfilerEnabled;
    isProfilerEnabled = !isProfilerEnabled;
    
    if (!isProfilerEnabled) 
        for (unsigned int i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) 
            Engine.console.WriteDialog(i, "");
    for (unsigned int i=0; i < 20; i++) 
        Engine.console.WriteDialog(i, "");
}



void Start() {
    
    // Key bindings
    Input.BindKeyPressToFunction(VK_F4, keyBindF4);
    Input.BindKeyPressToFunction(VK_F11, keyBindFullscreen);
    Input.BindKeyPressToFunction(VK_ESCAPE, keyBindEscape);
    
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
    
    // Initial player position in the world.
    Vector3 playerPosition;
    playerPosition.x = Random.Range(-120, 120);
    playerPosition.y = Random.Range(-120, 120);
    playerPosition.z = Random.Range(-120, 120);
    
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
    // Perlin layers
    
    // Mountain base ripple formation
    Perlin perlinMountainA;
    perlinMountainA.heightMultuplier = 20;
    perlinMountainA.heightThreshold = 10.0f;
    perlinMountainA.noiseWidth  = 0.02;
    perlinMountainA.noiseHeight = 0.02;
    
    // Macro scale world formation
    Perlin perlinMountainB;
    perlinMountainB.heightMultuplier = 20;
    perlinMountainB.heightThreshold = 20.0f;
    perlinMountainB.noiseWidth  = 0.014;
    perlinMountainB.noiseHeight = 0.014;
    perlinMountainB.offsetX = 100.0f;
    perlinMountainB.offsetY = 100.0f;

    // Macro scale world formation
    Perlin perlinMountainC;
    perlinMountainC.heightMultuplier = 100;
    perlinMountainC.heightThreshold = 20.0f;
    perlinMountainC.noiseWidth  = 0.014;
    perlinMountainC.noiseHeight = 0.014;
    perlinMountainC.offsetX = 100.0f;
    perlinMountainC.offsetY = 100.0f;
    
    // Low level ripple layers
    Perlin perlinLayerA;
    perlinLayerA.heightMultuplier = 20;
    perlinLayerA.heightThreshold = 1.0f;
    perlinLayerA.noiseWidth  = 0.02;
    perlinLayerA.noiseHeight = 0.02;
    
    Perlin perlinLayerB;
    perlinLayerB.heightMultuplier = 1.7;
    //perlinLayerB.heightThreshold = 0.1f;
    perlinLayerB.noiseWidth  = 0.1;
    perlinLayerB.noiseHeight = 0.1;
    
    // Flat world base layer
    Perlin perlinFlatland;
    perlinFlatland.heightMultuplier = 0.4;
    perlinFlatland.noiseWidth  = 0.5;
    perlinFlatland.noiseHeight = 0.5;
    
    //GameWorld.world.mPerlin.push_back(perlinMountainA);
    //GameWorld.world.mPerlin.push_back(perlinMountainB);
    //GameWorld.world.mPerlin.push_back(perlinMountainC);
    GameWorld.world.mPerlin.push_back(perlinLayerA);
    GameWorld.world.mPerlin.push_back(perlinLayerB);
    GameWorld.world.mPerlin.push_back(perlinFlatland);
    
    
    //
    // Biomes
    Color biomeColor;
    
    // Desert
    Biome biomeLayerDesert;
    biomeColor = Colors.Lerp(Colors.yellow, Colors.red, 0.3f) * 0.5f;
    biomeLayerDesert.color = glm::vec3(biomeColor.r, biomeColor.g, biomeColor.b);
    biomeLayerDesert.offsetX = 1000;
    biomeLayerDesert.offsetZ = 1000;
    biomeLayerDesert.noiseWidth  = 0.001f;
    biomeLayerDesert.noiseHeight = 0.001f;
    
    // Tropical
    Biome biomeLayerTropical;
    biomeColor = Colors.green * 0.4f;
    biomeLayerTropical.color = glm::vec3(biomeColor.r, biomeColor.g, biomeColor.b);
    biomeLayerTropical.offsetX = 7600;
    biomeLayerTropical.offsetZ = 7600;
    biomeLayerTropical.noiseWidth  = 0.001f;
    biomeLayerTropical.noiseHeight = 0.001f;
    
    
    
    GameWorld.world.mBiomes.push_back(biomeLayerDesert);
    GameWorld.world.mBiomes.push_back(biomeLayerTropical);
    
    
    //
    // Decoration generation
    
    DecorationSpecifier decorGrass;
    decorGrass.type = DECORATION_GRASS;
    decorGrass.density = 1000;
    decorGrass.spawnHeightMaximum = 10;
    decorGrass.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrass.spawnStackHeightMin = 1;
    decorGrass.spawnStackHeightMax = 8;
    decorGrass.threshold = 0.0f;
    decorGrass.noise = 0.5f;
    
    DecorationSpecifier decorTrees;
    decorTrees.type = DECORATION_TREE;
    decorTrees.density = 10;
    decorTrees.spawnHeightMaximum = 10;
    decorTrees.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorTrees.spawnStackHeightMin = 4;
    decorTrees.spawnStackHeightMax = 24;
    decorTrees.threshold = -0.08f;
    decorTrees.noise     = 0.001f;
    
    DecorationSpecifier decorStructure;
    decorStructure.type = DECORATION_STRUCTURE;
    decorStructure.density = 3;
    decorStructure.spawnHeightMaximum = 20;
    decorStructure.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorStructure.spawnStackHeightMax = 4;
    decorStructure.spawnStackHeightMin = 2;
    decorStructure.threshold = 0.02f;
    decorStructure.noise     = 0.087f;
    
    
    GameWorld.world.mDecorations.push_back(decorGrass);
    GameWorld.world.mDecorations.push_back(decorTrees);
    GameWorld.world.mDecorations.push_back(decorStructure);
    
    
    
    //
    // Actors
    
    DecorationSpecifier decorSheep;
    decorSheep.type = DECORATION_ACTOR;
    decorSheep.name = "Sheep";
    decorSheep.density = 20;
    decorSheep.spawnHeightMaximum = 10;
    decorSheep.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorSheep.threshold = 0.0f;
    decorSheep.noise     = 0.3f;
    
    DecorationSpecifier decorHorse;
    decorHorse.type = DECORATION_ACTOR;
    decorHorse.name = "Horse";
    decorHorse.density = 2;
    decorHorse.spawnHeightMaximum = 10;
    decorHorse.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorHorse.threshold = 0.0f;
    decorHorse.noise     = 0.5;
    
    DecorationSpecifier decorBovine;
    decorBovine.type = DECORATION_ACTOR;
    decorBovine.name = "Bovine";
    decorBovine.density = 2;
    decorBovine.spawnHeightMaximum = 10;
    decorBovine.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorBovine.threshold = 0.0f;
    decorBovine.noise     = 0.5f;
    
    DecorationSpecifier decorBear;
    decorBear.type = DECORATION_ACTOR;
    decorBear.name = "Bear";
    decorBear.density = 1;
    decorBear.spawnHeightMaximum = 50;
    decorBear.spawnHeightMinimum = 10;
    decorBear.threshold = 0.0f;
    decorBear.noise     = 0.5f;
    
    DecorationSpecifier decorDog;
    decorDog.type = DECORATION_ACTOR;
    decorDog.name = "Dog";
    decorDog.density = 1;
    decorDog.spawnHeightMaximum = 50;
    decorDog.spawnHeightMinimum = 5;
    decorDog.threshold = 0.0f;
    decorDog.noise     = 0.5f;
    
    GameWorld.world.mDecorations.push_back(decorSheep);
    GameWorld.world.mDecorations.push_back(decorBovine);
    GameWorld.world.mDecorations.push_back(decorHorse);
    GameWorld.world.mDecorations.push_back(decorBear);
    GameWorld.world.mDecorations.push_back(decorDog);
    
    // Lighting levels
    
    GameWorld.world.chunkColorLow   = Colors.MakeGrayScale(0.3f);
    GameWorld.world.staticColorLow  = Colors.MakeGrayScale(0.3f);
    GameWorld.world.actorColorLow   = Colors.MakeGrayScale(0.02f);
    
    GameWorld.world.chunkColorHigh  = Colors.MakeGrayScale(0.87f);
    GameWorld.world.staticColorHigh = Colors.MakeGrayScale(0.87f);
    GameWorld.world.actorColorHigh  = Colors.MakeGrayScale(0.87f);
    
    GameWorld.world.ambientLight = 0.87f;
    
    
    // World rendering
    GameWorld.chunkSize = 64;
    GameWorld.renderDistance = 8;
    GameWorld.staticDistance = 0.5f;
    GameWorld.actorDistance  = 0.3f;
    
    GameWorld.world.snowCapHeight = 20.0f;
    GameWorld.world.snowCapBias = 8.0f;
    GameWorld.world.waterLevel = -8.0f;
    
    GameWorld.world.chunkColorLow  = Colors.brown * Colors.green * Colors.MakeGrayScale(0.4f);
    GameWorld.world.chunkColorHigh = Colors.brown * Colors.MakeGrayScale(0.2f);
    GameWorld.world.chunkColorBias = 0.087f;
    
    
    // Load world
    if (!GameWorld.LoadWorld()) {
        GameWorld.worldSeed = Random.Range(100, 10000000) - Random.Range(100, 10000000);
        Weather.SetTime(9000);
        Weather.SetWeather(WeatherType::Clear);
        GameWorld.SaveWorld();
    }
    
    return;
    
    
    
    
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

