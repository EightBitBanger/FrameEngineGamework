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
    
    /*
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
    */
    
    
    // Initiate platform
    //Platform.EnableVSync();
    Platform.ShowMouseCursor();
    Platform.isPaused = true;
    
    //Engine.EnablePhysicsDebugRenderer();
    
    // Event callback
    Platform.EventCallbackLoseFocus = EventLostFocus;
    Platform.EventCallbackResize = EventOnResize;
    
    // Client version
    std::string version = "0.0.1";
    
    // User plug-in initiation
    Weather.Initiate();
    Particle.Initiate();
    
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
    Material* worldMaterial  = Engine.Create<Material>();
    Material* staticMaterial = Engine.Create<Material>();
    Material* waterMaterial  = Engine.Create<Material>();
    
    
    Weather.SetPlayerObject(Engine.cameraController);
    Weather.SetWorldMaterial(worldMaterial);
    Weather.SetStaticMaterial(staticMaterial);
    Weather.SetWaterMaterial(waterMaterial);
    
    // Lighting levels
    Weather.SetWorldLightLevel(0.0087f, 0.3f);
    
    // Load world
    Resources.LoadWaveFront("data/models/zone0.obj",    "zone0", true);
    Resources.LoadTexture("data/textures/grass.png",    "grass", true);
    Resources.LoadTexture("data/textures/dirt.png",     "dirt", true);
    Resources.LoadTexture("data/textures/cliff.png",    "cliff", true);
    Resources.LoadTexture("data/textures/bias.png",     "bias", true);
    
    GameObject* gameObject = Engine.Create<GameObject>();
    
    gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
    MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(meshRenderer);
    
    meshRenderer->mesh     = Engine.Create<Mesh>();
    meshRenderer->material = Engine.Create<Material>();
    
    Resources.LoadMeshFromTag("zone0", meshRenderer->mesh);
    meshRenderer->mesh->CalculateNormals();
    
    Resources.LoadMaterialFromTag("grass", meshRenderer->material, MATERIAL_FILTER_TRILINEAR, MATERIAL_FILTER_TRILINEAR);
    Resources.LoadMaterialFromTag("dirt", meshRenderer->material, MATERIAL_FILTER_TRILINEAR, MATERIAL_FILTER_TRILINEAR);
    Resources.LoadMaterialFromTag("bias", meshRenderer->material, MATERIAL_FILTER_TRILINEAR, MATERIAL_FILTER_TRILINEAR);
    
    meshRenderer->material->ambient = Colors.black;
    meshRenderer->material->diffuse = Colors.white;
    meshRenderer->material->shader = Engine.shaders.texture;
    
    
    
    
    // Generate a new world
    Weather.SetTime(9000);
    Weather.SetWeather(WeatherType::Clear);
    
}








