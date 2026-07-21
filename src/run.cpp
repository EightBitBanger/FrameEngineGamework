#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

bool isProfilerEnabled = false;

extern MeshRenderer* boundsRenderer;
extern Panel* selectedTab;

Actor* actorCheck  = nullptr;
Actor* actorTarget = nullptr;

float distance = 14.0f;
void HitDetection(void);



void Run() {
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera == nullptr) 
        return;
    glm::vec3 forward = mainCamera->forward;
    glm::vec3 from = mainCamera->transform.position;
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    GameWorld.Update();
    //Inventory.Update();
    
    if (Engine.cameraController == nullptr) 
        return;
    
    if (Input.CheckMouseLeftPressed()) {
        Input.SetMouseLeftPressed(false);
        
        boundsRenderer->isActive = false;
        actorTarget = AI.Raycast(from, forward, distance);
    }
    
    
    if (Input.CheckMouseMiddlePressed()) {
        //Input.SetMouseMiddlePressed(false);
        for (unsigned int i=0; i < 4; i++) {
            float randAmount = 8.0f;
            float xx = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
            float zz = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
            
            Hit hit;
            if (Physics.Raycast(from, forward, 100, hit, LayerMask::Ground)) {
                Actor* actor = GameWorld.SummonActor( glm::vec3(hit.point.x + xx, hit.point.y+5, hit.point.z + zz) );
                
                Color headColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
                Color limbColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
                Color bodyColor = Colors.Lerp(Colors.white, Colors.yellow, 0.2);
                
                bodyColor = Colors.Lerp(bodyColor, Colors.red, 0.5);
                bodyColor = Colors.Lerp(bodyColor, Colors.brown, 0.9);
                bodyColor = Colors.Lerp(bodyColor, Colors.black, 0.5);
                
                bodyColor *= Colors.MakeRandomGrayScale() * 0.4f;
                bodyColor *= Color(0.9f, 0.1f, 0.1f);
                
                //if (Input.CheckKeyCurrent(VK_T)) {
                    AI.genomes.presets.Dwarf(actor);
                    actor->CalculateBoundingRegionFromGenome();
                    
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    if (Random.Range(0, 100) > 50) {
                        
                        std::string weaponBuildBlade  = "build: 0.0`0.2`0.0: 0.01`0.5`0.2: 0.4`0.4`0.44";
                        std::string weaponBuildHandle = "build: 0.0`-0.39`0.0: 0.25`0.1`0.25: 0.01`0.01`0.03";
                        std::string itemSword = "name:ironsword, damage:1.2, defense:1.0," + weaponBuildBlade +","+ weaponBuildHandle;
                        
                        std::string weaponBuildWood = "build: 0.0`0.1`0.0: 0.2`0.4`0.2: 0.02`0.02`0.001";
                        std::string itemStick = "name:stick, damage:1.2, defense:1.0," + weaponBuildWood;
                        
                        if (Random.Range(0, 100) > 50) {
                            actor->inventory.GiveItem(itemSword);
                            actor->inventory.GiveItem(itemStick);
                        } else {
                            actor->inventory.GiveItem(itemStick);
                            actor->inventory.GiveItem(itemSword);
                        }
                    }
                    
                    actor->physical.SetAge( age );
                    
                    actor->memories.Add("kingdom", "Amethesian Empire");
                    actor->memories.Add("sentience", "quota:0.9");
                    actor->memories.Add("behavior", "curiosity:0.2, libido:0.08, social:0.1");
                    
                    if (sex) {
                        actor->memories.Add("Snort Fort Empire", "anger:0.7");
                    } else {
                        actor->memories.Add("Snort Fort Empire", "fear:0.1");
                    }
                    
                    actor->memories.Add("Dog", "fear:0.9");
                    
                    //
                    // Give the actor a weapon
                    /*
                    if (age > actor->physical.GetAdultAge() && sex) {
                        
                        actor->inventory.holdingRenderer = Engine.Create<MeshRenderer>();
                        actor->inventory.holdingRenderer->transform.scale = glm::vec3(0.08f, 1.1f, 0.08f);
                        actor->inventory.damageMul = 200.5f;
                        
                        actor->inventory.holdingRenderer->mesh = Resources.meshes.cube;
                        actor->inventory.holdingRenderer->material = Engine.Create<Material>();
                        actor->inventory.holdingRenderer->material->shader = Resources.shaders.color;
                        actor->inventory.holdingRenderer->material->ambient = Colors.white;
                        actor->inventory.holdingRenderer->material->diffuse = Colors.brown * 0.1f;
                        
                        actor->inventory.handPosition = glm::vec3(0.0f, -0.5f, 0.0f);
                        actor->inventory.handOffset   = glm::vec3(-0.01f, 0.3f, 0.0f);
                        actor->inventory.handRotation = glm::vec3(1.570795f, 0.0f, 0.0f);
                        actor->inventory.handScale    = glm::vec3(0.1f, 0.8f, 0.1f);
                        
                        Engine.sceneMain->AddMeshRendererToSceneRoot(actor->inventory.holdingRenderer);
                        
                    }
                    */
                //} else {
                    /*
                    AI.genomes.presets.HumanWhite(actor);
                    
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    actor->physical.SetAge( age );
                    
                    actor->memories.Add("kingdom", "Snort Fort Empire");
                    actor->memories.Add("sentience", "quota:0.9");
                    actor->memories.Add("behavior", "curiosity:0.07, libido:0.01, social:0.3");
                    
                    if (sex) {
                        actor->memories.Add("Amethesian Empire", "anger:0.7");
                    } else {
                        actor->memories.Add("Amethesian Empire", "fear:0.3");
                    }
                    
                    actor->memories.Add("Dog", "fear:0.9");
                    */
                    
                //}
                
                actor->RebuildGeneticExpression();
                actor->isActive = true;
            }
        }
        
    }
    
    
    
    
    
    
    //
    // Initiate the mesh outline shadow effect
    
    //
    // Ray cast an actor
    if (actorTarget == nullptr) {
        actorCheck = AI.Raycast(from, forward, distance);
        if (actorCheck != nullptr) {
            //Engine.console.textDialog[0]->color = Colors.green + (Colors.yellow * 0.09f);
            Engine.console.WriteDialog( 0, actorCheck->GetName());
            
            // Initiate bounding transform
            glm::vec3 boundsMax = actorCheck->GetBoundingBoxMax();
            glm::vec3 boundsMin = actorCheck->GetBoundingBoxMin();
            glm::vec3 boundsScale = (boundsMax - boundsMin) * 0.75f;
            
            boundsRenderer->mesh->ClearSubMeshes();
            boundsRenderer->mesh->AddCube(0, 0, 0, boundsScale.x, boundsScale.y, boundsScale.z, Colors.white);
            boundsRenderer->mesh->Load();
            
            glm::vec3 position = actorCheck->navigation.GetPosition() + glm::vec3(0, (boundsScale.y * 0.5f), 0);
            
            boundsRenderer->transform.SetPosition(position);
            boundsRenderer->transform.SetScale(boundsScale);
            boundsRenderer->transform.UpdateMatrix();
            boundsRenderer->isActive = true;
            
        } else {
            Engine.console.ClearDialog();
            boundsRenderer->isActive = false;
        }
    } else {
        
        Engine.console.textDialog[0]->color = Colors.green * Colors.yellow * 0.9f;
        Engine.console.WriteDialog( 0, "[" + actorTarget->GetName() + "]");
        
        Engine.console.WriteDialog( 1, "Age " + Int.ToString( actorTarget->physical.GetAge() ) );
        if (actorTarget->physical.GetSexualOrientation()) 
            Engine.console.WriteDialog( 2, "Male");
        else 
            Engine.console.WriteDialog( 2, "Female");
        
        Engine.console.textDialog[4]->color = Colors.green * Colors.yellow * 0.9f;
        Engine.console.WriteDialog( 4, "[ Vitality ]" );
        Engine.console.WriteDialog( 5, "Health   " + Int.ToString( actorTarget->biological.health ) );
        
        Engine.console.textDialog[7]->color = Colors.green * Colors.yellow * 0.9f;
        Engine.console.WriteDialog( 7, "[ Mental ]" );
        
        std::string mode = "unknown";
        
        switch (actorTarget->state.mode) {
            case ActorState::Mode::Idle:          mode = "idling"; break;
            case ActorState::Mode::MoveAttack:    mode = "Attacking"; break;
            case ActorState::Mode::MoveFlee:      mode = "Fleeing"; break;
            case ActorState::Mode::MoveRandom:    mode = "Wandering"; break;
            
            case ActorState::Mode::MoveHunting:   mode = "Hunting"; break;
            case ActorState::Mode::MoveSocialize: mode = "Socializing"; break;
            case ActorState::Mode::MoveBreed:     mode = "Breeding"; break;
            
            case ActorState::Mode::MoveTo:        mode = "Moving"; break;
            case ActorState::Mode::RunTo:         mode = "Running"; break;
            case ActorState::Mode::WalkTo:        mode = "Walking"; break;
        }
        Engine.console.WriteDialog( 8, "State  " + mode );
        
        // Emotional state
        
        Engine.console.WriteDialog( 10, "anger     " + Float.ToString(actorTarget->emotions.current.anger));
        Engine.console.WriteDialog( 11, "fear      " + Float.ToString(actorTarget->emotions.current.fear));
        Engine.console.WriteDialog( 12, "comfort   " + Float.ToString(actorTarget->emotions.current.comfort));
        Engine.console.WriteDialog( 13, "curiosity " + Float.ToString(actorTarget->emotions.current.curiosity));
        Engine.console.WriteDialog( 14, "fatigue   " + Float.ToString(actorTarget->emotions.current.fatigue));
        Engine.console.WriteDialog( 15, "libido    " + Float.ToString(actorTarget->emotions.current.libido));
        Engine.console.WriteDialog( 16, "stress    " + Float.ToString(actorTarget->emotions.current.stress));
        Engine.console.WriteDialog( 17, "social    " + Float.ToString(actorTarget->emotions.current.social));
        
        // Cool down counters
        
        Engine.console.WriteDialog( 19, "Attack        " + Float.ToString(actorTarget->counters.GetCoolDownAttack()));
        Engine.console.WriteDialog( 20, "Breeding      " + Float.ToString(actorTarget->counters.GetCoolDownBreeding()));
        Engine.console.WriteDialog( 21, "Movement      " + Float.ToString(actorTarget->counters.GetCoolDownMovement()));
        Engine.console.WriteDialog( 22, "Observe       " + Float.ToString(actorTarget->counters.GetCoolDownObservation()));
        Engine.console.WriteDialog( 23, "Socialize     " + Float.ToString(actorTarget->counters.GetCoolDownSocial()));
        
        // Update bounding box
        glm::vec3 boundsMax = actorTarget->GetBoundingBoxMax();
        glm::vec3 boundsMin = actorTarget->GetBoundingBoxMin();
        glm::vec3 boundsScale = (boundsMax - boundsMin) * 0.75f;
        
        glm::vec3 position = actorTarget->navigation.GetPosition() + glm::vec3(0, (boundsScale.y * 0.5f), 0);
        
        boundsRenderer->transform.SetPosition(position);
        boundsRenderer->transform.SetScale(boundsScale);
        boundsRenderer->transform.UpdateMatrix();
        boundsRenderer->isActive = true;
        
        if (Input.CheckKeyPressed(VK_K)) {
            
            actorTarget->biological.health = 0;
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
        
        Engine.console.textDialog[10]->color = Colors.green * 0.8f;
        Engine.console.WriteDialog(10, "[Profiler]");
        Engine.console.WriteDialog(11, "AI          " + Float.ToString( Profiler.profileActorAI ) );
        Engine.console.WriteDialog(12, "Engine      " + Float.ToString( Profiler.profileGameEngineUpdate ) );
        Engine.console.WriteDialog(13, "Renderer    " + Float.ToString( Profiler.profileRenderSystem ) );
        
        Engine.console.WriteDialog(15, "Draw calls      " + Int.ToString(Renderer.GetNumberOfDrawCalls()) );
        
        Engine.console.WriteDialog(17, "GameObjects     " + Int.ToString(Engine.GetNumberOfGameObjects()) );
        Engine.console.WriteDialog(18, "Components      " + Int.ToString(Engine.GetNumberOfComponents()) );
        
        Engine.console.WriteDialog(19, "MeshRenderers   " + Int.ToString(Renderer.GetNumberOfMeshRenderers()) );
        Engine.console.WriteDialog(10, "Meshes          " + Int.ToString(Renderer.GetNumberOfMeshes()) );
        Engine.console.WriteDialog(20, "Materials       " + Int.ToString(Renderer.GetNumberOfMaterials()) );
        Engine.console.WriteDialog(21, "RigidBodies     " + Int.ToString(Physics.world->getNbRigidBodies()) );
        Engine.console.WriteDialog(22, "Actors          " + Int.ToString(AI.GetNumberOfActors()) );
        
    }
    
    
    CameraControllerUpdate();
    
}


void TickUpdate(void) {
    
}





void HitDetection(void) {
    return;
    
    float distance = 9.0f;
    
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera == nullptr) 
        return;
    glm::vec3 forward = mainCamera->forward;
    glm::vec3 from = mainCamera->transform.position;
    
    if (actorCheck != nullptr) {
        glm::vec3 boundsMax = actorCheck->GetBoundingBoxMax();
        glm::vec3 boundsMin = actorCheck->GetBoundingBoxMin();
        glm::vec3 boundsScale = (boundsMax - boundsMin) * 0.75f;
        
        glm::vec3 position = actorCheck->navigation.GetPosition() + glm::vec3(0, (boundsScale.y * 0.5f), 0);;
        
        boundsRenderer->transform.SetPosition(position);
        boundsRenderer->transform.SetScale(boundsScale);
        boundsRenderer->transform.UpdateMatrix();
        boundsRenderer->isActive = true;
        
        return;
    }
    
    // Actors
    
    // Static objects
    /*
    const float hitMaxDistance = 4.0f;
    const float hitThreshold   = 0.9f;
    
    DecorationHitInfo info = GameWorld.QueryDecor(Engine.sceneMain->camera->transform.position, Engine.sceneMain->camera->forward, hitMaxDistance, hitThreshold);
    
    if (info.didHit && actorCheck == nullptr) {
        Engine.console.textDialog[0]->color = Colors.green * Colors.yellow * 0.9f;
        Engine.console.WriteDialog(0, "[" + info.type + "]");
        
        // Generate mesh outline shadow effect
        SubMesh& subMesh = GameWorld.mStaticMeshes[info.mesh];
        boundsRenderer->mesh->ClearSubMeshes();
        boundsRenderer->mesh->AddSubMesh(0.0f, 0.0f, 0.0f, subMesh, true);
        boundsRenderer->mesh->Load();
        
        glm::vec3 position = info.worldPosition;
        glm::vec3 scale    = info.scale * 1.009f;
        glm::vec3 rotation = info.rotation;
        
        boundsRenderer->transform.SetPosition(position);
        boundsRenderer->transform.SetOrientation( glm::radians(rotation) );
        boundsRenderer->transform.SetScale(scale);
        boundsRenderer->transform.UpdateMatrix();
        
        boundsRenderer->isActive = true;
    }
    */
    
    if (actorCheck == nullptr && actorTarget == nullptr) {
        if (boundsRenderer) 
            boundsRenderer->isActive = false;
    }
    
}
