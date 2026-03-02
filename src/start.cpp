#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

GameObject* CreateCameraController(glm::vec3 position) {
    GameObject* cameraController = Engine.Create<GameObject>();
    cameraController->AddComponent(Engine.CreateComponent<Camera>());
    cameraController->AddComponent(Engine.CreateComponent<rp3d::RigidBody>());
    
    rp3d::RigidBody* rigidBody = cameraController->GetComponent<RigidBody>();
    
    rp3d::Vector3 bodyPosition(position.x, position.y, position.z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    
    rp3d::Transform bodyTransform(bodyPosition, quat);
    rigidBody->setTransform(bodyTransform);
    
    cameraController->SetAngularAxisLockFactor(0, 0, 0);
    cameraController->SetPosition(position);
    
    return cameraController;
}

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
    std::string version = "0.1.0";
    GameWorld.version = version;
    //Inventory.version = version;
    
    // User plug-in initiation
    GameWorld.Initiate();
    Weather.Initiate();
    Particle.Initiate();
    //Inventory.Initiate(8, Platform.windowArea.w, Platform.windowArea.h);
    
    // Create a camera controller
    
    // Create a new camera controller object and 
    // assign it as the engines main camera.
    Engine.cameraController = CreateCameraController(glm::vec3(0, 10, 0));
    rp3d::RigidBody* rigidBody = Engine.cameraController->GetComponent<RigidBody>();
    
    // Assign the camera controller's camera for rendering the main scene.
    Engine.sceneMain->camera = Engine.cameraController->GetComponent<Camera>();
    
    // Disable mouse look on startup.
    Engine.sceneMain->camera->DisableMouseLook();
    rigidBody->enableGravity(false);
    
    // Create a box collider for collisions.
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(glm::vec3(1, 1, 1));
    rp3d::Collider* collider = Engine.cameraController->AddColliderBox(boxShape, 0, 0, 0);
    collider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
    
    rigidBody->updateMassPropertiesFromColliders();
    Engine.cameraController->SetLinearDamping(2.4f);
    
    
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
    GameWorld.renderDistance = 7;
    GameWorld.staticDistance = 0.8f;
    GameWorld.actorDistance  = 0.4f;
    
    GameWorld.world.snowCapHeight = 20.0f;
    GameWorld.world.snowCapBias = 8.0f;
    GameWorld.world.waterLevel = -8.0f;
    
    // Physics
    
    Physics.SetWorldGravity(0.0f, -9.81f, 0.0f);
    //Physics.world->enableDisableJoints
    
    // Load world
    if (!GameWorld.LoadWorld()) {
        
        // Generate a new world
        GameWorld.worldSeed = Random.Range(100, 10000000) - Random.Range(100, 10000000);
        Weather.SetTime(9000);
        Weather.SetWeather(WeatherType::Clear);
        GameWorld.SaveWorld();
    }
}

