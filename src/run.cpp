#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/Plugins/plugins.h>

bool isProfilerEnabled = false;

extern bool InspectorWindow(void);
extern MeshRenderer* boundsRenderer;
extern Panel* selectedTab;

Actor* actorCheck  = nullptr;
Actor* actorTarget = nullptr;

float distance = 14.0f;

// Random name generation
std::vector<std::string> germanPrefixes       = {"Eisen", "Stark", "Rosen", "Adel", "Wald", "Kaiser", "Nord", "Donner"};
std::vector<std::string> germanNames          = {"berg", "stein", "burg", "feld", "gard", "muth", "brand", "hart", "vogel", "wolf"};
std::vector<std::string> germanSuffixes       = {"er", "en", "ung", "ich", "heim", "stadt", "dorf", "mann", "dort"};
std::vector<std::string> germanCoreSuffixes   = {"art", "old", "hard", "rich", "mann", "mund", "vald", "mar", "st", "cht"};

std::vector<std::string> norsePrefixes        = {"Skar", "Varg", "Thor", "Grim", "Bjor", "Rune", "Hild", "Fen", "Krag", "Tyr"};
std::vector<std::string> norseNames           = {"val", "mund", "gard", "geir", "kell", "ulf", "dorn", "var", "brand", "skald"};
std::vector<std::string> norseSuffixes        = {"heim", "dottir", "son", "vir", "fjell", "rok", "vik"};
std::vector<std::string> norseCoreSuffixes    = {"ar", "ur", "ir", "ald", "ing", "or"};

NameGenerator generator;

void ClearAllDialogLines() {
    for (unsigned int i = 0; i < 32 + DIALOG_NUMBER_OF_ELEMENTS; i++) {
        Engine.console.WriteDialog(i, "");
    }
}

float worldTimeScale = 0.25f;
Sound* sound = nullptr;

void Run() {
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera == nullptr) 
        return;
    glm::vec3 forward = mainCamera->forward;
    glm::vec3 from = mainCamera->transform.position;
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    GameWorld.Update( GameWorld.threadTimer.GetCurrentDelta() );
    AI.SetTimeOfDay( Weather.GetTime() );
    
    if (Engine.cameraController == nullptr) 
        return;
    
    if (Input.CheckKeyPressed(VK_U)) {
        
        Hit hit;
        if (Physics.Raycast(from, forward, 100.0f, hit, LayerMask::Ground)) {
            
            for (unsigned int i=0; i < 16; i++) {
                float spread = 2.0f;
                float xx = hit.point.x + Random.Range(-spread, spread);
                float zz = hit.point.z + Random.Range(-spread, spread);
                if (Physics.Raycast(glm::vec3(xx, from.y - 100.0f, zz), glm::vec3(0, -1, 0), 1000.0f, hit, LayerMask::Ground)) {
                    float yy = hit.point.y;
                    
                    GameWorld.QueueDecorAt("wheat", glm::vec3(xx, yy, zz), glm::vec3(0, 0, 0));
                }
            }
        }
    }
    
    if (Input.CheckKeyPressed(VK_L)) {
        std::string filename = "gen.log";
        AI.DumpGenealogy(filename);
        Engine.console.Print("Genealogy dumped to file '" +filename+ "'");
    }
    
    
    if (Input.CheckKeyPressed(VK_G)) {
        
        //GameWorld.PlaceStructure(from, forward, "teepee", 100.0f, 0.1f);
    }
    
    if (Input.CheckKeyPressed(VK_Y)) {
        
        Hit hit;
        if (Physics.Raycast(from, forward, 100.0f, hit, LayerMask::Ground)) {
            Color color = Colors.green.ToVec3();
            
            GameWorld.SetWorldColorAdditive(hit.point, color);
        }
    }
    
    if (Input.CheckKeyPressed(VK_H)) {
        
        Hit hit;
        if (Physics.Raycast(from, forward, 100.0f, hit, LayerMask::Ground)) {
            Color color = Color(0.0f);
            
            GameWorld.SetWorldColorAdditive(hit.point, color);
        }
    }
    
    if (Input.CheckKeyPressed(VK_M)) {
        
        //GameWorld.PlaceStructure(from, forward, "campfire", 100.0f, 0.1f);
        
        std::string weaponBuildBlade  = "build: cube: 0.0,0.1,0.0: 0.001,0.45,0.02: 0.4,0.4,0.44";
        std::string weaponBuildHandle = "build: cube: 0.0,-0.39,0.0: 0.025,0.1,0.025: 0.01,0.01,0.03";
        std::string weaponBuildGuard  = "build: cube: 0.0,-0.28,0.0: 0.025,0.01,0.08: 0.2,0.1,0.01";
        std::string itemSword = "name:ironsword; damage:8.1; defense:1.0;" + weaponBuildBlade +";"+ weaponBuildHandle +";"+ weaponBuildGuard;
        
        GameWorld.PlacePickup(from, forward, 10.0f, itemSword);
        
    }
    
    // =============================
    // CTRL + Scroll wheel - Temporal manipulation
    
    const float worldTimeScaleMax  = 8.0f * 2;
    const float worldTimeScaleMin  = 0.5f;
    const float worldTimeStep      = 4.0f;
    
    if (Input.CheckKeyCurrent(VK_CONTROL) && Input.mouseWheelDelta != 0.0f) {
        if (Input.mouseWheelDelta > 0.0f) {
            worldTimeScale += worldTimeStep;
            if (worldTimeScale > worldTimeScaleMax) 
                worldTimeScale = worldTimeScaleMax;
        } else if (Input.mouseWheelDelta < 0.0f) {
            worldTimeScale -= worldTimeStep;
            if (worldTimeScale < worldTimeScaleMin) 
                worldTimeScale = worldTimeScaleMin;
        }
        AI.SetTimeScale(worldTimeScale);
        Weather.SetTimeScale(worldTimeScale);
    }
    
    // =============================
    // Q key pressed
    
    if (Input.CheckKeyPressed(VK_Q)) {
        Input.CheckKeyPressed(VK_Q);
        
        if (sound == nullptr) {
            sound = Audio.CreateSound();
            sound->doContinuousStream = false;
            sound->sample = Audio.CreateAudioSample();
            
            VocalParameters bark;

    // Snappier duration (~150ms total)
    bark.seconds        = 0.15f; 
    bark.attackSeconds  = 0.005f; // Fast, click-free attack
    bark.holdSeconds    = 0.025f; // Crisp transient peak
    bark.releaseSeconds = 0.120f; // Quick acoustic tail

    // Higher pitch range (mid-size dog / sharp bark)
    bark.pitchStartHz   = 580.0f; // High attack punch
    bark.pitchEndHz     = 260.0f; // Settles into lower body
    bark.vibratoRateHz  = 0.0f;
    bark.vibratoDepthHz = 0.0f;
    bark.jitterHz       = 6.0f;

    // Timbre & distortion
    bark.brightness     = 0.78f;  // Extra high-order harmonics
    bark.growl          = 0.35f;  // Balanced throat drive without mud
    bark.noiseAmount    = 0.22f;  // Air release

    // Formants shifted up to match the higher pitch profile
    bark.formant1Hz     = 700.0f;
    bark.bandwidth1Hz   = 130.0f;
    bark.formant2Hz     = 1550.0f;
    bark.bandwidth2Hz   = 210.0f;
    bark.formant3Hz     = 2800.0f;
    bark.bandwidth3Hz   = 320.0f;

    bark.gain           = 0.95f;
    bark.seed           = 0;
            Audio.presets.RenderAnimalVocal(sound->sample, bark);
            
        }
        
        Audio.Play(sound);
        
    }
    
    // =============================
    // E key pressed
    
    if (Input.CheckKeyPressed(VK_E)) {
        Input.CheckKeyPressed(VK_E);
        Actor* targetActor = AI.Raycast(from, forward, 100.0f);
        
        if (targetActor !=  nullptr) {
            //targetActor->isActive = false;
            
            /*
            const glm::vec3& position = targetActor->navigation.GetPosition();
            std::string anchor = Float.ToString(position.x)+","+Float.ToString(position.y)+","+Float.ToString(position.z);
            targetActor->memories.Add("trade", "farmer:"+anchor);
            
            Engine.console.Print(targetActor->GetName() + " is now a farmer");
            */
        }
    }
    
    extern bool IsActorInspectorOpen();
    extern bool IsGeneEditorOpen();
    extern void OpenActorInspector(Actor* target);
    extern void OpenGeneEditor(Actor* target);
    
    // =============================
    // LEFT mouse pressed
    
    static Actor* lastClickedActor = nullptr;
    static double lastClickTime = 0.0;
    const double doubleClickThreshold = 300.0; // Milliseconds
    
    if (Input.CheckMouseLeftPressed()) {
        bool isUIOpen = IsActorInspectorOpen() || IsGeneEditorOpen();
        if ((isUIOpen && ImGui::GetIO().WantCaptureMouse) || Platform.isPaused) {
            return;
        }
        
        Input.SetMouseLeftPressed(false);
        Actor* newTarget = AI.Raycast(from, forward, 100.0f);
        
        double currentTime = Time.Current();
        
        if (!isUIOpen && newTarget != nullptr) {
            if (newTarget == lastClickedActor && (currentTime - lastClickTime) <= doubleClickThreshold) {
                if (Input.CheckKeyCurrent(VK_CONTROL)) {
                    OpenGeneEditor(newTarget);
                } else {
                    OpenActorInspector(newTarget);
                }
                lastClickedActor = nullptr;
                lastClickTime = 0.0;
            } else {
                lastClickedActor = newTarget;
                lastClickTime = currentTime;
            }
        } else {
            lastClickedActor = nullptr;
        }
    }
    
    // =============================
    // RIGHT mouse pressed
    
    if (Input.CheckMouseRightPressed()) {
        Input.ClearMouseRight();
        
        Actor* actor = AI.Raycast(from, forward, 10.0f);
        if (actor != nullptr) {
            
            //AI.genomes.ExposeToRadiation(actor, 2.0f);
            
            //
            // Phenotypic modification test
            /*
            unsigned int numberOfPhenes = actor->genetics.GetNumberOfPhens();
            for (unsigned int i=0; i < numberOfPhenes; i++) {
                Phen phen = actor->genetics.GetPhenFromPhenotype(i);
                Color infected = Colors.green;
                Color targetColor = infected * Colors.Range(Colors.green * 0.01f, Colors.green * 0.1f);
                phen.color.x = targetColor.r;
                phen.color.y = targetColor.g;
                phen.color.z = targetColor.b;
                actor->genetics.SetPhenFromPhenotype(i, phen);
                
                actor->RebuildGeneticExpression();
                Engine.console.Print("Infected");
            }
            */
        }
        
        //GameWorld.RemoveDecor(from, forward, 4.0f, 0.1f);
    }
    
    
    // =============================
    // MIDDLE mouse pressed
    
    if (Input.CheckMouseMiddlePressed()) {
        Input.ClearMouseMiddle();
        
        if (ImGui::GetIO().WantCaptureMouse || Platform.isPaused) 
            return;
        
        std::string family = generator.GenerateIncarnation(norsePrefixes, norseNames, norseSuffixes, norseCoreSuffixes);
        
        bool orientation = false;
        for (unsigned int i=0; i < 2; i++) {
            
            float randAmount = 4.0f;
            float xx = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
            float zz = Random.Range(0.0f, randAmount) - Random.Range(0.0f, randAmount);
            
            Hit hit;
            if (Physics.Raycast(from, forward, 100, hit, LayerMask::Ground)) {
                glm::vec3 position = glm::vec3(hit.point.x + xx, hit.point.y+5, hit.point.z + zz);
                
                Actor* actor = GameWorld.SummonActor(position);
                
                AI.genomes.presets.HumanWhite(actor);
                actor->CalculateBoundingRegionFromGenome();
                
                std::string name = generator.GenerateFirstName(germanPrefixes, germanNames, germanSuffixes, germanCoreSuffixes);
                std::string family = generator.GenerateIncarnation(germanPrefixes, germanNames, germanSuffixes, germanCoreSuffixes);
                
                actor->physical.SetSexualOrientation(orientation);
                orientation = !orientation;
                
                std::string anchor = Float.ToString(position.x)+","+Float.ToString(position.y)+","+Float.ToString(position.z);
                
                actor->physical.SetAge( actor->physical.GetAdultAge() );
                actor->memories.Add("name",   name);
                actor->memories.Add("family", family);
                
                actor->memories.Add("sentience", "quota:0.9");
                actor->memories.Add("behavior", "curiosity:0.24 ^ libido:0.07 ^ social:0.1");
                
                if (actor->physical.GetSexualOrientation() == true) {
                    
                    actor->memories.Add("trade", "farmer:"+anchor);
                    
                    /*
                    if (Random.Range(0, 100) > 10) {
                        if (Random.Range(0, 100) > 20) {
                            actor->memories.Add("trade", "farmer:"+anchor);
                        } else {
                            actor->memories.Add("trade", "carpenter");
                        }
                    } else {
                        actor->memories.Add("trade", "none");
                    }
                    */
                }
                
                // Kingdom affiliation
                actor->memories.Add("kingdom", "Amethesian Empire");
                
                if (orientation) 
                {actor->memories.Add("Snort Fort Empire", "anger:0.9");} else 
                {actor->memories.Add("Snort Fort Empire", "fear:0.9 ^ anger:0.2");}
                
                /*
                Color headColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
                Color limbColor = Colors.Lerp(Colors.white, Colors.yellow, 0.5) * 0.5f + (Colors.red * 0.087f);
                Color bodyColor = Colors.Lerp(Colors.white, Colors.yellow, 0.2);
                
                bodyColor = Colors.Lerp(bodyColor, Colors.red, 0.5);
                bodyColor = Colors.Lerp(bodyColor, Colors.brown, 0.9);
                bodyColor = Colors.Lerp(bodyColor, Colors.black, 0.5);
                bodyColor *= Colors.MakeRandomGrayScale() * 0.4f;
                bodyColor *= Color(0.9f, 0.1f, 0.1f);
                
                std::string weaponBuildBlade  = "build: cube: 0.0,0.3,0.0: 0.001,0.5,0.02: 0.4,0.4,0.44";
                std::string weaponBuildHandle = "build: cube: 0.0,-0.25,0.0: 0.025,0.1,0.025: 0.01,0.01,0.03";
                std::string weaponBuildGuard  = "build: cube: 0.0,-0.15,0.0: 0.025,0.01,0.08: 0.2,0.1,0.01";
                std::string itemSword = "name:ironsword; damage:8.1; defense:1.0;" + weaponBuildBlade +";"+ weaponBuildHandle +";"+ weaponBuildGuard;
                
                std::string weaponBuildWood = "build: cube: 0.0,0.1,0.0: 0.02,0.3,0.02: 0.02,0.02,0.001";
                std::string itemStick = "name:stick; damage:2.2; defense:1.0;" + weaponBuildWood;
                
                if (Input.CheckKeyCurrent(VK_T)) {
                    AI.genomes.presets.Dwarf(actor);
                    actor->CalculateBoundingRegionFromGenome();
                    
                    std::string name = generator.GenerateFirstName(norsePrefixes, norseNames, norseSuffixes, norseCoreSuffixes);
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    actor->physical.SetAge( age );
                    actor->memories.Add("name",   name);
                    actor->memories.Add("family", family);
                    actor->memories.Add("kingdom",   "Snort Fort Empire");
                    actor->memories.Add("sentience", "quota:0.9");
                    actor->memories.Add("behavior",  "curiosity:0.24 ^ libido:0.05 ^ social:0.1");
                    
                    actor->memories.Add("culture",   "");
                    actor->memories.Add("religion",  "");
                    
                    if (sex) {actor->memories.Add("Amethesian Empire", "anger:0.9");} else 
                             {actor->memories.Add("Amethesian Empire", "fear:0.9 ^ anger:0.2");}
                    
                } else if (Input.CheckKeyCurrent(VK_P)) {
                    AI.genomes.presets.Spider(actor);
                    actor->CalculateBoundingRegionFromGenome();
                    
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    actor->memories.Add("behavior", "curiosity:0.14 ^ libido:0.08 ^ social:0.08");
                    actor->physical.SetAge( age );
                    
                } else if (Input.CheckKeyCurrent(VK_O)) {
                    AI.genomes.presets.Bovine(actor);
                    actor->CalculateBoundingRegionFromGenome();
                    
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    actor->physical.SetAge( age );
                    
                    actor->memories.Add("behavior",  "curiosity:0.24 ^ libido:0.01 ^ social:0.1");
                } else {
                    AI.genomes.presets.HumanWhite(actor);
                    actor->CalculateBoundingRegionFromGenome();
                    
                    std::string name = generator.GenerateFirstName(germanPrefixes, germanNames, germanSuffixes, germanCoreSuffixes);
                    std::string family = generator.GenerateIncarnation(germanPrefixes, germanNames, germanSuffixes, germanCoreSuffixes);
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    std::string anchor = Float.ToString(position.x)+","+Float.ToString(position.y)+","+Float.ToString(position.z);
                    
                    actor->physical.SetAge( age );
                    actor->memories.Add("name",   name);
                    actor->memories.Add("family", family);
                    
                    actor->memories.Add("sentience", "quota:0.9");
                    actor->memories.Add("behavior", "curiosity:0.24 ^ libido:0.07 ^ social:0.1");
                    
                    // Occupation
                    if (actor->physical.GetSexualOrientation() == true) {
                        
                        actor->memories.Add("trade", "farmer:"+anchor);
                        
                        if (Random.Range(0, 100) > 10) {
                            if (Random.Range(0, 100) > 20) {
                                actor->memories.Add("trade", "farmer:"+anchor);
                            } else {
                                actor->memories.Add("trade", "carpenter");
                            }
                        } else {
                            actor->memories.Add("trade", "none");
                        }
                        
                    }
                    
                    // Kingdom affiliation
                    actor->memories.Add("kingdom", "Amethesian Empire");
                    
                    if (sex) {
                        actor->memories.Add("Snort Fort Empire", "anger:0.9");
                    } else {
                        actor->memories.Add("Snort Fort Empire", "fear:0.9 ^ anger:0.2");
                    }
                }
                */
                actor->RebuildGeneticExpression();
                actor->isActive = true;
            }
        }
    }
    
    // =============================
    // ESC key - menu back
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        if (IsGeneEditorOpen()) {
            CloseGeneEditor();
        } else if (IsActorInspectorOpen()) {
            CloseActorInspector();
        } else {
            // Prevent closing the main menu if no world is loaded
            if (Platform.isPaused && GameWorld.world.name.empty()) {
                return;
            }
            
            Platform.Pause();
            
            if (Platform.isPaused) {
                Platform.ShowMouseCursor();
                if (Engine.sceneMain != nullptr && Engine.sceneMain->camera != nullptr) {
                    Engine.sceneMain->camera->DisableMouseLook();
                }
                Input.ClearKeys();
            } else {
                if (Engine.sceneMain != nullptr && Engine.sceneMain->camera != nullptr) {
                    Engine.sceneMain->camera->EnableMouseLook();
                    Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
                }
                Platform.HideMouseCursor();
                Time.Update();
                PhysicsTime.Update();
            }
        }
    }
    
    // =============================
    // Shift zoom
    
    static float orbitDistance = 2.5f; // Start closer to the target
    const float minOrbitDistance = 1.0f;
    const float maxOrbitDistance = 10.0f;
    const float zoomSpeed = 0.5f;
    
    if (actorTarget != nullptr) {
        if (Input.CheckKeyPressed(VK_SHIFT)) {
            Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
        }
        
        if (Input.CheckKeyCurrent(VK_SHIFT)) {
            Engine.sceneMain->camera->EnableMouseLook();
            if (Input.mouseWheelDelta != 0.0f) {
                orbitDistance -= Input.mouseWheelDelta * zoomSpeed;
                orbitDistance = glm::clamp(orbitDistance, minOrbitDistance, maxOrbitDistance);
                Input.mouseWheelDelta = 0.0f;
            }
        } else {
            if (Input.CheckKeyReleased(VK_SHIFT)) {
                Engine.sceneMain->camera->DisableMouseLook();
            }
        }
        
        // Pass the zoom distance to centerStandOff
        Engine.sceneMain->camera->centerStandOff = orbitDistance;
        
        // Center on the actor's torso/chest (~1.0f Y offset instead of 0.1f feet level)
        glm::vec3 targetCenter = actorTarget->navigation.GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f);
        
        if (Engine.cameraController != nullptr) {
            rp3d::RigidBody* rigidBody = Engine.cameraController->GetComponent<rp3d::RigidBody>();
            if (rigidBody != nullptr) {
                rp3d::Transform bodyTransform = rigidBody->getTransform();
                bodyTransform.setPosition(rp3d::Vector3(targetCenter.x, targetCenter.y, targetCenter.z));
                rigidBody->setTransform(bodyTransform);
                rigidBody->setLinearVelocity(rp3d::Vector3(0.0f, 0.0f, 0.0f));
            }
            Engine.cameraController->SetPosition(targetCenter);
        }
        
        Engine.sceneMain->camera->transform.position = targetCenter;
        Engine.sceneMain->camera->transform.UpdateMatrix();
    } else {
        CameraControllerUpdate();
    }
    
    
    // Raycast check & Inspector Bounding Box
    if (actorTarget == nullptr) {
        actorCheck = AI.Raycast(from, forward, distance);
        if (actorCheck != nullptr) {
            Engine.console.WriteDialog(0, actorCheck->GetName());
            
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
            boundsRenderer->isActive = false;
            Engine.console.WriteDialog(0, "");
        }
    } else {
        // Update active target bounding box
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
    
    // Profiling section
    if (isProfilerEnabled) {
        glm::vec3 playerPos(0);
        if (Engine.sceneMain != nullptr && Engine.sceneMain->camera != nullptr) 
            playerPos = Engine.sceneMain->camera->transform.position;
        
        Engine.console.WriteDialog(0, "player      " + Float.ToString( playerPos.x ) + ", " + Float.ToString( playerPos.y ) + ", " + Float.ToString( playerPos.z ));
        
        glm::vec3 chunkPosition(0);
        Hit hit;
        if (Physics.Raycast(from, glm::vec3(0.0f, -1.0f, 0.0f), 1000, hit, LayerMask::Ground)) {
            GameObject* chunkObject = (GameObject*)hit.userData;
            chunkPosition = chunkObject->GetPosition();
            Engine.console.WriteDialog(1, "chunk       " + Float.ToString( chunkPosition.x ) + "_" + Float.ToString( chunkPosition.z ));
        }
        
        Engine.console.textDialog[8]->color = Colors.green * 0.8f;
        Engine.console.textDialog[9]->color = Colors.green * 0.8f;
        Engine.console.WriteDialog( 8, "Draw calls      " + Int.ToString(Renderer.GetNumberOfDrawCalls()) );
        Engine.console.WriteDialog( 9, "FPS             " + Int.ToString(Renderer.GetFPS()) );
        Engine.console.WriteDialog(11, "GameObjects     " + Int.ToString(Engine.GetNumberOfGameObjects()) );
        Engine.console.WriteDialog(12, "Components      " + Int.ToString(Engine.GetNumberOfComponents()) );
        Engine.console.WriteDialog(14, "MeshRenderers   " + Int.ToString(Renderer.GetNumberOfMeshRenderers()) );
        Engine.console.WriteDialog(15, "Meshes          " + Int.ToString(Renderer.GetNumberOfMeshes()) );
        Engine.console.WriteDialog(16, "Materials       " + Int.ToString(Renderer.GetNumberOfMaterials()) );
        Engine.console.WriteDialog(17, "RigidBodies     " + Int.ToString(Physics.world->getNbRigidBodies()) );
        Engine.console.WriteDialog(18, "Actors          " + Int.ToString(AI.GetNumberOfActors()) );
        
        Engine.console.textDialog[20]->color = Colors.green * 0.8f;
        Engine.console.WriteDialog(20, "[Profiler]");
        
        std::map<std::string, float> profs = Profiler.GetProfiles();
        
        unsigned int index=0;
        for (const std::pair<std::string, float>& pair : profs) {
            if (pair.second != 0.0f) Engine.console.WriteDialog(21 + index, pair.first +"     "+ Float.ToString( pair.second ) );
            index++; if (index > 8) break;
        }
        Profiler.Reset();
    }
}

void TickUpdate(void) {}
