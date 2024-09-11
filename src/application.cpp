// User application code space
//

#include <GameEngineFramework/Engine/EngineSystems.h>


// User plug-ins

#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
ChunkManager chunkManager;

#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>
WeatherSystem Weather;


// Day night cycle
bool cycleDirection = false;

float ambientLight = 0.9;





void FuncSummon(std::vector<std::string> args) {
    
    for (uint8_t i=0; i < 100; i++) {
        
        unsigned int entityType = 0;
        
        if (args[0] == "Sheep") {entityType = 1;}
        
        if (entityType == 0) {
            
            Engine.Print("Unknown actor type");
            
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
        
        /*
        // Add extra gene
        
        Gene gene;
        gene.attachmentIndex = 3;
        
        gene.position.x = 0;
        gene.position.y = 0.9;
        gene.position.z = 0;
        
        gene.scale.x = 0.35;
        gene.scale.y = 0.5;
        gene.scale.z = 0.4;
        
        newActor->AddGene(gene);
        */
        
        continue;
    }
    
    Engine.Print("Actor summoned");
    
    return;
}




std::vector<Chunk*> activeChunks;


//
// Application entry point
//

void Start() {
    
    Engine.ConsoleRegisterCommand("summon", FuncSummon);
    
    Platform.HideMouseCursor();
    
    chunkManager.Initiate();
    
    Weather.Initiate();
    
    //Engine.EnableProfiler();
    
    //Engine.EnablePhysicsDebugRenderer();
    
    
    //
    // Create a camera controller
    //
    
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
    Engine.cameraController->AddColliderBox(boxShape, 0, 0, 0, LayerMask::Ground);
    
    // Attach the sky object to the camera controller to prevent 
    // the player from moving outside of the sky.
    Weather.skyObject->GetComponent<Transform>()->parent = Engine.cameraController->GetComponent<Transform>();
    
    
    
    
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
    
    chunkManager.AddPerlinNoiseLayer(perlinMountainB);
    chunkManager.AddPerlinNoiseLayer(perlinMountainA);
    chunkManager.AddPerlinNoiseLayer(perlinBase);
    chunkManager.AddPerlinNoiseLayer(perlinLayerA);
    chunkManager.AddPerlinNoiseLayer(perlinLayerB);
    
    chunkManager.world.actorDensity   = 0;
    chunkManager.world.treeDensity    = 30;
    chunkManager.world.staticDensity  = 0;
    
    chunkManager.renderDistance = 8;
    chunkManager.renderDistanceStatic = 30;
    
    chunkManager.world.waterLevel = -21;
    chunkManager.world.waterColor = Colors.blue;
    
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
    
    if (Engine.cameraController == nullptr) 
        return;
    
    
    
    
    
    
    
    
    //
    // Raycast from player
    //
    
    //Engine.WriteDialog(1, Int.ToString( chunkManager.GetNumberOfChunks() ));
    
    
    // Move the player out of the way as we cant cast a ray from inside the collider...
    rp3d::RigidBody* rigidBody = Engine.cameraController->GetComponent<RigidBody>();
    rp3d::Transform bodyTransform = rigidBody->getTransform();
    rp3d::Vector3 bodyPosition = bodyTransform.getPosition();
    bodyPosition.y += 1000;
    
    // Cast forward a ray
    Hit hit;
    float distance = 1000;
    
    glm::vec3 from      = Engine.cameraController->GetComponent<Transform>()->position;
    glm::vec3 direction = glm::vec3(0, -1, 0);
    
    
    
    //
    // Print current chunk details
    //
    
    /*
    
    glm::vec3 cameraPosition = Engine.cameraController->GetPosition();
    Engine.WriteDialog(0, Int.ToString( cameraPosition.x ));
    Engine.WriteDialog(1, Int.ToString( cameraPosition.z ));
    
    //Engine.WriteDialog(2, Int.ToString( chunkManager.playerChunkX ));
    //Engine.WriteDialog(3, Int.ToString( chunkManager.playerChunkZ ));
    
    if (Physics.Raycast(from, direction, distance, hit, LayerMask::Ground)) {
        
        GameObject* hitObject = (GameObject*)hit.gameObject;
        
        Engine.WriteDialog(4, hitObject->name);
        
        Engine.WriteDialog(5, Int.ToString( hitObject->GetPosition().x ));
        Engine.WriteDialog(6, Int.ToString( hitObject->GetPosition().z ));
        
    }
    
    */
    
    //int chunkIndex = chunkManager.FindChunk(chunkManager.playerChunkX * chunkManager.chunkSize, chunkManager.playerChunkZ * chunkManager.chunkSize);
    //if (chunkIndex != -1) {
    //    Chunk chunk = chunkManager.GetChunk(chunkIndex);
    //    
    //    Engine.WriteDialog(2, Int.ToString( chunk.actorList.size() ));
    //    
    //}
    
    
    //
    // Print aimed entity details
    //
    
    distance = 8;
    
    from      = Engine.cameraController->GetComponent<Transform>()->position;
    direction = Engine.cameraController->GetComponent<Camera>()->forward;
    
    // Center the from angle
    from.y -= 0.3f;
    
    if (Physics.Raycast(from, direction, distance, hit, LayerMask::Actor)) {
        
        GameObject* hitObject = (GameObject*)hit.gameObject;
        Actor* hitActor = hitObject->GetComponent<Actor>();
        
        hitActor->SetUpdateGeneticsFlag();
        
        Engine.WriteDialog(1, hitActor->GetName());
        Engine.WriteDialog(2, Int.ToString(hitActor->GetAge()));
        
        Engine.WriteDialog(3, "Genome");
        
        // Print out the genes from the actors genome
        std::vector<std::string> genes = String.Explode(AI.genomes.ExtractGenome(hitActor), '#');
        for (unsigned int i=0; i < genes.size(); i++) {
            
            std::string gene = genes[i];
            
            Engine.WriteDialog(5 + i, gene);
            
        }
        
    } else {
        
        //for (unsigned int i=0; i < 10; i++) 
        //    Engine.WriteDialog(1 + i, "");
        
    }
    
    // Move the actor back into position as we are now finished casting rays...
    bodyPosition.y -= 1000;
    bodyTransform.setPosition(bodyPosition);
    
    
    
    
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
    if (Weather.sunLight != nullptr) {
        Transform* lightTransform = Weather.sunObject->GetComponent<Transform>();
        
        if (!Platform.isPaused) {
            
            if (Input.CheckKeyCurrent(VK_I)) {ambientLight += 0.01f;}
            if (Input.CheckKeyCurrent(VK_K)) {ambientLight -= 0.01f;}
            
            if (Input.CheckKeyCurrent(VK_T)) {lightTransform->RotateEuler(0, 0,  0.1);}
            if (Input.CheckKeyCurrent(VK_G)) {lightTransform->RotateEuler(0, 0, -0.1);}
            
        }
        
    }
    
    
    // Ambient limits
    if (ambientLight > 0.87f) ambientLight = 0.87f;
    if (ambientLight < 0.0f)  ambientLight = 0.0f;
    
    // World brightness
    //chunkManager.world.staticColorMul = ambientLight;
    
    // Sky brightness
    if (Weather.skyMaterial != nullptr) 
        Weather.skyMaterial->ambient = Math.Lerp(skyLightingMin, skyLightingMax, ambientLight);
    
    // Light brightness
    if (Weather.sunLight != nullptr) 
        Weather.sunLight->intensity = Math.Lerp(lightingMin, lightingMax, ambientLight);
    
    
    
    
    
    
    
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




