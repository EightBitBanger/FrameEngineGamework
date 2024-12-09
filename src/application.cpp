// User application code space
//

#include <GameEngineFramework/Engine/EngineSystems.h>


// User plug-ins

#include <GameEngineFramework/Plugins/ChunkSpawner/ChunkManager.h>
ChunkManager chunkManager;


#include <GameEngineFramework/Plugins/WeatherSystem/WeatherSystem.h>
WeatherSystem weather;


// Day night cycle
bool cycleDirection = false;

float ambientLight = 0.9;



// List worlds
void FuncList(std::vector<std::string> args) {
    
    std::vector<std::string> dirList = Directory.List("worlds");
    
    if (dirList.size() == 0) {
        
        Engine.Print("No worlds");
        
        return;
    }
    
    for (unsigned int i=0; i < dirList.size(); i++) 
        Engine.Print( dirList[i] );
    
    return;
}

// Save world
void FuncSave(std::vector<std::string> args) {
    
    if (chunkManager.SaveWorld()) {
        
        Engine.Print("World saved");
        
        return;
    }
    
    Engine.Print("World save error: " + chunkManager.world.name);
    
    return;
}

// Load world
void FuncLoad(std::vector<std::string> args) {
    
    if (chunkManager.LoadWorld()) {
        
        Engine.Print("World loaded");
        
        return;
    }
    
    Engine.Print("World not found: " + chunkManager.world.name);
    
    return;
}

// Set world name
void FuncName(std::vector<std::string> args) {
    
    if (args[0][0] > 0x20) 
        chunkManager.world.name = args[0];
    
    std::string worldName = "Name: " + chunkManager.world.name;
    
    Engine.Print(worldName);
    
    return;
}

// Clear world
void FuncClear(std::vector<std::string> args) {
    
    chunkManager.ClearWorld();
    
    Engine.Print("World cleared");
    
    return;
}

// Set the world seed
void FuncSeed(std::vector<std::string> args) {
    
    /*
    if (args[0] == "") {
        
        Engine.Print("World seed: " + Int.ToString(chunkManager.worldSeed));
        
        return;
    }
    
    chunkManager.worldSeed = String.ToInt(args[0]);
    
    Engine.Print("World seed: " + Int.ToString(chunkManager.worldSeed));
    */
    
    return;
}


// Summon an actor
void FuncSummon(std::vector<std::string> args) {
    
    for (uint8_t i=0; i < 1; i++) {
        
        unsigned int entityType = 0;
        
        if (args[0] == "Sheep")   {entityType = 1;}
        if (args[0] == "Bear")    {entityType = 2;}
        
        if (entityType == 0) {
            
            Engine.Print("Unknown actor type");
            
            return;
        }
        
        glm::vec3 randomOffset(Random.Range(0, 50) - Random.Range(0, 50), 
                               0, 
                               Random.Range(0, 50) - Random.Range(0, 50));
        
        GameObject* newActorObject = Engine.CreateAIActor( Engine.sceneMain->camera->transform.GetPosition() - randomOffset );
        Actor* newActor = newActorObject->GetComponent<Actor>();
        
        newActor->SetAge( 1000 );
        
        switch (entityType) {
            
            default:
            case 1: AI.genomes.Sheep( newActor ); break;
            case 2: AI.genomes.Bear( newActor ); break;
            
        }
        
        //chunkManager.actors.push_back( newActorObject );
        
        continue;
    }
    
    Engine.Print("Actor summoned");
    
    return;
}






//
// Application entry point
//

void Start() {
    
    // Load console functions
    Engine.ConsoleRegisterCommand("summon",  FuncSummon);
    Engine.ConsoleRegisterCommand("list",    FuncList);
    Engine.ConsoleRegisterCommand("name",    FuncName);
    Engine.ConsoleRegisterCommand("save",    FuncSave);
    Engine.ConsoleRegisterCommand("load",    FuncLoad);
    Engine.ConsoleRegisterCommand("clear",   FuncClear);
    Engine.ConsoleRegisterCommand("seed",    FuncSeed);
    
    
    Platform.HideMouseCursor();
    
    chunkManager.Initiate();
    
    weather.Initiate();
    
    Engine.DisableConsoleCloseOnReturn();
    
    
    //Engine.EnableProfiler();
    
    //Engine.EnablePhysicsDebugRenderer();
    
    
    //
    // Audio test sample
    
    Sound* soundA = Audio.CreateSound();
    AudioSample* sampleA = Audio.CreateAudioSample();
    
    sampleA->sample_rate = 44100;
    
    //Samples.RenderBlankSpace(sampleA, 0.4f);
    //Samples.RenderSweepingSineWave(sampleA, 17000, 11000, 0.1f);
    //Samples.RenderBlankSpace(sampleA, 0.7f);
    //Samples.RenderSweepingSineWave(sampleA, 17000, 15000, 0.1f);
    //Samples.RenderBlankSpace(sampleA, 1.0f);
    //Samples.RenderSweepingSineWave(sampleA, 17000, 15000, 0.1f);
    //Samples.RenderBlankSpace(sampleA, 0.1f);
    //Samples.RenderSweepingSineWave(sampleA, 17000, 15000, 0.1f);
    
    
    soundA->LoadSample(sampleA);
    
    //soundA->SetVolume(0.4f);
    
    //soundA->Play();
    //while (soundA->IsSamplePlaying());
    
    
    
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
    weather.skyObject->GetComponent<Transform>()->parent = Engine.cameraController->GetComponent<Transform>();
    
    
    
    
    
    
    //
    // Chunk generation
    //
    
    Decoration decorGrass;
    decorGrass.type = DECORATION_GRASS;
    decorGrass.density = 8000;
    decorGrass.spawnHeightMaximum = 35;
    decorGrass.spawnHeightMinimum = chunkManager.world.waterLevel;
    decorGrass.spawnStackHeightMin = 1;
    decorGrass.spawnStackHeightMax = 2;
    
    Decoration decorTrees;
    decorTrees.type = DECORATION_TREE;
    decorTrees.density = 800;
    decorTrees.spawnHeightMaximum = 20;
    decorTrees.spawnHeightMinimum = chunkManager.world.waterLevel;
    decorTrees.spawnStackHeightMin = 4;
    decorTrees.spawnStackHeightMax = 8;
    
    Decoration decorTreeHights;
    decorTreeHights.type = DECORATION_TREE;
    decorTreeHights.density = 1700;
    decorTreeHights.spawnHeightMaximum = 40;
    decorTreeHights.spawnHeightMinimum = 20;
    decorTreeHights.spawnStackHeightMin = 4;
    decorTreeHights.spawnStackHeightMax = 8;
    
    // Water adjacent plants
    
    Decoration decorWaterPlants;
    decorWaterPlants.type = DECORATION_GRASS_THIN;
    decorWaterPlants.density = 3000;
    decorWaterPlants.spawnHeightMaximum = chunkManager.world.waterLevel;
    decorWaterPlants.spawnHeightMinimum = -100;
    decorWaterPlants.spawnStackHeightMax = 4;
    decorWaterPlants.spawnStackHeightMin = 2;
    
    
    // Actors
    Decoration decorSheep;
    decorSheep.type = DECORATION_ACTOR;
    decorSheep.name = "Sheep";
    decorSheep.density = 24;
    decorSheep.spawnHeightMaximum = 10;
    decorSheep.spawnHeightMinimum = chunkManager.world.waterLevel;
    
    Decoration decorBear;
    decorBear.type = DECORATION_ACTOR;
    decorBear.name = "Bear";
    decorBear.density = 2;
    decorBear.spawnHeightMaximum = 40;
    decorBear.spawnHeightMinimum = 5;
    
    
    chunkManager.world.mDecorations.push_back(decorGrass);
    chunkManager.world.mDecorations.push_back(decorTrees);
    chunkManager.world.mDecorations.push_back(decorTreeHights);
    
    chunkManager.world.mDecorations.push_back(decorWaterPlants);
    
    chunkManager.world.mDecorations.push_back(decorSheep);
    chunkManager.world.mDecorations.push_back(decorBear);
    
    
    
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
    
    
    
    
    chunkManager.perlin.push_back(perlinMountainB);
    chunkManager.perlin.push_back(perlinMountainA);
    chunkManager.perlin.push_back(perlinBase);
    chunkManager.perlin.push_back(perlinLayerA);
    chunkManager.perlin.push_back(perlinLayerB);
    chunkManager.perlin.push_back(perlinFlatland);
    
    
    // Lighting levels
    
    chunkManager.world.chunkColorLow   = Colors.MakeGrayScale(0.3f);
    chunkManager.world.staticColorLow  = Colors.MakeGrayScale(0.3f);
    chunkManager.world.actorColorLow   = Colors.MakeGrayScale(0.02f);
    
    chunkManager.world.chunkColorHigh  = Colors.MakeGrayScale(0.87f);
    chunkManager.world.staticColorHigh = Colors.MakeGrayScale(0.87f);
    chunkManager.world.actorColorHigh  = Colors.MakeGrayScale(0.87f);
    
    chunkManager.world.ambientLight = 0.87f;
    
    
    // World rendering
    
    chunkManager.renderDistance = 24;
    
    
    
    return;
}














//
// Application loop
//

glm::vec3 force(0);
float forceDblTime=0;

bool isDebugReportActive = false;
bool isFullScreen = false;

std::string targetName = "";
std::string targetGene = "";







void Run() {
    
    chunkManager.Update();
    
    
    
    //
    // Raycast from player
    //
    
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
    
    
    
    distance = 13;
    
    from      = Engine.cameraController->GetComponent<Transform>()->position;
    direction = Engine.cameraController->GetComponent<Camera>()->forward;
    
    // Center the from angle
    from.y += 0.24f;
    
    
    if (Input.CheckMouseMiddlePressed()) {
        
        if (Physics.Raycast(from, direction, distance, hit, LayerMask::Actor)) {
            
            GameObject* hitObject = (GameObject*)hit.gameObject;
            Actor* hitActor = hitObject->GetComponent<Actor>();
            
            targetGene = AI.genomes.ExtractGenome(hitActor);
            
            std::string destGene = "gene<" + targetGene + ">";
            
            Platform.SetClipboardText( destGene );
            
            unsigned int numberOfGenes = hitActor->GetNumberOfGenes();
            
            for (unsigned int i=0 ; i < numberOfGenes; i++) {
                
                MeshRenderer* geneRenderer = hitActor->GetMeshRendererAtIndex(i);
                
                std::string geneDataString = "";
                
                float xPos = geneRenderer->transform.position.x;
                float yPos = geneRenderer->transform.position.y;
                float zPos = geneRenderer->transform.position.z;
                
                float xScale = geneRenderer->transform.scale.x;
                float yScale = geneRenderer->transform.scale.y;
                float zScale = geneRenderer->transform.scale.z;
                
                geneDataString += Float.ToString(xPos) + ", " + Float.ToString(yPos) + ", " + Float.ToString(zPos) + "    ";
                geneDataString += Float.ToString(xScale) + ", " + Float.ToString(yScale) + ", " + Float.ToString(zScale);
                
                Engine.Print( geneDataString );
                
            }
            
        }
        
    }
    
    if (Input.CheckMouseLeftPressed()) {
        
        if (Physics.Raycast(from, direction, distance, hit, LayerMask::Ground)) {
            
            std::string sourceGene = Platform.GetClipboardText();
            
            if (sourceGene.size() == 0) 
                return;
            
            // Remove end character
            size_t pos = sourceGene.find(">");
            if (pos != std::string::npos) 
                sourceGene.erase(pos, 1);
            
            std::vector<std::string> sourceArray = String.Explode(sourceGene, '<');
            
            if ((sourceArray[0][0] == 'g') & 
                (sourceArray[0][1] == 'e') & 
                (sourceArray[0][2] == 'n') & 
                (sourceArray[0][3] == 'e')) {
                
                GameObject* newActorObject = chunkManager.SpawnActor( hit.point.x, hit.point.y, hit.point.z );
                
                GameObject* hitObject = (GameObject*)hit.gameObject;
                Chunk chunkPtr = chunkManager.chunks[0];
                
                Actor* newActor = newActorObject->GetComponent<Actor>();
                chunkManager.actors.push_back(newActorObject);
                
                AI.genomes.PreyBase( newActor );
                
                AI.genomes.InjectGenome(newActor, sourceArray[1]);
                newActor->SetGeneticUpdateFlag();
                
                newActor->SetAge( Random.Range(200, 4000) );
                
            }
            
        }
        
    }
    
    
    if (Input.CheckMouseRightPressed()) {
        
        if (Physics.Raycast(from, direction, distance, hit, LayerMask::Actor)) {
            
            GameObject* hitObject = (GameObject*)hit.gameObject;
            
            chunkManager.KillActor( hitObject );
            
        }
        
    }
    
    
    // Move the actor back into position as we are now finished casting rays...
    bodyPosition.y -= 1000;
    bodyTransform.setPosition(bodyPosition);
    
    
    
    
    
    
    //
    // Profiling
    //
    if (Input.CheckKeyPressed(VK_F4)) {
        
        if (Engine.CheckIsProfilerActive()) {
            Engine.DisableProfiler();
            
            for (unsigned int i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) 
                Engine.WriteDialog(i, "");
            
        } else {
            
            Engine.EnableProfiler();
        }
        
    }
    
    if (Input.CheckKeyPressed(VK_F3)) {
        
        isDebugReportActive = !isDebugReportActive;
        
        if (!isDebugReportActive) {
            for (unsigned int i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) 
                Engine.WriteDialog(i, "");
        }
        
    }
    
    if (Input.CheckKeyPressed(VK_F11)) {
        
        isFullScreen = !isFullScreen;
        
        if (isFullScreen) {
            
            Platform.WindowEnableFullscreen();
            
        } else {
            
            Platform.WindowDisableFullscreen();
            
        }
        
    }
    
    // Debug report
    
    /*
    
    if (isDebugReportActive) {
        
        // Print current chunk details
        
        if (Physics.Raycast(from, glm::vec3(0, -1, 0), 1000, hit, LayerMask::Ground)) {
            
            GameObject* hitObject = (GameObject*)hit.gameObject;
            MeshRenderer* chunkRenderer = hitObject->GetComponent<MeshRenderer>();
            
            Chunk* hitChunk = (Chunk*)hitObject->GetUserData();
            
            if (hitObject != nullptr) {
                
                std::string chunkPosition = "Chunk ";
                
                chunkPosition += Float.ToString( hitObject->GetPosition().x );
                chunkPosition += ", ";
                chunkPosition += Float.ToString( hitObject->GetPosition().z );
                
                Engine.WriteDialog(0, chunkPosition);
                
                if (hitChunk->actorList.size() > 0) 
                    Engine.WriteDialog(1, "Actors " + Int.ToString(hitChunk->actorList.size()));
                
            }
            
        }
        
        // Print player position
        glm::vec3 cameraPosition = Engine.cameraController->GetPosition();
        
        std::string playerPosition = "x: ";
        playerPosition += Int.ToString( cameraPosition.x );
        playerPosition += " y: ";
        playerPosition += Int.ToString( cameraPosition.y );
        playerPosition += " z: ";
        playerPosition += Int.ToString( cameraPosition.z );
        
        Engine.WriteDialog(3, playerPosition);
        
        // Check object in front of camera
        
        if (Physics.Raycast(from, direction, distance, hit, LayerMask::Actor)) {
            
            GameObject* hitObject = (GameObject*)hit.gameObject;
            Actor* hitActor = hitObject->GetComponent<Actor>();
            
            Engine.WriteDialog(4, hitActor->GetName());
            Engine.WriteDialog(5, "Age: " + Int.ToString(hitActor->GetAge()));
            
            float actorChunkX = Math.Round( hitObject->GetPosition().x / chunkManager.chunkSize ) * chunkManager.chunkSize;
            float actorChunkZ = Math.Round( hitObject->GetPosition().z / chunkManager.chunkSize ) * chunkManager.chunkSize;
            
            // Set actor chunk to the current chunk
            Chunk* chunkPtr = chunkManager.CheckChunk( glm::vec2(actorChunkX, actorChunkZ) );
            if (chunkPtr != nullptr) {
                
                Engine.WriteDialog(6, Float.ToString(chunkPtr->position.x / chunkManager.chunkSize) + 
                                      ", " + 
                                      Float.ToString(chunkPtr->position.y / chunkManager.chunkSize));
                
            }
            
            Engine.WriteDialog(7, Float.ToString(actorChunkX) + ", " + Float.ToString(actorChunkZ));
            
            Engine.WriteDialog(8, "Genes         " + Int.ToString( hitActor->GetNumberOfGenes() ));
            
        } else {
            
            for (unsigned int i=0; i < 10-4; i++) 
                Engine.WriteDialog(4 + i, "");
            
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
    chunkManager.world.ambientLight = ambientLight;
    
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
    
    */
    
    
    
    
    //
    // Camera controller movement
    //
    
    if (Engine.cameraController == nullptr) 
        return;
    
    float forceAccelerate = 0.012f;
    float forceDecelerate = 0.015f;
    
    Camera* mainCamera = Engine.sceneMain->camera;
    
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




