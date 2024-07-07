// User application code space
//

#include <GameEngineFramework/Engine/EngineSystems.h>


// User plug-ins

#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
ChunkManager chunkManager;


// User globals
GameObject*  cameraController;




// Day night cycle
GameObject* lightObject;
Light* directionalLight;

bool cycleDirection = false;

float ambientLight = 0.9;

Material* skyMaterial;

Material* plainMaterial;








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
    // Directional light
    //
    
    // Create a new game object.
    lightObject = Engine.Create<GameObject>();
    
    // Add a new light component to the game object.
    lightObject->AddComponent( Engine.CreateComponent<Light>() );
    
    // Point the game object downward. This will cause 
    // the light component to point toward the ground.
    lightObject->GetComponent<Transform>()->RotateAxis(1, Vector3(0.0, -1.0, 0.0));
    
    // Get a pointer to the newly created light component.
    directionalLight = lightObject->GetComponent<Light>();
    
    // Setup the parameters for a directional light
    directionalLight->type       = LIGHT_TYPE_DIRECTIONAL;
    directionalLight->intensity  = 0.7;
    directionalLight->color      = Colors.white;
    
    // Add the light to the main scene.
    Engine.sceneMain->AddLightToSceneRoot( directionalLight );
    
    
    //
    // Create a sky
    //
    
    // Sky colors from hight to low.
    Color skyHigh(Colors.blue);
    Color skyLow(Colors.blue);
    
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
    skyMaterial->diffuse = Colors.white;
    skyMaterial->ambient = Colors.white;
    
    
    //
    // Chunk generation
    //
    
    chunkManager.generationDistance  = 700;
    chunkManager.destructionDistance = 1500;
    
    chunkManager.renderDistance = 15;
    
    chunkManager.doUpdateWithPlayerPosition = true;
    
    chunkManager.chunkSize = 100;
    
    
    chunkManager.world.snowCapHeight = 70;
    
    
    // World generation
    
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
    chunkManager.world.staticPerChunk = 400;
    chunkManager.world.actorsPerChunk = 10;
    
    
    // Chunk material
    Material* chunkMaterial = Engine.Create<Material>();
    
    chunkMaterial->DisableCulling();
    chunkMaterial->isShared = true;
    
    chunkMaterial->shader = Engine.shaders.color;
    
    chunkMaterial->ambient = Colors.Make(0.27, 0.27, 0.27);
    chunkMaterial->diffuse = Colors.white;
    chunkMaterial->specular = Colors.white;
    
    chunkManager.SetMaterial( chunkMaterial );
    
    plainMaterial = chunkMaterial;
    
    
    
    
    return;
}









//
// Application loop
//

glm::vec3 force(0);


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
    
    float lightingMax       = 0.87;
    float lightingMin       = 0.087;
    
    float skyLightingMax    = 0.87;
    float skyLightingMin    = 0.0034;
    
    float worldLightingMax  = 0.87;
    float worldLightingMin  = 0.34;
    
    
    // Light direction
    Transform* lightTransform = lightObject->GetComponent<Transform>();
    
    
    if (!Platform.isPaused) {
        
        if (Input.CheckKeyCurrent(VK_I)) {ambientLight += 0.01f;}
        if (Input.CheckKeyCurrent(VK_K)) {ambientLight -= 0.01f;}
        
        if (Input.CheckKeyCurrent(VK_T)) {lightTransform->RotateAxis( 0.1, Vector3(1, 0, 0));}
        if (Input.CheckKeyCurrent(VK_G)) {lightTransform->RotateAxis(-0.1, Vector3(1, 0, 0));}
        
    }
    
    
    // Ambient limits
    if (ambientLight > 0.87f) ambientLight = 0.87f;
    if (ambientLight < 0.0f)  ambientLight = 0.0f;
    
    // Sky brightness
    skyMaterial->ambient = Math.Lerp(skyLightingMin, skyLightingMax, ambientLight);
    
    // World brightness
    plainMaterial->diffuse = Math.Lerp(worldLightingMin, worldLightingMax, ambientLight);
    
    
    //Light* sunLight = directionalLight->GetComponent<Light>();
    //sunLight->intensity = Math.Lerp(lightingMin, lightingMax, ambientLight);;
    
    
    
    
    
    
    
    
    
    
    //
    // Map generation
    //
    
    chunkManager.Update();
    
    
    
    
    //
    // Camera controller movement
    //
    
    if (Engine.cameraController == nullptr) 
        return;
    
    float forceAccelerate = 0.004;
    float forceDecelerate = 0.015;
    float forceMax        = 20;
    
    if (mainCamera != nullptr) {
        
        // No movement when paused
        bool moving = false;
        if (!Platform.isPaused) {
            
            // WASD Directional
            if (Input.CheckKeyCurrent(VK_W)) {force += mainCamera->forward; moving = true;}
            if (Input.CheckKeyCurrent(VK_S)) {force -= mainCamera->forward; moving = true;}
            if (Input.CheckKeyCurrent(VK_A)) {force += mainCamera->right; moving = true;}
            if (Input.CheckKeyCurrent(VK_D)) {force -= mainCamera->right; moving = true;}
            
            // Space/Shift Elevation
            if (Input.CheckKeyCurrent(VK_SPACE)) {force += mainCamera->up; moving = true;}
            if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= mainCamera->up; moving = true;}
        }
        
        // Accumulate force
        if (glm::length(force) < forceMax) 
            force += force * forceAccelerate;
        
        // Decelerate
        if ( glm::length(force) >  0.0001) force -= (force * forceDecelerate);
        if (-glm::length(force) < -0.0001) force -= (force * forceDecelerate);
        
        // Apply the force to the camera controller
        glm::vec3 forceMul = force * glm::vec3(0.01);
        
        if (forceMul != glm::vec3(0)) 
            Engine.cameraController->AddForce(forceMul.x, forceMul.y, forceMul.z);
        
        
        
        // Minimum speed cut off
        //if (glm::length(force) < 0.001f) 
        //    force = glm::vec3(0.0f);
        
        // Max force
        //if ( glm::length(forceMul) >  forceMax) forceMul = glm::vec3(forceMax);
        //if (-glm::length(forceMul) < -forceMax) forceMul = glm::vec3(-forceMax);
        
        
        
        
        //
        // Update camera height
        //
        
        /*
        RigidBody* rigidBody = Engine.cameraController->GetComponent<RigidBody>();
        rp3d::Transform bodyTransform = rigidBody->getTransform();
        
        rp3d::Vector3 position = bodyTransform.getPosition();
        
        //position.y = z + 1;
        
        bodyTransform.setPosition(position);
        
        rigidBody->setTransform( bodyTransform );
        */
        
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




