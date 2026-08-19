#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

MeshRenderer* boundsRenderer = nullptr;
Panel* selectedTab = nullptr;

std::vector<std::pair<std::string, glm::vec3>> QueryWorld(const glm::vec3& position, float range) {return GameWorld.QueryPickupNames(position, range);}
bool PlaceWorld(const std::string& type, const glm::vec3& position, const glm::vec3& rotation) {return GameWorld.PlacePickupAt(type, position);}
bool RemoveWorld(const glm::vec3& position, std::string& collectedItem) {return GameWorld.RemovePickupAt(position, 0.1f, &collectedItem);}

void Start() {
    // Key bindings
    Input.BindKeyPressToFunction(VK_F4,     keyBindF4);
    Input.BindKeyPressToFunction(VK_F11,    keyBindFullscreen);
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
    
    // Selection bounds renderer
    boundsRenderer = Engine.Create<MeshRenderer>();
    boundsRenderer->mesh = Engine.Create<Mesh>();
    boundsRenderer->material = Engine.Create<Material>();
    
    boundsRenderer->mesh->isShared = false;
    boundsRenderer->mesh->SetPrimitive(MESH_LINE_LOOP);
    boundsRenderer->material->isShared = false;
    boundsRenderer->material->shader = Resources.shaders.colorUnlit;
    
    boundsRenderer->material->ambient = Colors.MakeGrayScale(0.2f);
    
    Engine.sceneMain->AddMeshRendererToSceneRoot(boundsRenderer);
    
    // Selection tab
    /*
    selectedTab = UI.CreatePanel();
    selectedTab->x = 0;
    selectedTab->y = 0;
    selectedTab->w = 200;
    selectedTab->h = 32;
    */
    
    
    // Initiate platform
    //Platform.EnableVSync();
    Platform.ShowMouseCursor();
    Platform.isPaused = true;
    
    // Event callback
    Platform.EventCallbackLoseFocus = EventLostFocus;
    Platform.EventCallbackResize = EventOnResize;
    
    // Initiate client version
    std::string version = "0.1.0";
    GameWorld.version = version;
    //Inventory.version = version;
    
    // User plug-in initiation
    GameWorld.Initiate();
    Weather.Initiate();
    Particle.Initiate();
    //Inventory.Initiate(8, Platform.windowArea.w, Platform.windowArea.h);
    
    // Create a camera controller
    
    CreateCameraController(glm::vec3(0, 10, 0));
    
    // Disable mouse look on startup.
    Engine.sceneMain->camera->DisableMouseLook();
    
    
    // Weather system
    
    Weather.SetPlayerObject(Engine.cameraController);
    Weather.SetWorldMaterial(GameWorld.worldMaterial);
    Weather.SetStaticMaterial(GameWorld.staticMaterial);
    Weather.SetWaterMaterial(GameWorld.waterMaterial);
    
    GameWorld.worldMaterial->ambient = Colors.MakeGrayScale(0.1f);
    GameWorld.staticMaterial->ambient = Colors.MakeGrayScale(0.1f);
    
    GameWorld.worldMaterial->diffuse = Colors.MakeGrayScale(0.3f);
    GameWorld.staticMaterial->diffuse = Colors.MakeGrayScale(0.3f);
    
    // Lighting levels
    Weather.SetWorldLightLevel(0.02f, 0.8f);
    Weather.SetSkyLightLevel(0.01f, 0.2f);
    
    GameWorld.world.ambientLight = 0.0f;
    
    // World rendering
    GameWorld.chunkSize = 64;
    GameWorld.renderDistance = 11;
    GameWorld.staticDistance = 0.6f;
    GameWorld.actorDistance  = 0.3f;
    
    GameWorld.world.snowCapHeight = 20.0f;
    GameWorld.world.snowCapBias = 8.0f;
    GameWorld.world.waterLevel = -8.0f;
    
    // Set the world query callback
    AI.SetWorldRaycastCallback(QueryWorld, PlaceWorld, RemoveWorld);
    
    // Physics
    
    Physics.SetWorldGravity(0.0f, -9.81f, 0.0f);
    //Engine.EnablePhysicsDebugRenderer();
    //AI.DebugRendererEnable();
    
    // Load world
    if (!GameWorld.LoadWorld()) {
        Engine.console.Print("Generating default world");
        
        GameWorld.worldSeed = Random.Range(100, 10000000) - Random.Range(100, 10000000);
        Weather.SetTime(9000);
        Weather.SetWeather(WeatherType::Clear);
        
        GameWorld.SaveWorld();
    } else {
        // Load existing world
        
        //Inventory.LoadFromFile("worlds/" + GameWorld.world.name);
        
    }
    Weather.Update();
    
}

