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
    
    
    Sound* sound = Audio.CreateSound();
    sound->sample = Audio.CreateAudioSample();
    
    VocalParameters param;
    param.seconds        = 0.1;
    param.pitchStartHz   = 2500.0f;
    param.pitchEndHz     = 1000.0f;
    param.attackSeconds  = 0.005f;
    param.holdSeconds    = 0.03f;
    param.releaseSeconds = 0.1f;
    param.brightness     = 0.8f;
    param.growl          = 0.15f;
    param.noiseAmount    = 0.05f;
    
    Audio.Presets.RenderAnimalVocal(sound->sample, param);
    
    Playback* playback = Audio.Play(sound);
    
    
    
    // Initiate platform
    //Platform.EnableVSync();
    Platform.ShowMouseCursor();
    Platform.isPaused = true;
    
    //Engine.EnablePhysicsDebugRenderer();
    
    // Event callback
    Platform.EventCallbackLoseFocus = EventLostFocus;
    Platform.EventCallbackResize = EventOnResize;
    
    // Initiate client version
    std::string version = "0.1.0";
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
    Engine.cameraController->AddColliderBox(boxShape, 0, 0, 0, LayerMask::Ground);
    
    
    // Weather system
    
    Weather.SetPlayerObject(Engine.cameraController);
    Weather.SetWorldMaterial(GameWorld.worldMaterial);
    Weather.SetStaticMaterial(GameWorld.staticMaterial);
    Weather.SetWaterMaterial(GameWorld.waterMaterial);
    
    
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
    if (GameWorld.LoadWorld()) {
        Inventory.LoadFromFile("worlds/" + GameWorld.world.name);
    }
    
    // Generate a new world
    GameWorld.worldSeed = Random.Range(100, 10000000) - Random.Range(100, 10000000);
    Weather.SetTime(9000);
    Weather.SetWeather(WeatherType::Clear);
    GameWorld.SaveWorld();
    
}

