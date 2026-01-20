#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

bool isProfilerEnabled = false;

glm::vec3 force(0);
float forceDblTime=0;

Actor* actorInSights = nullptr;

const float hitMaxDistance        = 4.5f;
const float hitThreshold          = 0.97f;
const float clickRepeatTimeout    = 250.0f;

float clickCooldownLeft  = 0.0;
float clickCooldownRight = 0.0;
bool isLeftClickHold     = false;
bool isRightClickHold    = false;

bool CheckCooldown(float nowMs, float& nextAllowedMs, float cooldownMs) {
    if (nowMs < nextAllowedMs) return false;
    nextAllowedMs = nowMs + cooldownMs;
    return true;
}

void MouseButtonLeft() {
    if (CheckCooldown(Time.Current(), clickCooldownLeft, clickRepeatTimeout)) {
        
        DecorationHitInfo info = GameWorld.QueryDecor(Engine.sceneMain->camera->transform.position, Engine.sceneMain->camera->forward, hitMaxDistance, hitThreshold);
        if (info.didHit) {
            
            if (GameWorld.RemoveDecor(Engine.sceneMain->camera->transform.position, Engine.sceneMain->camera->forward, hitMaxDistance, hitThreshold)) {
                
                Inventory.AddItem(info.type);
            }
        }
    }
}

void MouseButtonRight() {
    if (CheckCooldown(Time.Current(), clickCooldownRight, clickRepeatTimeout)) {
        
        if (!Inventory.CheckSlotEmpty(Inventory.GetSelectorIndex())) {
            std::string name = Inventory.QueryItem(Inventory.GetSelectorIndex());
            
            if (GameWorld.PlaceDecor(Engine.sceneMain->camera->transform.position, Engine.sceneMain->camera->forward, name, hitMaxDistance, hitThreshold)) 
                Inventory.RemoveItem(Inventory.GetSelectorIndex());
        }
    }
}

MeshRenderer* selectedMeshRenderer = nullptr;


void Run() {
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    GameWorld.Update();
    Inventory.Update();
    
    if (Engine.cameraController == nullptr) 
        return;
    
    Camera* cameraPtr = Engine.cameraController->GetComponent<Camera>();
    if (cameraPtr == nullptr) 
        return;
    
    glm::vec3 forward = cameraPtr->forward;
    glm::vec3 from = cameraPtr->transform.position;
    
    // Left click cooldown
    if (Input.CheckMouseLeftPressed()) {
        Input.SetMouseLeftPressed(false);
        isLeftClickHold = true;
        
    }
    
    if (isLeftClickHold) {
        MouseButtonLeft();
        
        
        
        
        GameObject* gameObject = Engine.Create<GameObject>();
        gameObject->AddComponent( Engine.CreateComponent<MeshRenderer>() );
        
        MeshRenderer* meshRenderer = gameObject->GetComponent<MeshRenderer>();
        meshRenderer->mesh = Engine.meshes.cube;
        meshRenderer->material = Engine.Create<Material>();
        meshRenderer->material->shader = Engine.shaders.color;
        
        meshRenderer->material->DisableCulling();
        meshRenderer->material->EnableShadowVolumePass();
        
        glm::vec3 randomColor = {Random.Range(0.0f, 1.0f), 0, Random.Range(0.0f, 1.0f)};
        
        meshRenderer->material->diffuse = randomColor * 0.4f;
        meshRenderer->material->ambient = Colors.gray;
        
        meshRenderer->material->EnableShadowVolumePass();
        meshRenderer->material->SetShadowVolumeColor(Colors.black);
        meshRenderer->material->SetShadowVolumeIntensityLow(0.3f);
        meshRenderer->material->SetShadowVolumeIntensityHigh(0.8f);
        meshRenderer->material->SetShadowVolumeColorIntensity(9.0f);
        meshRenderer->material->SetShadowVolumeLength(10.0f);
        
        meshRenderer->material->SetShadowVolumeAngleOfView(0.0f);
        
        glm::vec3 randomOffset = {Random.Range(-10, 10), 
                                  Random.Range(-10, 10), 
                                  Random.Range(-10, 10)};
        
        Transform* transform = gameObject->GetComponent<Transform>();
        transform->Translate( Engine.cameraController->GetPosition() + randomOffset );
        transform->Scale(2.0f, 2.0f, 2.0f);
        transform->UpdateMatrix();
        
        Engine.sceneMain->AddMeshRendererToSceneRoot(meshRenderer);
        
    }
    
    if (Input.CheckMouseLeftReleased()) {
        Input.SetMouseLeftReleased(false);
        isLeftClickHold = false;
        
        clickCooldownLeft = Time.Current();
    }
    
    // Right click cooldown
    if (Input.CheckMouseRightPressed()) {
        Input.SetMouseRightPressed(false);
        isRightClickHold = true;
    }
    
    if (isRightClickHold) {
        MouseButtonRight();
    }
    
    if (Input.CheckMouseRightReleased()) {
        Input.SetMouseRightReleased(false);
        isRightClickHold = false;
        
        clickCooldownRight = Time.Current();
    }
    
    // Scroll inventory items
    if (Input.mouseWheelDelta < 0.0f) 
        Inventory.NextSlot();
    if (Input.mouseWheelDelta > 0.0f) 
        Inventory.PrevSlot();
    
    DecorationHitInfo info = GameWorld.QueryDecor(Engine.sceneMain->camera->transform.position, Engine.sceneMain->camera->forward, hitMaxDistance, hitThreshold);
    
    if (info.didHit) {
        Engine.console.WriteDialog(0, info.type);
        
        // Initiate the mesh outline shadow effect
        if (!selectedMeshRenderer) {
            selectedMeshRenderer = Engine.Create<MeshRenderer>();
            selectedMeshRenderer->mesh = Engine.Create<Mesh>();
            selectedMeshRenderer->material = Engine.Create<Material>();
            
            selectedMeshRenderer->mesh->isShared = false;
            selectedMeshRenderer->mesh->SetPrimitive(MESH_LINE_LOOP);
            selectedMeshRenderer->material->isShared = false;
            selectedMeshRenderer->material->shader = Engine.shaders.colorUnlit;
            
            selectedMeshRenderer->material->ambient = Colors.black;
            
            Engine.sceneMain->AddMeshRendererToSceneRoot(selectedMeshRenderer);
        }
        
        // Generate mesh outline shadow effect
        SubMesh& subMesh = GameWorld.mStaticMeshes[info.mesh];
        selectedMeshRenderer->mesh->ClearSubMeshes();
        selectedMeshRenderer->mesh->AddSubMesh(0.0f, 0.0f, 0.0f, subMesh, true);
        selectedMeshRenderer->mesh->Load();
        
        glm::vec3 position = info.worldPosition;
        glm::vec3 scale    = info.scale * 1.009f;
        glm::vec3 rotation = info.rotation;
        
        selectedMeshRenderer->transform.SetPosition(position);
        selectedMeshRenderer->transform.SetOrientation(rotation * 3.14159f / 180.0f);
        
        selectedMeshRenderer->transform.SetScale(scale);
        selectedMeshRenderer->transform.UpdateMatrix();
        selectedMeshRenderer->isActive = true;
        
    } else {
        Engine.console.WriteDialog(0, "");
        if (selectedMeshRenderer) 
            selectedMeshRenderer->isActive = false;
    }
    
    
    
    if (Input.CheckMouseMiddlePressed()) {
        
        //Input.SetMouseMiddlePressed(false);
        float randAmount = 8.0f;
        float xx = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
        float zz = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
        
        Hit hit;
        if (Physics.Raycast(from, forward, 100, hit, LayerMask::Actor)) {
            Actor* actor = (Actor*)hit.userData;
            
            std::string genome = AI.genomes.ExtractGenome(actor);
            
            Engine.console.WriteDialog(0, "Actor hit");
            
            Platform.SetClipboardText(genome);
        }
        
        
        if (Physics.Raycast(from, forward, 100, hit, LayerMask::Ground)) {
            
            Actor* actor = GameWorld.SummonActor( glm::vec3(hit.point.x + xx, hit.point.y+5, hit.point.z + zz) );
            //AI.genomes.presets.Spider(actor);
            AI.genomes.presets.Human(actor);
            
            actor->physical.SetAge( actor->physical.GetAdultAge() );
            actor->physical.UpdatePhysicalCollider();
            
            actor->RebuildGeneticExpression();
            actor->isActive = true;
            
            
            /*
            glm::vec3 position = glm::vec3(hit.point.x, hit.point.y+1, hit.point.z);
            
            // Fire emitter
            Emitter* fireEmitter = Particle.CreateEmitter();
            fireEmitter->type = EmitterType::Point;
            fireEmitter->position  = position;
            fireEmitter->direction = glm::vec3(0.0f, 0.0f, 0.0f);
            fireEmitter->scale     = glm::vec3(0.1f, 0.04f, 0.1f);
            fireEmitter->scaleTo   = glm::vec3(1.0008f, 1.0008f, 1.0008f);
            
            fireEmitter->velocity = glm::vec3(0.0f, 0.01f, 0.0f);
            fireEmitter->velocityBias = 0.008f;
            
            fireEmitter->width = 0.1f;
            fireEmitter->height = 0.1f;
            
            fireEmitter->angle = 0.18f;
            fireEmitter->spread = 0.4f;
            
            fireEmitter->colorBegin = Colors.red;
            fireEmitter->colorEnd = Colors.yellow;
            
            fireEmitter->maxParticles = 20;
            fireEmitter->spawnRate = 20;
            
            fireEmitter->heightMinimum = GameWorld.world.waterLevel;
            
            Material* fireEmitterMaterial = fireEmitter->GetMaterial();
            
            //fireEmitterMaterial->EnableBlending();
            //fireEmitterMaterial->shader = Engine.shaders.water;
            
            
            // Smoke emitter test
            
            Emitter* smokeEmitter = Particle.CreateEmitter();
            smokeEmitter->type = EmitterType::Point;
            smokeEmitter->position = position;
            smokeEmitter->direction = glm::vec3(0.0f, 0.0f, 0.0f);
            smokeEmitter->scale     = glm::vec3(0.08f, 0.08f, 0.08f);
            smokeEmitter->scaleTo   = glm::vec3(1.005f, 1.0005f, 1.005f);
            
            float randomX = Random.Range(-1.0f, 1.0f) * 0.0024f;
            float randomZ = Random.Range(-1.0f, 1.0f) * 0.0024f;
            
            smokeEmitter->velocity = glm::vec3(randomX, 0.024f, randomZ);
            smokeEmitter->velocityBias = 0.02f;
            
            smokeEmitter->width = 4;
            smokeEmitter->height = 8;
            
            smokeEmitter->angle = 0.3f;
            smokeEmitter->spread = 0.5f;
            
            smokeEmitter->colorBegin = Colors.dkgray;
            smokeEmitter->colorEnd = Colors.gray;
            
            smokeEmitter->maxParticles = 40;
            smokeEmitter->spawnRate = 16;
            
            smokeEmitter->heightMinimum = GameWorld.world.waterLevel;
            
            Material* smokeEmitterMaterial = smokeEmitter->GetMaterial();
            
            //smokeEmitterMaterial->EnableBlending();
            //smokeEmitterMaterial->shader = Engine.shaders.water;
            */
        }
        
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
    
}


void TickUpdate(void) {
    
}

