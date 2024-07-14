// User application code space
//

#include <GameEngineFramework/Engine/EngineSystems.h>


// User plug-ins

#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
ChunkManager chunkManager;

#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>
WeatherSystem Weather;



// User globals
GameObject*  cameraController;




// Day night cycle
bool cycleDirection = false;

float ambientLight = 0.9;

Material* skyMaterial;









// Application entry point
//

void FuncSummon(std::vector<std::string> args) {
    
    for (uint8_t i=0; i < 24; i++) {
        
        unsigned int entityType = 0;
        
        if (args[0] == "sheep") {entityType = 1;}
        
        if (entityType == 0) {
            
            Engine.ConsoleShiftUp("Unknown actor type");
            
            return;
        }
        
        glm::vec3 randomOffset(Random.Range(0, 50) - Random.Range(0, 50), 
                               0, 
                               Random.Range(0, 50) - Random.Range(0, 50));
        
        GameObject* newActorObject = Engine.CreateAIActor( Engine.sceneMain->camera->transform.GetPosition() - randomOffset );
        Actor* newActor = newActorObject->GetComponent<Actor>();
        
        switch (entityType) {
            
            default:
            case 1: AI.genomes.SheepGene( newActor ); break;
            
        }
        
        
        Gene gene;
        gene.attachmentIndex = 1;
        
        gene.position.x = 0;
        gene.position.y = 0.9;
        gene.position.z = 0;
        
        gene.scale.x = 0.35;
        gene.scale.y = 0.5;
        gene.scale.z = 0.4;
        
        newActor->AddGene(gene);
        
        continue;
    }
    
    Engine.ConsoleShiftUp("Actor summoned");
    
    return;
}





void Start() {
    
    
    
    Engine.ConsoleRegisterCommand("summon", FuncSummon);
    
    Platform.HideMouseCursor();
    
    chunkManager.Initiate();
    Weather.Initiate();
    
    
    
    
    //
    // Create a camera controller
    //
    
    //Engine.EnablePhysicsDebugRenderer();
    
    
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
    Engine.cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    
    
    
    //
    // Create a sky
    //
    
    // Amount of fade bias from the color "skyHigh" to "skyLow".
    float colorBias = 1.0f;
    
    // Sky mesh resource name.
    // Note: this mesh is loaded by the resource manager.
    std::string skyResourceName = "sky";
    
    // Generate the sky which will be returned as a game object.
    // This game object will contain a mesh renderer to draw the sky.
    GameObject* skyObject = Engine.CreateSky(skyResourceName, Colors.blue, Colors.blue, colorBias);
    
    // Add the sky's mesh renderer to the main scene.
    Engine.sceneMain->AddMeshRendererToSceneRoot( skyObject->GetComponent<MeshRenderer>(), RENDER_QUEUE_SKY );
    
    // Attach the sky object to the camera controller to prevent 
    // the player from moving outside of the sky.
    skyObject->GetComponent<Transform>()->parent = Engine.cameraController->GetComponent<Transform>();
    
    // Sky rendering colors
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    skyMaterial = skyRenderer->material;
    skyMaterial->diffuse = Colors.dkgray;
    skyMaterial->ambient = Colors.white;
    
    
    
    
    //
    // Chunk generation
    //
    
    chunkManager.chunkSize = 50;
    
    // World generation
    chunkManager.renderDistance = 20;
    
    chunkManager.generationDistance  = chunkManager.renderDistance * chunkManager.chunkSize;
    chunkManager.destructionDistance = chunkManager.generationDistance * 1.5f;
    
    chunkManager.renderDistance = 500;
    
    chunkManager.doUpdateWithPlayerPosition = false;
    
    chunkManager.levelOfDetailDistance = 500;
    
    // Start culling at the chunk size boundary
    Engine.sceneMain->camera->frustumOffset = chunkManager.chunkSize + 20;
    
    float chunkHigh = 0.87f;
    float chunkLow  = 0.1f;
    
    float staticHigh = 0.87f;
    float staticLow  = 0.1f;
    
    float actorHigh = 0.87f;
    float actorLow  = 0.1f;
    
    chunkManager.world.chunkColorHigh = Colors.Make(chunkHigh, chunkHigh, chunkHigh);
    chunkManager.world.chunkColorLow  = Colors.Make(chunkLow, chunkLow, chunkLow);
    
    chunkManager.world.staticColorHigh = Colors.Make(staticHigh, staticHigh, staticHigh);
    chunkManager.world.staticColorLow  = Colors.Make(staticLow, staticLow, staticLow);
    
    chunkManager.world.actorColorHigh = Colors.Make(actorHigh, actorHigh, actorHigh);
    chunkManager.world.actorColorLow  = Colors.Make(actorLow, actorLow, actorLow);
    
    
    // Perlin layers
    
    Perlin perlinBase;
    perlinBase.equation = 0;
    perlinBase.heightMultuplier = 3;
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
    perlinMountainA.heightMultuplier = 80;
    perlinMountainA.noiseWidth  = 0.009;
    perlinMountainA.noiseHeight = 0.009;
    
    Perlin perlinMountainB;
    perlinMountainB.equation = 0;
    perlinMountainB.heightMultuplier = 200;
    perlinMountainB.noiseWidth  = 0.0007;
    perlinMountainB.noiseHeight = 0.0007;
    
    chunkManager.world.AddPerlinLayer(perlinBase);
    chunkManager.world.AddPerlinLayer(perlinLayerA);
    chunkManager.world.AddPerlinLayer(perlinLayerB);
    chunkManager.world.AddPerlinLayer(perlinMountainA);
    chunkManager.world.AddPerlinLayer(perlinMountainB);
    
    
    
    // Actor generation
    chunkManager.world.staticDensity = 8000;
    
    chunkManager.world.treeDensity = 30;
    
    chunkManager.world.actorDensity = 10;
    
    
    return;
}









//
// Application loop
//

glm::vec3 force(0);
float forceDblTime=0;

void Run() {
    
    
    Camera* mainCamera = Engine.sceneMain->camera;
    
    //
    // Pausing
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Platform.Pause();
        
        if (Platform.isPaused) {
            
            Engine.EnableConsole();
            
            mainCamera->DisableMouseLook();
            
            Input.ClearKeys();
            
            Platform.ShowMouseCursor();
            
        } else {
            
            Engine.DisableConsole();
            Engine.ConsoleClearInputString();
            
            mainCamera->EnableMouseLook();
            
            // Reset mouse position
            Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
            
            Platform.HideMouseCursor();
            
            // Reset timers
            Time.Update();
            PhysicsTime.Update();
        }
        
    }
    
    
    //
    // Profiling
    //
    if (Input.CheckKeyPressed(VK_F3)) {
        
        if (Engine.CheckIsProfilerActive()) {
            
            Engine.DisableProfiler();
            
        } else {
            
            Engine.EnableProfiler();
        }
        
    }
    
    
    //
    // Lighting day night cycle experimentation 
    //
    
    float skyLightingMax    = 0.87f;
    float skyLightingMin    = 0.0087f;
    
    float lightingMax       = 10.0f;
    float lightingMin       = 0.0f;
    
    
    
    // Light direction
    Transform* lightTransform = Weather.sunObject->GetComponent<Transform>();
    
    
    if (!Platform.isPaused) {
        
        if (Input.CheckKeyCurrent(VK_I)) {ambientLight += 0.01f;}
        if (Input.CheckKeyCurrent(VK_K)) {ambientLight -= 0.01f;}
        
        if (Input.CheckKeyCurrent(VK_T)) {lightTransform->RotateEuler(0, 0,  0.1);}
        if (Input.CheckKeyCurrent(VK_G)) {lightTransform->RotateEuler(0, 0, -0.1);}
        
    }
    
    
    // Ambient limits
    if (ambientLight > 0.87f) ambientLight = 0.87f;
    if (ambientLight < 0.0f)  ambientLight = 0.0f;
    
    // World brightness
    chunkManager.world.staticColorMul = ambientLight;
    
    // Sky brightness
    skyMaterial->ambient = Math.Lerp(skyLightingMin, skyLightingMax, ambientLight);
    
    // Light brightness
    Weather.sun->intensity = Math.Lerp(lightingMin, lightingMax, ambientLight);
    
    
    
    
    
    
    
    //
    // Update weather system
    //
    
    Weather.Update();
    
    
    //
    // Map generation
    //
    
    chunkManager.Update();
    
    
    
    
    //
    // Camera controller movement
    //
    
    if (Engine.cameraController == nullptr) 
        return;
    
    float forceAccelerate = 0.002f;
    float forceDecelerate = 0.015f;
    
    float forceMax = 0.08f;
    
    
    if (mainCamera != nullptr) {
        
        // No movement when paused
        bool moving = false;
        if (!Platform.isPaused) {
            
            // WASD Directional
            if (Input.CheckKeyCurrent(VK_W)) {force += mainCamera->forward;}
            if (Input.CheckKeyCurrent(VK_S)) {force -= mainCamera->forward;}
            if (Input.CheckKeyCurrent(VK_A)) {force += mainCamera->right;}
            if (Input.CheckKeyCurrent(VK_D)) {force -= mainCamera->right;}
            
            // Space/Shift Elevation
            if (Input.CheckKeyCurrent(VK_SPACE)) {force += mainCamera->up;}
            if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= mainCamera->up;}
        }
        
        // Double speed
        if (Input.CheckKeyCurrent(VK_CONTROL)) forceDblTime += 0.24f;
        
        if (forceDblTime > 1.0f) {forceDblTime -= (forceDecelerate * 8.0f);} else {forceDblTime = 1.0f;}
        
        if (forceDblTime > 3.5f) 
            forceDblTime = 3.5f;
        
        // Accelerate
        glm::vec3 forceTotal = force * forceAccelerate * forceDblTime;
        
        // Decelerate
        if ( glm::length(force) >  0.0001f) force -= (force * forceDecelerate);
        if (-glm::length(force) < -0.0001f) force -= (force * forceDecelerate);
        
        Engine.cameraController->AddForce(forceTotal.x, forceTotal.y, forceTotal.z);
        
        // Field of view effect
        float fovPullback = glm::length(forceTotal) * 80.0f;
        
        if (fovPullback > 10.0f) 
            fovPullback = 10.0f;
        
        Engine.sceneMain->camera->fov = 60 + fovPullback;
        
    }
    
    
    return;
}


//
// Called once every tick (20 frames)
//

void TickUpdate(void) {
    
    return;
}





void Shutdown(void) {
    
    
    
    return;
}




