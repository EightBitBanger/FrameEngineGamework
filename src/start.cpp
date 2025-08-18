#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

#include <glm/gtx/euler_angles.hpp>

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

void BuildStack(Structure& structure, glm::vec3 position, glm::vec3 scale, float height, int length, Color color) {
    for (int l=0; l < length; l++) {
        float step = height * static_cast<float>(l);
        glm::vec3 pos(0.0f, step, 0.0f);
        
        StructurePart pole;
        pole.offset   = position + pos;
        pole.rotation = glm::vec3(0);
        pole.scale    = scale;
        pole.color    = color;
        
        structure.parts.push_back(pole);
    }
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
    
    
    // Biome region noise
    
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
    
    DecorationSpecifier decorGrassShort;
    decorGrassShort.type = DecorationType::Grass;
    decorGrassShort.name = "short";
    decorGrassShort.density = 1000;
    decorGrassShort.spawnHeightMaximum = 10;
    decorGrassShort.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrassShort.threshold = 0.0f;
    decorGrassShort.noise = 0.5f;
    
    DecorationSpecifier decorGrassTall;
    decorGrassTall.type = DecorationType::Grass;
    decorGrassTall.name = "tall";
    decorGrassTall.density = 400;
    decorGrassTall.spawnHeightMaximum = 10;
    decorGrassTall.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrassTall.threshold = 0.0f;
    decorGrassTall.noise = 0.5f;
    
    DecorationSpecifier decorGrassDry;
    decorGrassDry.type = DecorationType::Grass;
    decorGrassDry.name = "dry";
    decorGrassDry.density = 300;
    decorGrassDry.spawnHeightMaximum = 10;
    decorGrassDry.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrassDry.threshold = 0.0f;
    decorGrassDry.noise = 0.5f;
    
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
    
    DecorationSpecifier decorGrassDense;
    decorGrassDense.type = DecorationType::Grass;
    decorGrassDense.name = "short";
    decorGrassDense.density = 4000;
    decorGrassDense.spawnHeightMaximum = 10;
    decorGrassDense.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrassDense.threshold = 0.0f;
    decorGrassDense.noise = 0.5f;
    
    DecorationSpecifier decorGrassTallDense;
    decorGrassTallDense.type = DecorationType::Grass;
    decorGrassTallDense.name = "tall";
    decorGrassTallDense.density = 2000;
    decorGrassTallDense.spawnHeightMaximum = 10;
    decorGrassTallDense.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorGrassTallDense.threshold = 0.0f;
    decorGrassTallDense.noise = 0.5f;
    
    biomeLayerPlains.decorations.push_back(decorGrassShort);
    biomeLayerPlains.decorations.push_back(decorGrassTall);
    biomeLayerPlains.decorations.push_back(decorTrees);
    biomeLayerDesert.decorations.push_back(decorGrassDry);
    biomeLayerForest.decorations.push_back(decorTreesDense);
    biomeLayerForest.decorations.push_back(decorGrassDense);
    biomeLayerForest.decorations.push_back(decorGrassTallDense);
    
    
    //
    // Actors
    
    DecorationSpecifier decorSheep;
    decorSheep.type = DecorationType::Actor;
    decorSheep.name = "Sheep";
    decorSheep.density = 40;
    decorSheep.spawnHeightMaximum = 10;
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
    decorBovine.spawnHeightMaximum = 10;
    decorBovine.spawnHeightMinimum = GameWorld.world.waterLevel;
    decorBovine.threshold = 0.0f;
    decorBovine.noise     = 0.5f;
    
    DecorationSpecifier decorBear;
    decorBear.type = DecorationType::Actor;
    decorBear.name = "Bear";
    decorBear.density = 8;
    decorBear.spawnHeightMaximum = 50;
    decorBear.spawnHeightMinimum = 10;
    decorBear.threshold = 0.0f;
    decorBear.noise     = 0.5f;
    
    DecorationSpecifier decorDog;
    decorDog.type = DecorationType::Actor;
    decorDog.name = "Dog";
    decorDog.density = 1;
    decorDog.spawnHeightMaximum = 50;
    decorDog.spawnHeightMinimum = 5;
    decorDog.threshold = 0.0f;
    decorDog.noise     = 0.5f;
    
    DecorationSpecifier decorVillage;
    decorVillage.type = DecorationType::Structure;
    decorVillage.name = "village";
    decorVillage.density = 200;
    decorVillage.noise = 0.08f;
    decorVillage.threshold = 0.0f;
    decorVillage.spawnHeightMinimum = GameWorld.world.waterLevel + 4;
    decorVillage.spawnHeightMaximum = GameWorld.world.waterLevel + 24;
    
    biomeLayerPlains.decorations.push_back(decorSheep);
    biomeLayerPlains.decorations.push_back(decorVillage);
    
    biomeLayerDesert.decorations.push_back(decorBovine);
    
    biomeLayerPlains.decorations.push_back(decorHorse);
    biomeLayerPlains.decorations.push_back(decorBear);
    biomeLayerPlains.decorations.push_back(decorDog);
    
    GameWorld.world.biomes.push_back(biomeLayerPlains);
    GameWorld.world.biomes.push_back(biomeLayerDesert);
    GameWorld.world.biomes.push_back(biomeLayerForest);
    
    
    // Structures
    Structure village;
    village.name = "village";
    village.buildSpread = 25.0f;
    village.actorCountMin = 0;
    village.actorCountMax = 8;
    village.actorSpread = 4.0f;
    
    glm::vec3 stackPos(0, 10, 0);
    
    
    
    float spread = 4.0f;
    float height = 4.0f;
    
    float thickness = 1.0f;
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
            
            GameWorld.build.StackAtAngle(village, position, glm::vec3(thickness, 1, thickness), glm::vec3(0, 90, 0), 1, height, color);
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
        
        StructurePart pole;
        pole.offset = glm::ivec2((int)x + 3, (int)z + 3);
        pole.rotation = rotation;
        pole.scale = glm::vec3(stickThickness, height, stickThickness);
        pole.color = stickColor;
        
        village.parts.push_back(pole);
    }
    
    // Add to world structure list
    GameWorld.world.structures.push_back(village);
    */
    
    
    
    
    
    /*
    Structure village;
    village.name = "village";
    village.buildSpread = 30.0f;
    
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
        
        StructurePart pole;
        pole.offset = glm::ivec2((int)x + 3, (int)z + 3);
        pole.rotation = rotation;
        pole.scale = glm::vec3(stickThickness, height, stickThickness);
        pole.color = stickColor;
        
        village.parts.push_back(pole);
    }
    
    // Add to world structure list
    GameWorld.world.structures.push_back(village);
    */
    
    
    
    /*
    Structure house;
    house.name = "village";
    house.buildSpread = 30.0f;
    
    house.actorCountMin = 0;
    house.actorCountMax = 6;
    house.actorSpread   = 4.0f;
    
    // ---- Tunables ----
    const float width          = 8.0f;   // X span (left-right)
    const float depth          = 10.0f;  // Z span (front-back)
    const float wallHeight     = 6.0f;   // vertical post height
    const float roofPitchDeg   = 35.0f;  // pitch of rafters
    const int   rafterCount    = 5;      // rafters per side (spaced along Z)
    const float stickThickness = 0.6f;   // thickness of each stick
    
    Color postColor   = Colors.brown * 0.28f;
    Color beamColor   = Colors.brown * 0.12f;
    Color rafterColor = Colors.brown * 0.24f;
    
    // helper to push a stick
    auto addStick = [&](glm::ivec2 gridXZ, glm::vec3 rotDeg, glm::vec3 scale, Color col) {
        StructurePart p;
        p.offset   = glm::ivec2(gridXZ.x + 3, gridXZ.y + 3); // keep your +3 offset convention
        p.rotation = rotDeg;                                  // degrees, same convention as your teepee
        p.scale    = scale;                                   // (thickness, length, thickness)
        p.color    = col;
        house.parts.push_back(p);
    };
    
    // Convert world-like float coords to your integer grid offsets (rounding keeps it stable)
    auto toGrid = [](float x, float z) -> glm::ivec2 {
        return glm::ivec2((int)std::round(x), (int)std::round(z));
    };
    
    // Footprint corners (centered around origin, like your ring code)
    const float hx = width * 0.5f;
    const float hz = depth * 0.5f;
    
    // Corner posts (vertical)
    {
        glm::vec3 rot(0.0f, 0.0f, 0.0f); // vertical along local Y, no tilt
        glm::vec3 scl(stickThickness, wallHeight, stickThickness);
    
        addStick(toGrid(-hx, -hz), rot, scl, postColor); // Front-Left
        addStick(toGrid( hx, -hz), rot, scl, postColor); // Front-Right
        addStick(toGrid(-hx,  hz), rot, scl, postColor); // Back-Left
        addStick(toGrid( hx,  hz), rot, scl, postColor); // Back-Right
    }
    
    // Ridge posts (centered along Z at ± around middle to support ridge)
    {
        glm::vec3 rot(0.0f, 0.0f, 0.0f);
        glm::vec3 scl(stickThickness, wallHeight + 1.5f, stickThickness);
    
        addStick(toGrid(0.0f, -hz * 0.6f), rot, scl, postColor);
        addStick(toGrid(0.0f,  hz * 0.6f), rot, scl, postColor);
    }
    
    // Top plates (side beams) — shallow tilt so they visually sit above posts
    // Left plate: runs along +Z and -Z near x = -hx
    // Right plate: near x = +hx
    // Since parts are Y-aligned, we "lay them along Z" via a small X-rotation and Yaw them to 90°.
    {
        // Effective length equals depth; add small fudge so they overlap into posts
        float plateLen = depth + 1.0f;
        // Small upward tilt to imply elevation (you can reduce to ~8–12 to lower)
        float plateTilt = 12.0f;
    
        // Left plate
        {
            glm::ivec2 at = toGrid(-hx, 0.0f);
            // Rotate so the stick lies roughly along Z: rotate X by ~90° to lay it, then yaw 0/180 has little effect here;
            // We add a small positive tilt so it appears higher.
            glm::vec3 rot(90.0f - plateTilt, 0.0f, 0.0f);
            glm::vec3 scl(stickThickness, plateLen, stickThickness);
            addStick(at, rot, scl, beamColor);
        }
        // Right plate
        {
            glm::ivec2 at = toGrid(hx, 0.0f);
            glm::vec3 rot(90.0f - plateTilt, 0.0f, 0.0f);
            glm::vec3 scl(stickThickness, plateLen, stickThickness);
            addStick(at, rot, scl, beamColor);
        }
    }
    
    // Ridge beam — centered at x = 0, runs along Z with a modest tilt so it reads above plates
    {
        float ridgeLen = depth + 1.2f;
        glm::ivec2 at = toGrid(0.0f, 0.0f);
        glm::vec3 rot(90.0f - roofPitchDeg * 0.5f, 0.0f, 0.0f); // laid along Z with slight upward feel
        glm::vec3 scl(stickThickness, ridgeLen, stickThickness);
        addStick(at, rot, scl, beamColor);
    }
    
    // Rafters — pairs from left/right plates up toward ridge, spaced along Z
    {
        // Space rafters evenly along the depth
        for (int i = 0; i < rafterCount; ++i) {
            float t = (rafterCount == 1) ? 0.5f : (float)i / (float)(rafterCount - 1);
            float z = -hz + t * depth;
    
            // Left rafter: base near (-hx, z), tilt up toward ridge (in +X direction)
            {
                // Aim roughly toward center with roof pitch as X-rotation.
                glm::ivec2 at = toGrid(-hx + 0.3f, z); // slight nudge inward so it visually sits on plate
                glm::vec3 rot( -roofPitchDeg,  0.0f,  90.0f ); // rotate around Z to lay along X, then pitch down around X
                glm::vec3 scl(stickThickness, hx + 0.5f, stickThickness); // length spans to ridge
                addStick(at, rot, scl, rafterColor);
            }
    
            // Right rafter: base near (+hx, z), tilt up toward ridge (in -X direction)
            {
                glm::ivec2 at = toGrid( hx - 0.3f, z);
                glm::vec3 rot(  roofPitchDeg, 0.0f, 90.0f ); // mirrored pitch
                glm::vec3 scl(stickThickness, hx + 0.5f, stickThickness);
                addStick(at, rot, scl, rafterColor);
            }
        }
    }
    
    // Front header beam (door lintel feel) — shallow tilt so it reads above ground
    {
        float headerLen = width + 0.8f;
        glm::ivec2 at = toGrid(0.0f, -hz);
        glm::vec3 rot(90.0f - 10.0f, 90.0f, 0.0f); // lay along X with small tilt
        glm::vec3 scl(stickThickness, headerLen, stickThickness);
        addStick(at, rot, scl, beamColor);
    }
    
    // Back header beam
    {
        float headerLen = width + 0.8f;
        glm::ivec2 at = toGrid(0.0f, hz);
        glm::vec3 rot(90.0f - 10.0f, 90.0f, 0.0f);
        glm::vec3 scl(stickThickness, headerLen, stickThickness);
        addStick(at, rot, scl, beamColor);
    }
    
    // Add to world structure list
    GameWorld.world.structures.push_back(house);
    
    
    
    
    /*
    Structure hut;
    hut.name = "village";
    hut.actorCount = 2;
    
    float wallHeight = 10.0f;
    float roofHeight = 2.0f;
    float postThickness = 0.25f;
    float halfExt = 4.0f;
    Color postColor = Colors.brown;
    Color roofColor = Colors.brown * 0.9f;
    
    // Corner posts
    int cornersX[4] = {0, 0, halfExt, halfExt};
    int cornersZ[4] = {0, halfExt, 0, halfExt};
    
    for (int i = 0; i < 4; i++) {
        StructurePart post;
        post.offset = glm::ivec2(cornersX[i], cornersZ[i]);
        post.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        post.scale = glm::vec3(postThickness, wallHeight, postThickness);
        post.color = postColor;
        hut.parts.push_back(post);
    }
    
    StructurePart northWall;
    northWall.offset = glm::ivec2(halfExt / 2.0f, 0);
    northWall.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    northWall.scale = glm::vec3(halfExt * 2.0f, wallHeight, postThickness);
    northWall.color = postColor;
    hut.parts.push_back(northWall);
    
    StructurePart southWall;
    southWall.offset = glm::ivec2((halfExt / 2.0f), halfExt);
    southWall.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    southWall.scale = glm::vec3(halfExt * 2.0f, wallHeight, postThickness);
    southWall.color = postColor;
    hut.parts.push_back(southWall);
    
    
    
    
    // Side walls (centered between corner posts)
    
    StructurePart southWall = northWall;
    southWall.offset = glm::ivec2(2, 4);
    hut.parts.push_back(southWall);
    
    StructurePart westWall;
    westWall.offset = glm::ivec2(0, 2);
    westWall.rotation = glm::vec3(0.0f, 90.0f, 0.0f);
    westWall.scale = glm::vec3(1.5f, wallHeight, postThickness);
    westWall.color = postColor;
    hut.parts.push_back(westWall);
    
    StructurePart eastWall = westWall;
    eastWall.offset = glm::ivec2(4, 2);
    hut.parts.push_back(eastWall);
    
    // Flat roof (center tile + 4 beams around it)
    for (int x = 1; x < 4; x++) {
        for (int z = 1; z < 4; z++) {
            StructurePart roof;
            roof.offset = glm::ivec2(x, z);
            roof.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
            roof.scale = glm::vec3(1.0f, roofHeight, 1.0f);
            roof.color = roofColor;
    
            hut.parts.push_back(roof);
        }
    }
    
    // Optional center pole chimney
    StructurePart centerPost;
    centerPost.offset = glm::ivec2(2, 2);
    centerPost.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    centerPost.scale = glm::vec3(0.2f, wallHeight * 1.2f, 0.2f);
    centerPost.color = postColor * 0.7f;
    hut.parts.push_back(centerPost);
    
    
    // Add to world
    GameWorld.world.structures.push_back(hut);
    */
    
    
    
    
    
    
    
    // Lighting levels
    //Weather.SetWorldLightLevel(0.0087f, 0.3f);
    
    GameWorld.world.staticColorLow  = Colors.MakeGrayScale(0.3f);
    GameWorld.world.actorColorLow   = Colors.MakeGrayScale(0.02f);
    
    GameWorld.world.staticColorHigh = Colors.MakeGrayScale(0.87f);
    GameWorld.world.actorColorHigh  = Colors.MakeGrayScale(0.87f);
    
    // World rendering
    GameWorld.chunkSize = 64;
    GameWorld.renderDistance = 9;
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
    
    return;
}

