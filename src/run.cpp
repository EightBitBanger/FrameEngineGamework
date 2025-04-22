#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>




void ApplyGene(Actor* targetActor) {
    
    targetActor->physical.SetAge( 1000 );
    targetActor->ReexpressPhenotype();
    //targetActor->RebuildPhenotype();
    
    while (1) {
        
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Human(targetActor); return;}
        
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Bovine(targetActor); return;}
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Horse(targetActor); return;}
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Dog(targetActor); return;}
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Bear(targetActor); return;}
    }
    
    return;
}







glm::vec3 force(0);
float forceDblTime=0;

bool isDebugReportActive = false;
bool isFullScreen = false;

std::string targetGene = "";

Actor* actorSelected = nullptr;

bool isNetInit = false;
bool isTrigger = false;

AudioSample* sample;
Sound* sound;

void Run() {
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    GameWorld.Update();
    
    sample = Audio.CreateAudioSample();
    Audio.Presets.RenderWhiteNoise(sample, 0.8f);
    
    if (!isTrigger) {
        isTrigger = true;
        sample = Audio.CreateAudioSample();
        Audio.Presets.RenderWhiteNoise(sample, 0.1f);
        
    }
    
    
    /*
    
    // Create a neural network
    NeuralNetwork nnet;
    
    if (isNetInit == false) {
        isNetInit = true;
        
        // Add layers to the neural network
        
        nnet.AddNeuralLayer(16, 2);
        
        nnet.AddNeuralLayer(16, 16);
        nnet.AddNeuralLayer(16, 16);
        
        nnet.AddNeuralLayer(8, 16);
        
        
        // Train the model
        
        std::vector<TrainingSet> trainingBook;
        
        TrainingSet ts[5];
        
        // Idle state
        ts[0].input  = {0.5f, 0.5f};
        ts[0].target = {0.06f, 0.4f, 0.01f, 0.9f,    0.1f, 0.1f, 0.1f, 0.1f};
        
        // Attack state
        ts[1].input  = {0.5f, 0.3f};
        ts[1].target = {0.5f, 0.5f, 0.5f, 0.5f,    0.3f, 0.2f, 0.3f, 0.3f};
        
        // Flee state
        ts[2].input  = {0.3f, 0.5f};
        ts[2].target = {0.8f, 0.8f, 0.8f, 0.8f,    0.6f, 0.2f, 0.5f, 0.5f};
        
        
        trainingBook.push_back(ts[0]);
        trainingBook.push_back(ts[1]);
        trainingBook.push_back(ts[2]);
        
        for (int epoch = 0; epoch < 300000; epoch++) {
            
            for (std::vector<TrainingSet>::iterator it = trainingBook.begin(); it != trainingBook.end(); ++it) {
                nnet.Train(*it, 0.4f);
            }
            
        }
        
        
        
        // Test the network
        
        std::vector<float> dataset = {0.5f, 0.5f};
        
        nnet.FeedForward(dataset);
        
        std::vector<float> results = nnet.GetResults();
        
        std::string output = Float.ToString( results[0] ) + " " + 
                             Float.ToString( results[1] ) + " " + 
                             Float.ToString( results[2] ) + " " + 
                             Float.ToString( results[3] ) + " " + 
                             
                             Float.ToString( results[4] ) + " " + 
                             Float.ToString( results[5] ) + " " + 
                             Float.ToString( results[6] ) + " " + 
                             Float.ToString( results[7] );
        
        Engine.WriteDialog(1, output);
        
        
        // Save the state
        
        std::vector<std::string> states = nnet.SaveState();
        
        unsigned int numberOfStates = states.size();
        
        std::string saveString;
        
        for (unsigned int i=0; i < numberOfStates; i++) 
            saveString += states[i] + "\n";
        
        Serializer.Serialize("neuralstates.dat", (void*)saveString.data(), saveString.size());
        
        std::vector<float> saveState = nnet.SaveStateBin();
        
        
        
        
        // Load the neural state
        /
        NeuralNetwork newnet;
        
        std::string buffer;
        unsigned int fileSz = Serializer.GetFileSize("neuralstates.dat");
        
        buffer.resize(fileSz);
        Serializer.Deserialize("neuralstates.dat", (void*)buffer.data(), fileSz);
        
        std::vector<std::string> loadStates = String.Explode(buffer, '\n');
        
        
        newnet.LoadStateBin( saveState );
        
        
        
        // Test the network
        
        std::vector<float> testdataset = {0.5f,0.7f};
        
        newnet.FeedForward(testdataset);
        
        std::vector<float> testresults = newnet.GetResults();
        
        std::string testoutput = Float.ToString( testresults[0] ) + " " + 
                                 Float.ToString( testresults[1] ) + " " + 
                                 Float.ToString( testresults[2] ) + " " + 
                                 Float.ToString( testresults[3] );
        
        Engine.WriteDialog(1, testoutput);
        /
        
        
    }
    */
    
    
    
    
    
    
    
    
    
    // Cast a ray from the player
    
    Hit hit;
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    
    glm::vec3 from    = cameraPtr->transform.position;
    glm::vec3 forward = cameraPtr->forward;
    
    // Prevent player controller from going under ground
    if (Physics.Raycast(from, glm::vec3(0.0f, -1.0f, 0.0f), 1000, hit, LayerMask::Ground)) {
        
        RigidBody* bodyPtr = Engine.cameraController->GetComponent<RigidBody>();
        
        rp3d::Transform transform = bodyPtr->getTransform();
        rp3d::Vector3 position = transform.getPosition();
        
        if (position.y < hit.point.y) {
            position.y = hit.point.y;
            transform.setPosition( position );
            bodyPtr->setTransform( transform );
        }
        
    }
    
    
    
    // DEBUG - Show data on the aimed actor
    
    /*
    glm::vec3 fromHigh = from;
    
    if (Physics.Raycast(from, forward, 100, hit, LayerMask::Actor)) {
        
        GameObject* hitObject = (GameObject*)hit.gameObject;
        Actor* hitActor = hitObject->GetComponent<Actor>();
        
        Engine.WriteDialog( 2, hitActor->GetName() );
        Engine.WriteDialog( 3, Int.ToString( hitActor->physical.GetAge() ) );
        
        unsigned int numberOfGenes = hitActor->genetics.GetNumberOfGenes();
        
    } else {
        
        for (unsigned int i=0; i < 2; i++) 
            Engine.WriteDialog(i + 2, "");
        
    }
    
    
    if (Physics.Raycast(from, forward, 100, hit, LayerMask::Static)) {
        
        GameObject* hitObject = (GameObject*)hit.gameObject;
        
        Engine.WriteDialog( 2, "OBJECT DETECTED" );
        
    } else {
        
        for (unsigned int i=0; i < 1; i++) 
            Engine.WriteDialog(i + 2, "");
        
    }
    */
    
    // Spawn an actor with the picked genome
    
    if (Input.CheckMouseLeftPressed()) {
        
        if (Physics.Raycast(from, forward, 100, hit, LayerMask::Ground)) {
            
            for (unsigned int retry=0; retry < 8; retry++) {
                
                float randomX = Random.Range(0, 10) - Random.Range(0, 10);
                float randomZ = Random.Range(0, 10) - Random.Range(0, 10);
                
                GameObject* actorObject = GameWorld.SpawnActor( hit.point.x + randomX, hit.point.y, hit.point.z + randomZ);
                Actor* actor = actorObject->GetComponent<Actor>();
                
                ApplyGene(actor);
                
                actor->physical.SetAge(400 + Random.Range(0, 100));
                AI.genomes.mental.PreyBase( actor );
                
            }
            
        }
        
    }
    
    // Plant tree (testing)
    if (Input.CheckKeyPressed(VK_P)) {
        
        if (Physics.Raycast(from, forward, 1000, hit, LayerMask::Ground)) {
            
            GameObject* gameObject = (GameObject*)hit.gameObject;
            
            std::vector<std::string> name_pos = String.Explode(gameObject->name, '_');
            float posX = String.ToInt( name_pos[0] );
            float posZ = String.ToInt( name_pos[1] );
            
            Chunk* chunk = GameWorld.FindChunk(posX, posZ);
            
            if (chunk->gameObject != nullptr) {
                
                float chunkPosX = posX - hit.point.x;
                float chunkPosY = hit.point.y;
                float chunkPosZ = posZ - hit.point.z;
                
                GameObject* hitObject = chunk->staticObject;
                
                MeshRenderer* chunkRenderer = hitObject->GetComponent<MeshRenderer>();
                
                StaticObject staticObj;
                staticObj.x = chunkPosX;
                staticObj.y = chunkPosY;
                staticObj.z = chunkPosZ;
                
                GameWorld.AddDecorTree(*chunk, staticObj, chunkRenderer->mesh, -chunkPosX, chunkPosY, -chunkPosZ, Decoration::TreeOak);
                chunkRenderer->mesh->Load();
                
            }
            
        }
        
    }
    
    
    
    // Kill actor test
    if (Input.CheckMouseRightPressed()) {
        
        if (Physics.Raycast(from, forward, 1000, hit, LayerMask::Actor)) {
            
            GameWorld.KillActor( (GameObject*)hit.gameObject );
            
        }
        
    }
    
    
    //
    // Profiling
    //
    
    /*
    if (Input.CheckKeyPressed(VK_F4)) {
        
        if (Engine.CheckIsProfilerActive()) {
            Engine.DisableProfiler();
            
            for (unsigned int i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) 
                Engine.WriteDialog(i, "");
            
        } else {
            
            Engine.EnableProfiler();
        }
        
    }
    */
    
    
    if (Input.CheckKeyPressed(VK_F3)) {
        
        isDebugReportActive = !isDebugReportActive;
        
        if (!isDebugReportActive) {
            //for (unsigned int i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) 
            //    Engine.WriteDialog(i, "");
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
                
                //Engine.WriteDialog(0, chunkPosition);
                
            }
            
        }
        
        // Print player position
        glm::vec3 cameraPosition = Engine.cameraController->GetPosition();
        
        std::string playerPosition = "x ";
        playerPosition += Int.ToString( cameraPosition.x );
        playerPosition += "  y ";
        playerPosition += Int.ToString( cameraPosition.y );
        playerPosition += "  z ";
        playerPosition += Int.ToString( cameraPosition.z );
        
        //Engine.WriteDialog(3, playerPosition);
        
        // Check object in front of camera
        
        /*
        
        if (Physics.Raycast(from, direction, distance, hit, LayerMask::Actor)) {
            
            GameObject* hitObject = (GameObject*)hit.gameObject;
            Actor* hitActor = hitObject->GetComponent<Actor>();
            
            Engine.WriteDialog(4, hitActor->GetName());
            Engine.WriteDialog(5, "Age: " + Int.ToString(hitActor->GetAge()));
            
            float actorChunkX = Math.Round( hitObject->GetPosition().x / GameWorld.chunkSize ) * GameWorld.chunkSize;
            float actorChunkZ = Math.Round( hitObject->GetPosition().z / GameWorld.chunkSize ) * GameWorld.chunkSize;
            
            // Set actor chunk to the current chunk
            Chunk* chunkPtr = GameWorld.CheckChunk( glm::vec2(actorChunkX, actorChunkZ) );
            if (chunkPtr != nullptr) {
                
                Engine.WriteDialog(6, Float.ToString(chunkPtr->position.x / GameWorld.chunkSize) + 
                                      ", " + 
                                      Float.ToString(chunkPtr->position.y / GameWorld.chunkSize));
                
            }
            
            Engine.WriteDialog(7, Float.ToString(actorChunkX) + ", " + Float.ToString(actorChunkZ));
            
            Engine.WriteDialog(8, "Genes         " + Int.ToString( hitActor->GetNumberOfGenes() ));
            
        } else {
            
            for (unsigned int i=0; i < 10-4; i++) 
                Engine.WriteDialog(4 + i, "");
            
        }
        */
        
    }
    
    
    
    
    
    //
    // Camera controller movement
    //
    
    if (Engine.cameraController == nullptr) 
        return;
    
    float forceAccelerate = 0.0043f;
    float forceDecelerate = 0.015f;
    
    Camera* mainCamera = Engine.sceneMain->camera;
    
    if (mainCamera != nullptr) {
        
        // No movement when paused
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
        if (Input.CheckKeyCurrent(VK_CONTROL)) 
            forceDblTime += 0.24f;
        
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
        float fovPullback = glm::length(forceTotal) * 40.0f;
        
        if (fovPullback > 4.0f) 
            fovPullback = 4.0f;
        
        Engine.sceneMain->camera->fov = 60 + fovPullback;
        
    }
    
    
    //
    // Pausing
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        
        Platform.Pause();
        
        if (Platform.isPaused) {
            
            MainMenuEnable();
            
        } else {
            
            MainMenuDisable();
            
        }
        
    }
    
    return;
}


void TickUpdate(void) {
    
    return;
}

