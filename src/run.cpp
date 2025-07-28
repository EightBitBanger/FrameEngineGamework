#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>

bool isProfilerEnabled = false;

glm::vec3 force(0);
float forceDblTime=0;

Actor* actorInSights = nullptr;

void Run() {
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    GameWorld.Update();
    
    if (Engine.cameraController == nullptr) 
        return;
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    if (cameraPtr == nullptr) 
        return;
    glm::vec3 forward = cameraPtr->forward;
    glm::vec3 from = cameraPtr->transform.position;
    
    // Pick an actor
    if (Input.CheckMouseLeftPressed()) {
        Input.SetMouseLeftPressed(false);
        glm::vec3 fromHigh = cameraPtr->transform.position;
        
        GameWorld.RemoveDecor( Engine.sceneMain->camera->transform.position, Engine.sceneMain->camera->forward );
        
        
        
        /*
        if (Physics.Raycast(from, forward, 1000, hit, LayerMask::Actor)) {
            Actor* hitActor = (Actor*)hit.userData;
            if (hitActor != nullptr) 
                actorInSights = hitActor;
        } else {
            actorInSights = nullptr;
            for (unsigned int i=0; i < 24; i++) 
                Engine.console.WriteDialog(i, "");
        }
        */
    }
        
    // Murder an actor
    if (Input.CheckMouseRightPressed()) {
        //Input.SetMouseRightPressed(false);
        Hit hit;
        if (Physics.Raycast(from, forward, 1000, hit, LayerMask::Actor)) {
            Actor* hitActor = (Actor*)hit.userData;
            if (hitActor == actorInSights) {
                for (unsigned int i=0; i < 24; i++) 
                    Engine.console.WriteDialog(i, "");
            }
            
            hitActor->biological.health = 0.0f;
        }
    }
    
    
    
    
    // Spawn actors
    if (Input.CheckMouseMiddlePressed()) {
        //Input.SetMouseMiddlePressed(false);
        float randAmount = 8.0f;
        float xx = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
        float zz = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
        
        Hit hit;
        if (Physics.Raycast(from, forward, 100, hit, LayerMask::Ground)) {
            
            Actor* actor = GameWorld.SummonActor( glm::vec3(hit.point.x + xx, hit.point.y+5, hit.point.z + zz) );
            AI.genomes.presets.Human(actor);
            
            //float range = Random.Range(0, 100);
            
            //if (range >= 0  && range <  30)   {AI.genomes.presets.Sheep(actor);}
            //if (range >= 30 && range <  60)   {AI.genomes.presets.Bear(actor);}
            //if (range >= 60 && range <= 100)  {AI.genomes.presets.Human(actor);}
            
            /*
            unsigned int randomActor = Random.Range(0, 5);
            switch (randomActor) {
                default:
                case 0: AI.genomes.presets.Human(actor); break;
                case 1: AI.genomes.presets.Bovine(actor); break;
                case 2: AI.genomes.presets.Horse(actor); break;
                case 3: AI.genomes.presets.Sheep(actor); break;
                case 4: AI.genomes.presets.Bear(actor); break;
            }
            */
            
            actor->physical.SetAge( Random.Range(actor->physical.GetAdultAge(), actor->physical.GetAdultAge() * 2.0f) );
            actor->RebuildGeneticExpression();
            
            actor->isActive = true;
            actor->physical.UpdatePhysicalCollider();
        }
        
        /*
        // Spawn a tree example
        if (Physics.Raycast(from, forward, 1000, hit, LayerMask::Ground)) {
            
            GameObject* gameObject = (GameObject*)hit.gameObject;
            
            std::vector<std::string> name_pos = String.Explode(gameObject->name, '_');
            float posX = String.ToInt( name_pos[0] );
            float posZ = String.ToInt( name_pos[1] );
            
            Chunk* chunk = GameWorld.FindChunk(posX, posZ);
            
            if (chunk->gameObject != nullptr) {
                
                float chunkPosX = posX - hit.point.x + xx;
                float chunkPosY = hit.point.y;
                float chunkPosZ = posZ - hit.point.z + zz;
                
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
        */
        
    }
    
    
    
    //
    // Profiling
    //
    
    if (isProfilerEnabled) {
        glm::vec3 playerPos(0);
        if (Engine.sceneMain != nullptr) 
            if (Engine.sceneMain->camera != nullptr) 
                playerPos = Engine.sceneMain->camera->transform.position;
        
        Engine.console.WriteDialog(0, "player      " + Float.ToString( playerPos.x ) + ", " + Float.ToString( playerPos.y ) + ", " + Float.ToString( playerPos.z ));
        
        // Get chunk info
        glm::vec3 chunkPosition(0);
        Hit hit;
        if (Physics.Raycast(from, glm::vec3(0.0f, -1.0f, 0.0f), 1000, hit, LayerMask::Ground)) {
            GameObject* chunkObject = (GameObject*)hit.userData;
            chunkPosition = chunkObject->GetPosition();
            Engine.console.WriteDialog(1, "chunk       " + Float.ToString( chunkPosition.x ) + "_" + Float.ToString( chunkPosition.z ));
        }
        
        Engine.console.WriteDialog(10, "AI          " + Float.ToString( Profiler.profileActorAI ) );
        Engine.console.WriteDialog(11, "Engine      " + Float.ToString( Profiler.profileGameEngineUpdate ) );
        Engine.console.WriteDialog(12, "Renderer    " + Float.ToString( Profiler.profileRenderSystem ) );
        
        Engine.console.WriteDialog(13, "Draw calls      " + Int.ToString(Renderer.GetNumberOfDrawCalls()) );
        
        Engine.console.WriteDialog(14, "GameObjects     " + Int.ToString(Engine.GetNumberOfGameObjects()) );
        Engine.console.WriteDialog(15, "Components      " + Int.ToString(Engine.GetNumberOfComponents()) );
        
        Engine.console.WriteDialog(16, "MeshRenderers   " + Int.ToString(Renderer.GetNumberOfMeshRenderers()) );
        Engine.console.WriteDialog(17, "Meshes          " + Int.ToString(Renderer.GetNumberOfMeshes()) );
        Engine.console.WriteDialog(18, "Materials       " + Int.ToString(Renderer.GetNumberOfMaterials()) );
        Engine.console.WriteDialog(19, "RigidBodies     " + Int.ToString(Physics.world->getNbRigidBodies()) );
        Engine.console.WriteDialog(20, "Actors          " + Int.ToString(AI.GetNumberOfActors()) );
        Engine.console.WriteDialog(21, "Colliders       " + Int.ToString(Engine.mBoxCollider.size()) );
        
        
    }
    
    
    //
    // DEBUG - Show actor stats
    //
    
    if (actorInSights != nullptr && !isProfilerEnabled) {
        Engine.console.WriteDialog( 1, actorInSights->GetName() );
        Engine.console.WriteDialog( 2, Int.ToString( actorInSights->physical.GetAge() ) );
        
        Engine.console.WriteDialog( 4, "Active   " + Int.ToString( actorInSights->isActive ) );
        Engine.console.WriteDialog( 5, "Garbage  " + Int.ToString( actorInSights->isGarbage ) );
        
        Engine.console.WriteDialog( 7, "[ States ]" );
        
        switch (actorInSights->state.current) {
            case ActorState::State::None:    Engine.console.WriteDialog(8, "State  None"); break;
            case ActorState::State::Attack:  Engine.console.WriteDialog(8, "State  Attack"); break;
            case ActorState::State::Flee:    Engine.console.WriteDialog(8, "State  Flee"); break;
            case ActorState::State::Defend:  Engine.console.WriteDialog(8, "State  Defend"); break;
            case ActorState::State::Observe: Engine.console.WriteDialog(8, "State  Focus"); break;
            case ActorState::State::Look:    Engine.console.WriteDialog(8, "State  Look"); break;
            case ActorState::State::Breed:   Engine.console.WriteDialog(8, "State  Breed"); break;
            default: Engine.console.WriteDialog(8, "Mode   Unknown"); break;
        }
        
        switch (actorInSights->state.mode) {
            case ActorState::Mode::Idle:        Engine.console.WriteDialog(9, "Mode   Idle"); break;
            case ActorState::Mode::Sleeping:    Engine.console.WriteDialog(9, "Mode   Sleeping"); break;
            case ActorState::Mode::MoveRandom:  Engine.console.WriteDialog(9, "Mode   MoveRandom"); break;
            case ActorState::Mode::MoveTo:      Engine.console.WriteDialog(9, "Mode   MoveTo"); break;
            case ActorState::Mode::WalkTo:      Engine.console.WriteDialog(9, "Mode   WalkTo"); break;
            case ActorState::Mode::RunTo:       Engine.console.WriteDialog(9, "Mode   RunTo"); break;
            default: Engine.console.WriteDialog(9, "Mode   Unknown"); break;
        }
        
        Engine.console.WriteDialog( 11, "[ Counters ]" );
        
        Engine.console.WriteDialog( 12, "Attack    " + Int.ToString( actorInSights->counters.GetCoolDownAttack() ) );
        Engine.console.WriteDialog( 13, "Movement  " + Int.ToString( actorInSights->counters.GetCoolDownMovement() ) );
        Engine.console.WriteDialog( 14, "Observe   " + Int.ToString( actorInSights->counters.GetCoolDownObservation() ) );
        Engine.console.WriteDialog( 15, "Breeding  " + Int.ToString( actorInSights->counters.GetCoolDownBreeding() ) );
        
        Engine.console.WriteDialog( 17, "Distance to target   " + Int.ToString( actorInSights->navigation.GetDistanceToTarget() ) );
        Engine.console.WriteDialog( 18, "TargetPoint          " + Int.ToString( actorInSights->navigation.GetTargetPoint().x ) + ", " 
                                                                + Int.ToString( actorInSights->navigation.GetTargetPoint().y ) + ", "
                                                                + Int.ToString( actorInSights->navigation.GetTargetPoint().z ) );
        if (actorInSights->navigation.GetTargetActor() != nullptr) {
            Engine.console.WriteDialog( 19, "TargetActor          " + actorInSights->navigation.GetTargetActor()->GetName());
        } else {
            Engine.console.WriteDialog( 19, "TargetActor          nullptr");
        }
        
        
        /*
        Engine.console.WriteDialog( 19, "[ Vitality ]" );
        Engine.console.WriteDialog( 20, "Health   " + Int.ToString( actorInSights->biological.health ) );
        if (actorInSights->physical.GetSexualOrientation()) 
            Engine.console.WriteDialog( 21, "Male");
        else 
            Engine.console.WriteDialog( 21, "Female");
        
        Engine.console.WriteDialog( 22, "Mesh renderers   " + Int.ToString( actorInSights->genetics.GetNumberOfMeshRenderers() ) );
        */
        
    }
    
    
    //
    // Cast a ray from the player
    
    Hit hit;
    float heightStanding = 0.87f;
    
    // Prevent player controller from going under ground
    if (Physics.Raycast(from, glm::vec3(0.0f, -1.0f, 0.0f), 1000, hit, LayerMask::Ground)) {
        
        RigidBody* bodyPtr = Engine.cameraController->GetComponent<RigidBody>();
        
        rp3d::Transform transform = bodyPtr->getTransform();
        rp3d::Vector3 position = transform.getPosition();
        GameObject* chunkObject = (GameObject*)hit.userData;
        
        glm::vec3 chunkPosition = chunkObject->GetPosition();
        
        if (position.y < hit.point.y + heightStanding) {
            position.y = hit.point.y + heightStanding;
            transform.setPosition( position );
            bodyPtr->setTransform( transform );
        }
        
    }
    
    
    //
    // Camera controller movement
    
    if (Engine.cameraController == nullptr) 
        return;
    
    float forceAccelerate = 0.00145f;
    float forceDecelerate = 0.0117f;
    
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
        
        // Apply gravity
        //force -= mainCamera->up;
        
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
        float forceLength = glm::length(forceTotal);
        
        // Prevent drift
        if (forceLength < 0.0001f) 
            return;
        
        Engine.cameraController->AddForce(forceTotal.x, forceTotal.y, forceTotal.z);
        
        // Field of view zoom effect
        float fovPullback = forceLength * 40.0f;
        if (fovPullback > 4.0f) 
            fovPullback = 4.0f;
        Engine.sceneMain->camera->fov = 60 + fovPullback;
        
    }
    
    return;
}


void TickUpdate(void) {
    
    return;
}

