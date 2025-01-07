#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>


glm::vec3 force(0);
float forceDblTime=0;

bool isDebugReportActive = false;
bool isFullScreen = false;

std::string targetGene = "";

Actor* actorSelected = nullptr;

NeuralNetwork net;
bool initNet = false;





void Run() {
    
    /*
    
    if (!initNet) {
        initNet = true;
        
        net.AddNeuralLayer(4, 4);
        net.AddNeuralLayer(4, 4);
        net.AddNeuralLayer(4, 4);
        net.AddNeuralLayer(4, 4);
        net.AddNeuralLayer(4, 4);
        net.AddNeuralLayer(4, 4);
    }
    
    if (Input.CheckKeyPressed(VK_N)) {
        
        std::vector<float> inputs = {(Random.Range(0, 10000) * 0.1f) - (Random.Range(0, 10000) * 0.1f),
                                     (Random.Range(0, 10000) * 0.1f) - (Random.Range(0, 10000) * 0.1f),
                                     (Random.Range(0, 10000) * 0.1f) - (Random.Range(0, 10000) * 0.1f)};
        
        for (unsigned int i=0; i < inputs.size(); i++) 
            Engine.WriteDialog( 15 + i, Float.ToString(inputs[i]) );
        
        net.FeedForward(inputs);
    }
    
    std::vector<float> outputs = net.GetResults();
    for (unsigned int i=0; i < outputs.size(); i++) {
        
        Engine.WriteDialog( 19 + i, Float.ToString(outputs[i]) );
        
    }
    
    */
    
    
    
    // Update plug-in systems
    
    Weather.Update();
    
    Particle.Update();
    
    GameWorld.Update();
    
    
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
    glm::vec3 fromHigh = from;
    
    if (Physics.Raycast(from, forward, 100, hit, LayerMask::Actor)) {
        
        GameObject* hitObject = (GameObject*)hit.gameObject;
        Actor* hitActor = hitObject->GetComponent<Actor>();
        
        Engine.WriteDialog( 2, hitActor->GetName() );
        Engine.WriteDialog( 3, Int.ToString( hitActor->GetAge() ) );
        
        unsigned int numberOfGenes = hitActor->GetNumberOfGenes();
        
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
    
    
    
    // Pick an actors genome
    
    if (Input.CheckMouseMiddlePressed()) {
        
        if (Physics.Raycast(from, forward, 100, hit, LayerMask::Actor)) {
            
            GameObject* hitObject = (GameObject*)hit.gameObject;
            Actor* hitActor = hitObject->GetComponent<Actor>();
            
            actorSelected = hitActor;
            
            targetGene = AI.genomes.ExtractGenome(hitActor);
            
            std::string destGene = "gene<" + targetGene;
            
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
    
    
    // Spawn an actor with the picked genome
    
    if (Input.CheckMouseLeftPressed()) {
        
        if (Physics.Raycast(from, forward, 100, hit, LayerMask::Ground)) {
            
            std::string sourceGene = Platform.GetClipboardText();
            
            if (sourceGene.size() == 0) 
                return;
            
            // Remove end character
            //size_t pos = sourceGene.find(">");
            
            //if (pos != std::string::npos) 
            //    sourceGene.erase(pos, 1);
            
            std::vector<std::string> sourceArray = String.Explode(sourceGene, '<');
            
            if ((sourceArray[0][0] == 'g') & 
                (sourceArray[0][1] == 'e') & 
                (sourceArray[0][2] == 'n') & 
                (sourceArray[0][3] == 'e')) {
                
                GameObject* newActorObject = GameWorld.SpawnActor( hit.point.x, hit.point.y, hit.point.z );
                
                GameObject* hitObject = (GameObject*)hit.gameObject;
                Actor* newActor = newActorObject->GetComponent<Actor>();
                
                AI.genomes.mental.PreyBase( newActor );
                
                AI.genomes.InjectGenome(newActor, sourceArray[1]);
                
                newActor->SetAge( 900 + Random.Range(100, 800) );
                
                newActor->SetGeneticUpdateFlag();
                
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
        
    }
    
    
    */
    
    
    
    
    //
    // Camera controller movement
    //
    
    if (Engine.cameraController == nullptr) 
        return;
    
    float forceAccelerate = 0.0034f;
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

