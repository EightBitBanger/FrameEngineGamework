#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>




void ApplyGene(Actor* targetActor) {
    /*
    targetActor->physical.SetAge( 100 + Random.Range(0, 400) );
    targetActor->RebuildPhenotype();
    
    while (1) {
        
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Human(targetActor); return;}
        
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Bovine(targetActor); return;}
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Horse(targetActor); return;}
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Dog(targetActor); return;}
        if (Random.Range(0, 100) > 70) {AI.genomes.presets.Bear(targetActor); return;}
    }
    */
    
    targetActor->physical.SetAge( Random.Range(1000, 1100) );
    targetActor->ReexpressPhenotype();
    
    if (Random.Range(0, 100) > 70) {
        
        AI.genomes.presets.Human(targetActor);
        return;
    }
    
    AI.genomes.ClearGenes(targetActor);
    
    targetActor->SetName("Dog");
    
    targetActor->physical.SetAdultAge(750);
    targetActor->physical.SetSeniorAge(10000);
    
    targetActor->physical.SetSpeed(0.9f);
    targetActor->physical.SetSpeedYouth(1.2f);
    targetActor->physical.SetSpeedMultiplier(1.4f);
    
    targetActor->physical.SetYouthScale(0.3f);
    targetActor->physical.SetAdultScale(1.0f);
    
    targetActor->behavior.SetHeightPreferenceMax(30.0f);
    targetActor->behavior.SetHeightPreferenceMin(10.0f);
    
    targetActor->biological.health = 100;
    
    AI.genomes.mental.PredatorBase(targetActor);
    
    if (Random.Range(0, 100) > 55) 
    {targetActor->physical.SetSexualOrientation(true);} else  // Male
    {targetActor->physical.SetSexualOrientation(false);}      // Female
    
    // Color variants
    Color headColor = Colors.MakeRandomGrayScale();
    Color bodyColor = Colors.MakeRandomGrayScale();
    Color limbColor = Colors.MakeRandomGrayScale();
    Color tailColor = Colors.MakeRandomGrayScale();
    
    headColor *= 0.7f;
    bodyColor *= 0.5f;
    limbColor *= 0.3f;
    tailColor *= 0.2f;
    
    // Body gene
    Gene geneBody;
    geneBody.offset    = Codon(0, 0, -0.1);
    geneBody.position  = Codon(0, 0.34, 0);
    geneBody.rotation  = Codon(0, 0, 0);
    geneBody.scale     = Codon(0.25, 0.25, 0.4);
    geneBody.color.x   = bodyColor.r;
    geneBody.color.y   = bodyColor.g;
    geneBody.color.z   = bodyColor.b;
    
    // Upper body gene
    Gene geneBodyUpper;
    geneBodyUpper.offset    = Codon(0, 0, 0.2);
    geneBodyUpper.position  = Codon(0, 0.35, 0);
    geneBodyUpper.rotation  = Codon(0, 0, 0);
    geneBodyUpper.scale     = Codon(0.29, 0.29, 0.22);
    geneBodyUpper.color.x   = bodyColor.r;
    geneBodyUpper.color.y   = bodyColor.g;
    geneBodyUpper.color.z   = bodyColor.b;
    
    // Head gene
    Gene geneHead;
    geneHead.offset    = Codon(0, 0, 0);
    geneHead.position  = Codon(0, 0.43, 0.33);
    geneHead.rotation  = Codon(0, 0, 0);
    geneHead.scale     = Codon(0.23, 0.23, 0.18);
    geneHead.color.x   = headColor.r;
    geneHead.color.y   = headColor.g;
    geneHead.color.z   = headColor.b;
    
    // Muzzle gene
    Gene geneMuzzle;
    geneMuzzle.attachmentIndex = 3;
    geneMuzzle.offset    = Codon(0, 0, 0);
    geneMuzzle.position  = Codon(0, 0.4, 0.45);
    geneMuzzle.rotation  = Codon(0, 0, 0);
    geneMuzzle.scale     = Codon(0.15, 0.15, 0.14);
    geneMuzzle.color.x   = headColor.r;
    geneMuzzle.color.y   = headColor.g;
    geneMuzzle.color.z   = headColor.b;
    
    // Ear left gene
    Gene geneEarLeft;
    geneEarLeft.attachmentIndex = 3;
    geneEarLeft.offset    = Codon(0.0, 0.0, 0.0);
    geneEarLeft.position  = Codon(0.07, 0.555, 0.27);
    geneEarLeft.rotation  = Codon(0.1, 0, -0.01);
    geneEarLeft.scale     = Codon(0.07, 0.1, 0.04);
    geneEarLeft.color.x   = headColor.r;
    geneEarLeft.color.y   = headColor.g;
    geneEarLeft.color.z   = headColor.b;
    
    // Ear right gene
    Gene geneEarRight;
    geneEarRight.attachmentIndex = 3;
    geneEarRight.offset    = Codon(0.0, 0.0, 0.0);
    geneEarRight.position  = Codon(-0.07, 0.555, 0.27);
    geneEarRight.rotation  = Codon(0.1, 0, 0.01);
    geneEarRight.scale     = Codon(0.07, 0.1, 0.04);
    geneEarRight.color.x   = headColor.r;
    geneEarRight.color.y   = headColor.g;
    geneEarRight.color.z   = headColor.b;
    
    // Limb FL gene
    Gene geneLimbFrontLeft;
    geneLimbFrontLeft.offset    = Codon(0.11, 0.35, 0.21);
    geneLimbFrontLeft.position  = Codon(0.0, -0.2, 0);
    geneLimbFrontLeft.rotation  = Codon(0, 0, 0);
    geneLimbFrontLeft.scale     = Codon(0.13, 0.4, 0.13);
    geneLimbFrontLeft.color.x   = limbColor.r;
    geneLimbFrontLeft.color.y   = limbColor.g;
    geneLimbFrontLeft.color.z   = limbColor.b;
    
    geneLimbFrontLeft.doAnimationCycle = true;
    geneLimbFrontLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbFrontLeft.animationRange   = 15;
    
    // Limb FR gene
    Gene geneLimbFrontRight;
    geneLimbFrontRight.offset    = Codon(-0.11, 0.35, 0.21);
    geneLimbFrontRight.position  = Codon(0.0, -0.2, 0);
    geneLimbFrontRight.rotation  = Codon(0, 0, 0);
    geneLimbFrontRight.scale     = Codon(0.13, 0.4, 0.13);
    geneLimbFrontRight.color.x   = limbColor.r;
    geneLimbFrontRight.color.y   = limbColor.g;
    geneLimbFrontRight.color.z   = limbColor.b;
    
    geneLimbFrontRight.doAnimationCycle   = true;
    geneLimbFrontRight.doInverseAnimation = true;
    geneLimbFrontRight.animationAxis      = Codon(1, 0, 0);
    geneLimbFrontRight.animationRange     = 15;
    
    // Limb RL gene
    Gene geneLimbRearLeft;
    geneLimbRearLeft.offset    = Codon(0.08, 0.3, -0.18);
    geneLimbRearLeft.position  = Codon(0.0, -0.2, -0.04);
    geneLimbRearLeft.rotation  = Codon(0, 0, 0);
    geneLimbRearLeft.scale     = Codon(0.13, 0.35, 0.13);
    geneLimbRearLeft.color.x   = limbColor.r;
    geneLimbRearLeft.color.y   = limbColor.g;
    geneLimbRearLeft.color.z   = limbColor.b;
    
    geneLimbRearLeft.doAnimationCycle = true;
    geneLimbRearLeft.animationAxis    = Codon(1, 0, 0);
    geneLimbRearLeft.animationRange   = 15;
    
    // Limb RR gene
    Gene geneLimbRearRight;
    geneLimbRearRight.offset    = Codon(-0.08, 0.3, -0.18);
    geneLimbRearRight.position  = Codon(-0.01, -0.2, -0.04);
    geneLimbRearRight.rotation  = Codon(0, 0, 0);
    geneLimbRearRight.scale     = Codon(0.13, 0.35, 0.13);
    geneLimbRearRight.color.x   = limbColor.r;
    geneLimbRearRight.color.y   = limbColor.g;
    geneLimbRearRight.color.z   = limbColor.b;
    
    geneLimbRearRight.doAnimationCycle   = true;
    geneLimbRearRight.doInverseAnimation = true;
    geneLimbRearRight.animationAxis      = Codon(1, 0, 0);
    geneLimbRearRight.animationRange     = 15;
    
    // Tail gene
    Gene geneTail;
    geneTail.offset    = Codon(0.0, 0.0, 0.0);
    geneTail.position  = Codon(0.0, 0.32, -0.33);
    geneTail.rotation  = Codon(-1.2, 0, 0);
    geneTail.scale     = Codon(0.07, 0.07, 0.3);
    geneTail.color.x   = tailColor.r;
    geneTail.color.y   = tailColor.g;
    geneTail.color.z   = tailColor.b;
    
    // Apply genes to the actor
    targetActor->genetics.AddGene(geneBody);
    targetActor->genetics.AddGene(geneBodyUpper);
    targetActor->genetics.AddGene(geneHead);
    targetActor->genetics.AddGene(geneMuzzle);
    targetActor->genetics.AddGene(geneEarLeft);
    targetActor->genetics.AddGene(geneEarRight);
    targetActor->genetics.AddGene(geneLimbFrontLeft);
    targetActor->genetics.AddGene(geneLimbFrontRight);
    targetActor->genetics.AddGene(geneLimbRearLeft);
    targetActor->genetics.AddGene(geneLimbRearRight);
    targetActor->genetics.AddGene(geneTail);
    
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
        //
        //
        // target 0 - Walk  
        // target 1 - Focus 
        // target 2 - Attack
        // target 3 - Flee  
        
        std::vector<TrainingSet> trainingBook;
        
        unsigned int bookSz = 4;
        
        TrainingSet ts[bookSz];
        
        
        // Idle state
        ts[0].input  = {0.5f, 0.5f};
        ts[0].target = {0.06f, 0.4f, 0.01f, 0.9f,    0.1f, 0.1f, 0.1f, 0.1f};
        
        // Moving state
        ts[1].input  = {0.8f, 0.3f};
        ts[1].target = {0.06f, 0.4f, 0.01f, 0.9f,    0.1f, 0.1f, 0.1f, 0.1f};
        
        // Attack state
        ts[2].input  = {0.5f, 0.3f};
        ts[2].target = {0.5f, 0.5f, 0.5f, 0.5f,    0.3f, 0.2f, 0.3f, 0.3f};
        
        // Flee state
        ts[3].input  = {0.3f, 0.5f};
        ts[3].target = {0.8f, 0.8f, 0.8f, 0.8f,    0.6f, 0.2f, 0.5f, 0.5f};
        
        // Add layers to the training book
        for (unsigned int i=0; i < bookSz; i++) 
            trainingBook.push_back(ts[i]);
        
        float learningRate = 0.4f;
        int epochCycles = 300000;
        
        for (int epoch = 0; epoch < epochCycles; epoch++) {
            
            for (std::vector<TrainingSet>::iterator it = trainingBook.begin(); it != trainingBook.end(); ++it) {
                nnet.Train(*it, learningRate);
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
        
        
        // Save the state
        
        std::vector<std::string> states = nnet.SaveState();
        
        unsigned int numberOfStates = states.size();
        
        std::string saveString;
        
        for (unsigned int i=0; i < numberOfStates; i++) 
            saveString += states[i] + "\n";
        
        Serializer.Serialize("prey.dat", (void*)saveString.data(), saveString.size());
        
        std::vector<float> saveState = nnet.SaveStateBin();
        
        
        
        
        // Load the neural state
        
        NeuralNetwork newnet;
        
        std::string buffer;
        unsigned int fileSz = Serializer.GetFileSize("prey.dat");
        
        buffer.resize(fileSz);
        Serializer.Deserialize("prey.dat", (void*)buffer.data(), fileSz);
        
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
        
        
    }
    
    
    
    
    
    
    
    
    
    
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
            
            for (unsigned int retry=0; retry < 4; retry++) {
                glm::vec3 position = Engine.cameraController->GetPosition();
                float xx = Random.Range(0, 10) - Random.Range(0, 10);
                float zz = Random.Range(0, 10) - Random.Range(0, 10);
                
                Actor* actor = AI.SpawnActor();
                AI.AddActorToSimulation(actor);
                ApplyGene(actor);
                actor->navigation.SetTargetPoint(glm::vec3(hit.point.x + xx, hit.point.y+5, hit.point.z + zz));
                
                GameObject* actorObject = (GameObject*)actor->user.GetUserDataA();
                actorObject->SetPosition(hit.point.x + xx, hit.point.y+5, hit.point.z + zz);
                
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
            
            GameObject* actorObject = (GameObject*)hit.gameObject;
            Actor* actor = actorObject->GetComponent<Actor>();
            
            AI.KillActor( actor );
            
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

