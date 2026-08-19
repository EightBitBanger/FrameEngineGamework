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

// Random name generation
std::vector<std::string> africanPrefixes      = {"Abo", "Boma", "Kibo", "Suli", "Zuba", "Sim", "Masa", "Tari", "Kaza", "Olu"};
std::vector<std::string> africanNames         = {"mani", "kosi", "diko", "tani", "beke", "jaro", "mbo", "gona", "wasi", "femi"};
std::vector<std::string> africanSuffixes      = {"ani", "ya", "o", "ika", "ele", "ka", "ba", "wa"};
std::vector<std::string> africanCoreSuffixes  = {"mbe", "kwa", "zi", "ndo", "ra", "sa", "bo", "ni"};

std::vector<std::string> germanPrefixes       = {"Eisen", "Stark", "Rosen", "Adel", "Wald", "Kaiser", "Nord", "Donner"};
std::vector<std::string> germanNames          = {"berg", "stein", "burg", "feld", "gard", "muth", "brand", "hart", "vogel", "wolf"};
std::vector<std::string> germanSuffixes       = {"er", "en", "ung", "ich", "heim", "stadt", "dorf", "mann", "dort"};
std::vector<std::string> germanCoreSuffixes   = {"art", "old", "hard", "rich", "mann", "mund", "vald", "mar", "st", "cht"};

std::vector<std::string> russianPrefixes      = {"Vlad", "Yar", "Kaz", "Volk", "Mir", "Bel", "Stan", "Bor", "Chern", "Sviato"};
std::vector<std::string> russianNames         = {"islav", "omir", "gor", "dmit", "vost", "kass", "rog", "zar", "drak", "slov"};
std::vector<std::string> russianSuffixes      = {"ov", "ev", "ski", "sky", "in", "ich", "enko", "off", "kov"};
std::vector<std::string> russianCoreSuffixes  = {"ov", "ev", "ich", "sky", "in", "ir", "mir", "nik", "ovitch", "ya"};

std::vector<std::string> norsePrefixes        = {"Skar", "Varg", "Thor", "Grim", "Bjor", "Rune", "Hild", "Fen", "Krag", "Tyr"};
std::vector<std::string> norseNames           = {"val", "mund", "gard", "geir", "kell", "ulf", "dorn", "var", "brand", "skald"};
std::vector<std::string> norseSuffixes        = {"heim", "dottir", "son", "vir", "fjell", "rok", "vik"};
std::vector<std::string> norseCoreSuffixes    = {"ar", "ur", "ir", "ald", "ing", "or"};

std::vector<std::string> aztecPrefixes        = {"Xol", "Quetz", "Teo", "Itz", "Acat", "Chimal", "Cuit", "Zac", "Ix", "Ten"};
std::vector<std::string> aztecNames           = {"tl", "coatl", "popoca", "tlan", "pilli", "xoch", "pan", "tepetl", "mitl", "tli"};
std::vector<std::string> aztecSuffixes        = {"pec", "tepetl", "tli", "ca", "zin", "pan"};
std::vector<std::string> aztecCoreSuffixes    = {"xil", "zan", "cal", "ic", "ox", "atl"};

std::vector<std::string> scifiPrefixes        = {"Vex", "Kryo", "Synth", "Nova", "Zero", "Cy", "Aero", "Pulse", "Omni", "Xen"};
std::vector<std::string> scifiNames           = {"core", "byte", "tron", "net", "matrix", "grid", "tech", "node", "link", "drive"};
std::vector<std::string> scifiSuffixes        = {" Prime", " System", " Protocol", " IX", " MK-4", " Alpha"};
std::vector<std::string> scifiCoreSuffixes    = {"ex", "ix", "ox", "ion", "or", "yze"};

std::vector<std::string> egyptianPrefixes     = {"Khen", "Amon", "Sekh", "Osir", "Anu", "Sob", "Thoth", "Hathor", "Rams", "Nefer"};
std::vector<std::string> egyptianNames        = {"tep", "hetep", "kare", "re", "siris", "hotep", "ptah", "mose", "amon", "khet"};
std::vector<std::string> egyptianSuffixes     = {"-Ra", "-Amun", "-Aton", " the Blessed", " of the Nile"};
std::vector<std::string> egyptianCoreSuffixes = {"is", "es", "ut", "kh", "en", "is"};

NameGenerator generator;

void ClearAllDialogLines() {
    for (unsigned int i = 0; i < 32 + DIALOG_NUMBER_OF_ELEMENTS; i++) {
        Engine.console.WriteDialog(i, "");
    }
}

void Run() {
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera == nullptr) 
        return;
    glm::vec3 forward = mainCamera->forward;
    glm::vec3 from = mainCamera->transform.position;
    
    // Update plug-in systems
    Weather.Update();
    Particle.Update();
    GameWorld.Update(Time.delta);
    
    if (Engine.cameraController == nullptr) 
        return;
    if (Input.CheckKeyPressed(VK_M)) {
        std::vector<std::pair<std::string, glm::vec3>> names = GameWorld.QueryPickupNames(from, 3.0f);
        
        for (unsigned int i=0; i < names.size(); i++) {
            Engine.console.Print(names[i].first);
        }
    }
    
    // Left-click target selection handling
    if (Input.CheckMouseLeftPressed()) {
        Input.SetMouseLeftPressed(false);
        Actor* newTarget = AI.Raycast(from, forward, 100.0f);
        
        // If selection changed (including deselecting / clicking away into nullptr)
        if (newTarget != actorTarget) {
            actorTarget = newTarget;
            ClearAllDialogLines();
            return;
        }
        
        
        if (newTarget == nullptr) {
            
            std::string weaponBuildBlade  = "build: 0.0,0.2,0.0: 0.001,0.5,0.02: 0.4,0.4,0.44";
            std::string weaponBuildHandle = "build: 0.0,-0.39,0.0: 0.025,0.1,0.025: 0.01,0.01,0.03";
            std::string weaponBuildGuard  = "build: 0.0,-0.28,0.0: 0.025,0.01,0.08: 0.2,0.1,0.01";
            std::string itemSword = "name:ironsword; damage:8.1; defense:1.0;" + weaponBuildBlade +";"+ weaponBuildHandle +";"+ weaponBuildGuard;
            
            GameWorld.PlacePickup(from, forward, 10.0f, itemSword);
        }
        
    }
    
    if (Input.CheckMouseRightPressed()) {
        
        std::string collected;
        if (GameWorld.RemovePickup(from, forward, 10.0f, collected)) {
            
            Engine.console.Print("item  " + collected);
        }
        
        std::string structureName = "";
        if (Input.CheckKeyCurrent(VK_H)) {structureName = "teepee"; Input.ClearMouseRight();}
        if (Input.CheckKeyCurrent(VK_C)) {structureName = "campfire"; Input.ClearMouseRight();}
        if (Input.CheckKeyCurrent(VK_K)) {structureName = "KILL_TARGET";}
        if (Input.CheckKeyCurrent(VK_T)) {structureName = "DECOR_DESTROY";}
        
        if (structureName != "DECOR_DESTROY") {
            
            GameWorld.RemoveDecor(from, forward, 10.0f, 1.0f);
            
        } else if (structureName != "KILL_TARGET") {
            Actor* actor = AI.Raycast(from, forward, 100.0f);
            
            if (actor != nullptr) {
                actor->biological.health = 0;
            }
            
        } else if (structureName != "") {
            Hit hit;
            if (Physics.Raycast(from, forward, 100, hit, LayerMask::Ground)) {
                glm::vec3 pos(0);
                
                //for (unsigned int i=0; i < 8; i++) {
                    
                    //float rx = Random.Range(-4.0f, 4.0f);
                    //float ry = 0.0f;
                    //float rz = Random.Range(-4.0f, 4.0f);
                    //pos = glm::vec3(rx, ry, rz);
                    
                    const float grid = 1.0f;
                    const glm::vec3 gridOrigin(0.5f, 0.5f, 0.5f);
                    hit.point = SnapAxes(hit.point, glm::bvec3(true, false, true), grid, gridOrigin);
                    
                    if (!GameWorld.PlaceStructure(hit.point + pos, glm::vec3(0, -1, 0), structureName, 100.0f, 0.01f)) {
                        Engine.console.Print("Structure '"+structureName+"' does not exist");
                    }
                    
                //}
                
            }
        }
        
        /*
        Actor* actor = AI.Raycast(from, forward, 100.0f);
        if (actor != nullptr) {
            actor->emotions.current.anger = 1.0f;
            actor->memories.SetEmotion(TriggerType::Anger, 1.0f);
        }
        */
        
    }
    
    if (Input.CheckMouseMiddlePressed()) {
        //Input.ClearMouseMiddle();
        
        for (unsigned int i=0; i < 1; i++) {
            float randAmount = 4.0f;
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
                
                std::string weaponBuildBlade  = "build: 0.0,0.3,0.0: 0.001,0.5,0.02: 0.4,0.4,0.44";
                std::string weaponBuildHandle = "build: 0.0,-0.25,0.0: 0.025,0.1,0.025: 0.01,0.01,0.03";
                std::string weaponBuildGuard  = "build: 0.0,-0.15,0.0: 0.025,0.01,0.08: 0.2,0.1,0.01";
                std::string itemSword = "name:ironsword; damage:8.1; defense:1.0;" + weaponBuildBlade +";"+ weaponBuildHandle +";"+ weaponBuildGuard;
                
                std::string weaponBuildWood = "build: 0.0,0.1,0.0: 0.02,0.3,0.02: 0.02,0.02,0.001";
                std::string itemStick = "name:stick; damage:2.2; defense:1.0;" + weaponBuildWood;
                
                if (Input.CheckKeyCurrent(VK_T)) {
                    
                    // Dwarf
                    
                    AI.genomes.presets.Dwarf(actor);
                    actor->CalculateBoundingRegionFromGenome();
                    
                    std::string name = generator.GenerateIncarnation(norsePrefixes, norseNames, norseSuffixes, norseCoreSuffixes);
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    actor->physical.SetAge( age );
                    
                    actor->memories.Add("name", name);
                    actor->memories.Add("kingdom", "Snort Fort Empire");
                    actor->memories.Add("sentience", "quota:0.9");
                    actor->memories.Add("behavior", "curiosity:0.14, libido:0.04, social:0.08");
                    
                    if (sex) {
                        //actor->inventory.AddItem(itemStick);
                        actor->memories.Add("Amethesian Empire", "anger:0.9");
                    } else {
                        actor->memories.Add("Amethesian Empire", "fear:0.9");
                    }
                } else if (Input.CheckKeyCurrent(VK_P)) {
                    
                    // Rabbit
                    
                    AI.genomes.presets.Rabbit(actor);
                    actor->CalculateBoundingRegionFromGenome();
                    
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    actor->memories.Add("behavior", "curiosity:0.14, libido:0.06, social:0.08");
                    
                    actor->physical.SetAge( age );
                    
                } else if (Input.CheckKeyCurrent(VK_O)) {
                    
                    // Spider
                    
                    AI.genomes.presets.Spider(actor);
                    actor->CalculateBoundingRegionFromGenome();
                    
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    actor->physical.SetAge( age );
                } else {
                    
                    // Human
                    
                    AI.genomes.presets.HumanWhite(actor);
                    actor->CalculateBoundingRegionFromGenome();
                    
                    std::string name = generator.GenerateIncarnation(germanPrefixes, germanNames, germanSuffixes, germanCoreSuffixes);
                    float age = Random.Range(actor->physical.GetAdultAge() / 2.0f, actor->physical.GetSeniorAge() * 1.15f);
                    bool sex = actor->physical.GetSexualOrientation();
                    
                    actor->physical.SetAge( age );
                    
                    actor->memories.Add("name", name);
                    actor->memories.Add("kingdom", "Amethesian Empire");
                    actor->memories.Add("sentience", "quota:0.9");
                    actor->memories.Add("behavior", "curiosity:0.14, libido:0.04, social:0.08");
                    actor->memories.Add("trade", "farmer");
                    
                    if (sex) {
                        //actor->inventory.AddItem(itemSword);
                        actor->memories.Add("Snort Fort Empire", "anger:0.9");
                    } else {
                        actor->memories.Add("Snort Fort Empire", "fear:0.9");
                    }
                }
                
                actor->RebuildGeneticExpression();
                actor->isActive = true;
            }
        }
    }
    
    /*
    if (Input.CheckMouseRightPressed()) {
        actorCheck = AI.Raycast(from, forward, distance);
        if (actorCheck != nullptr) {
            std::string randomThought = Int.ToString(Random.Range(1000, 9999));
            actorCheck->memories.Add(randomThought, "wtflol");
        }
    }
    */
    
    //
    // Raycast check & Inspector Rendering
    //
    if (actorTarget == nullptr) {
        actorCheck = AI.Raycast(from, forward, distance);
        if (actorCheck != nullptr) {
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
            boundsRenderer->isActive = false;
            // Clear hover name from line 0 when not looking at an actor
            Engine.console.WriteDialog(0, "");
        }
    } else {
        Engine.console.textDialog[0]->color = Colors.green * Colors.yellow * 0.9f;
        Engine.console.WriteDialog( 0, "[" + actorTarget->GetName() + "]");
        
        Engine.console.WriteDialog( 1, "Age " + Int.ToString( actorTarget->physical.GetAge() ) );
        if (actorTarget->physical.GetSexualOrientation()) 
            Engine.console.WriteDialog( 2, "Male");
        else 
            Engine.console.WriteDialog( 2, "Female");
        
        Engine.console.WriteDialog( 3, "Generation " + Int.ToString( actorTarget->genetics.GetGeneration() ) );
        
        Engine.console.textDialog[5]->color = Colors.green * Colors.yellow * 0.9f;
        Engine.console.WriteDialog( 5, "[ Vitality ]" );
        Engine.console.WriteDialog( 6, "Health      " + Float.ToString( actorTarget->biological.health ) );
        Engine.console.WriteDialog( 7, "Hunger      " + Float.ToString( actorTarget->biological.hunger) );
        Engine.console.WriteDialog( 8, "Saturation  " + Float.ToString( actorTarget->biological.saturation) );
        
        Engine.console.textDialog[10]->color = Colors.green * Colors.yellow * 0.9f;
        Engine.console.WriteDialog( 10, "[ Mental ]" );
        
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
        Engine.console.WriteDialog(11, "State  " + mode );
        
        // Emotional state
        Engine.console.WriteDialog(13, "anger     " + Float.ToString(actorTarget->emotions.current.anger));
        Engine.console.WriteDialog(14, "fear      " + Float.ToString(actorTarget->emotions.current.fear));
        Engine.console.WriteDialog(15, "comfort   " + Float.ToString(actorTarget->emotions.current.comfort));
        Engine.console.WriteDialog(16, "curiosity " + Float.ToString(actorTarget->emotions.current.curiosity));
        Engine.console.WriteDialog(17, "fatigue   " + Float.ToString(actorTarget->emotions.current.fatigue));
        Engine.console.WriteDialog(18, "libido    " + Float.ToString(actorTarget->emotions.current.libido));
        Engine.console.WriteDialog(19, "stress    " + Float.ToString(actorTarget->emotions.current.stress));
        Engine.console.WriteDialog(20, "social    " + Float.ToString(actorTarget->emotions.current.social));
        
        // Cool down counters
        Engine.console.WriteDialog(22, "Attack        " + Float.ToString(actorTarget->counters.GetCoolDownAttack()));
        Engine.console.WriteDialog(23, "Breeding      " + Float.ToString(actorTarget->counters.GetCoolDownBreeding()));
        Engine.console.WriteDialog(24, "Movement      " + Float.ToString(actorTarget->counters.GetCoolDownMovement()));
        Engine.console.WriteDialog(25, "Observe       " + Float.ToString(actorTarget->counters.GetCoolDownObservation()));
        Engine.console.WriteDialog(26, "Socialize     " + Float.ToString(actorTarget->counters.GetCoolDownSocial()));
        
        // Memories
        Engine.console.textDialog[28]->color = Colors.green * 0.8f;
        Engine.console.WriteDialog(28, "[Memories]");
        
        unsigned int numberOfMemories = actorTarget->memories.GetNumberOfMemories();
        
        // Clear elements
        for (unsigned int i=0; i < DIALOG_NUMBER_OF_ELEMENTS; i++) 
            Engine.console.WriteDialog(29 + i, "");
        for (unsigned int i=0; i < numberOfMemories && i < DIALOG_NUMBER_OF_ELEMENTS; i++) {
            std::string memory;
            std::string name = actorTarget->memories.GetMemoryNameByIndex(i);
            std::string value = actorTarget->memories.GetMemoryValueByIndex(i);
            memory = name + " == " + value;
            Engine.console.WriteDialog(29 + i, memory);
        }
        
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
        Engine.console.WriteDialog( 8, "Draw calls      " + Int.ToString(Renderer.GetNumberOfDrawCalls()) );
        Engine.console.WriteDialog(10, "GameObjects     " + Int.ToString(Engine.GetNumberOfGameObjects()) );
        Engine.console.WriteDialog(11, "Components      " + Int.ToString(Engine.GetNumberOfComponents()) );
        Engine.console.WriteDialog(13, "MeshRenderers   " + Int.ToString(Renderer.GetNumberOfMeshRenderers()) );
        Engine.console.WriteDialog(14, "Meshes          " + Int.ToString(Renderer.GetNumberOfMeshes()) );
        Engine.console.WriteDialog(15, "Materials       " + Int.ToString(Renderer.GetNumberOfMaterials()) );
        Engine.console.WriteDialog(16, "RigidBodies     " + Int.ToString(Physics.world->getNbRigidBodies()) );
        Engine.console.WriteDialog(17, "Actors          " + Int.ToString(AI.GetNumberOfActors()) );
        
        Engine.console.textDialog[19]->color = Colors.green * 0.8f;
        Engine.console.WriteDialog(19, "[Profiler]");
        
        std::map<std::string, float> profs = Profiler.GetProfiles();
        
        unsigned int index=0;
        for (const std::pair<std::string, float>& pair : profs) {
            if (pair.second != 0.0f) Engine.console.WriteDialog(20 + index, pair.first +"     "+ Float.ToString( pair.second ) );
            index++; if (index > 8) break;
        }
        Profiler.Reset();
    }
    
    CameraControllerUpdate();
}

void TickUpdate(void) {}

void HitDetection(void) {
    return;
}
