#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>
#include <GameEngineFramework/Plugins/plugins.h>

extern MeshRenderer* boundsRenderer;
extern Actor* actorTarget;

enum MenuTab {
    TAB_NONE = 0,
    TAB_CURRENT,
    TAB_NEW_WORLD,
    TAB_LOAD_WORLD
};

static MenuTab activeTab = TAB_NEW_WORLD;
static MenuTab requestedTab = TAB_NEW_WORLD;
static bool isInspectorOpen = false;

void EventLostFocus(void) {
    if (Engine.cameraController != nullptr) {
        Camera* mainCamera = Engine.cameraController->GetComponent<Camera>();
        mainCamera->DisableMouseLook();
    }
    
    Input.ClearKeys();
    Platform.ShowMouseCursor();
    
    // Do not open the main menu or change pause state if inspecting an actor
    if (actorTarget != nullptr || isInspectorOpen) {
        return;
    }
    
    if (!Platform.isPaused) {
        requestedTab = TAB_CURRENT;
        activeTab = TAB_CURRENT;
        Platform.Pause();
    }
}

void EventOnResize(void) {
    //Inventory.OnWindowResize(Platform.windowArea.w, Platform.windowArea.h);
}

void FuncList(std::vector<std::string> args) {
    std::vector<std::string> dirList = fs.DirectoryGetList("worlds");
    unsigned int numberOfWorlds = dirList.size();
    
    if (numberOfWorlds == 0) {
        Engine.console.Print("No worlds available");
        return;
    }
    
    std::string worldCount = Int.ToString(numberOfWorlds);
    if (numberOfWorlds == 1) {
        Engine.console.Print(worldCount + " world available");
    } else {
        Engine.console.Print(worldCount + " worlds available");
    }
    unsigned int nameCountMax = 5;
    
    std::string currentLine = "";
    for (unsigned int i = 0; i < dirList.size(); i++) {
        currentLine += dirList[i];
        
        if ((i + 1) % nameCountMax != 0 && i != dirList.size() - 1) {
            currentLine += " "; 
        }
        
        if ((i + 1) % nameCountMax == 0) {
            Engine.console.Print(currentLine);
            currentLine = "";
        }
    }
    
    if (!currentLine.empty()) {
        Engine.console.Print(currentLine);
    }
}

void FuncSave(std::vector<std::string> args) {
    if (args[0] != "") 
        GameWorld.world.name = args[0];
    bool failed = false;
    
    if (!GameWorld.SaveWorld()) 
        failed = true;
    
    if (failed) {
        Engine.console.Print("Error saving world: " + GameWorld.world.name);
        return;
    }
    
    Engine.console.Print("World saved '" + GameWorld.world.name + "'");
}

void FuncLoad(std::vector<std::string> args) {
    if (args[0] != "") 
        GameWorld.world.name = args[0];
    
    GameWorld.ClearWorld();
    
    if (GameWorld.LoadWorld()) {
        Engine.console.Print("World loaded " + GameWorld.world.name);
        return;
    }
    
    Engine.console.Print("Generating world '" + GameWorld.world.name + "'");
    
    GameWorld.worldSeed = Random.Range(100, 10000000) - Random.Range(100, 10000000);
    
    Weather.SetTime(9000);
    Weather.SetWeather(WeatherType::Clear);
    
    Engine.cameraController->SetPosition(0, 0, 0);
    Camera* camera = Engine.cameraController->GetComponent<Camera>();
    camera->yaw = 0.0f;
    camera->pitch = 0.0f;
    
    GameWorld.SaveWorld();
}

void FuncRemove(std::vector<std::string> args) {
    if (args[0] == "") 
        return;
    
    if (GameWorld.DestroyWorld(args[0])) {
        Engine.console.Print("World destroyed '" + args[0] + "'");
        return;
    }
    Engine.console.Print("Error deleting world '" + args[0] + "'");
}

void FuncClear(std::vector<std::string> args) {
    GameWorld.ClearWorld();
    
    Engine.cameraController->SetPosition(0, 0, 0);
    Camera* camera = Engine.cameraController->GetComponent<Camera>();
    camera->yaw = 0.0f;
    camera->pitch = 0.0f;
    
    GameWorld.world.name = "";
    
    Engine.console.Print("World cleared");
}

void FuncSeed(std::vector<std::string> args) {
    if (args[0] == "") {
        Engine.console.Print("Invalid seed '" + Int.ToString(GameWorld.worldSeed) + "'");
        return;
    }
    GameWorld.worldSeed = String.ToInt(args[0]);
    Engine.console.Print("World seed '" + Int.ToString(GameWorld.worldSeed) + "'");
}

void FuncSummon(std::vector<std::string> args) {
    for (uint8_t i = 0; i < 1; i++) {
        unsigned int entityType = 0;
        
        if (args[0] == "Sheep")   { entityType = 1; }
        if (args[0] == "Bear")    { entityType = 2; }
        if (args[0] == "Horse")   { entityType = 3; }
        if (args[0] == "Bovine")  { entityType = 4; }
        
        if (entityType == 0) {
            Engine.console.Print("Unknown actor type '" + args[0] + "'");
            return;
        }
        
        glm::vec3 randomOffset = Engine.sceneMain->camera->transform.GetPosition();
        randomOffset.x += Random.Range(0, 20) - Random.Range(0, 20);
        randomOffset.z += Random.Range(0, 20) - Random.Range(0, 20);
        
        Actor* actor = GameWorld.SummonActor(glm::vec3(randomOffset.x, randomOffset.y, randomOffset.z));
        unsigned long int actorAge = 900 + (Random.Range(0, 500) - Random.Range(0, 500));
        actor->physical.SetAge(actorAge);
        
        switch (entityType) {
            default:
            case 1: AI.genomes.presets.Sheep(actor); break;
            case 2: AI.genomes.presets.Bear(actor); break;
            case 3: AI.genomes.presets.Horse(actor); break;
            case 4: AI.genomes.presets.Bovine(actor); break;
        }
    }
}

void FuncTime(std::vector<std::string> args) {
    if (args[0] == "set") {
        std::string msgTimeSetTo = "Time set to ";
        
        if (args[1] == "day") {
            Weather.SetTime(7000);
            Engine.console.Print(msgTimeSetTo + "7000");
            return;
        }
        if (args[1] == "noon") {
            Weather.SetTime(12000);
            Engine.console.Print(msgTimeSetTo + "12000");
            return;
        }
        if (args[1] == "night") {
            Weather.SetTime(17000);
            Engine.console.Print(msgTimeSetTo + "17000");
            return;
        }
        if (args[1] == "midnight") {
            Weather.SetTime(0);
            Engine.console.Print(msgTimeSetTo + "0");
            return;
        }
        
        int time = String.ToInt(args[1]);
        if (!String.IsNumeric(args[1]) || (time < 0) || (time > 24000)) {
            Engine.console.Print("Invalid time '" + args[1] + "'");
            return;
        }
        
        Weather.SetTime((float)time);
        Engine.console.Print(msgTimeSetTo + args[1]);
    }
}

void FuncWeather(std::vector<std::string> args) {
    std::string msgWeatherSet = "Weather ";
    
    if (args[0] == "clear") {
        Weather.SetWeatherNext(WeatherType::Clear);
        Engine.console.Print(msgWeatherSet + "clear");
        return;
    }
    if (args[0] == "rain") {
        Weather.SetWeatherNext(WeatherType::Rain);
        Engine.console.Print(msgWeatherSet + "rain");
        return;
    }
    if (args[0] == "snow") {
        Weather.SetWeatherNext(WeatherType::Snow);
        Engine.console.Print(msgWeatherSet + "snow");
        return;
    }
    
    Engine.console.Print(msgWeatherSet + " invalid");
}

void MainMenuEnable(void) {
    requestedTab = TAB_CURRENT;
    activeTab = TAB_CURRENT;
    Engine.sceneMain->camera->DisableMouseLook();
    Input.ClearKeys();
    Platform.ShowMouseCursor();
}

void MainMenuDisable(void) {
    Engine.console.ClearInput();
    Engine.sceneMain->camera->EnableMouseLook();
    Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
    Platform.HideMouseCursor();
    Time.Update();
    PhysicsTime.Update();
}

bool isFullScreen = false;
void keyBindFullscreen(void) {
    isFullScreen = !isFullScreen;
    if (isFullScreen) {
        Platform.WindowEnableFullscreen();
    } else {
        Platform.WindowDisableFullscreen();
    }
}

void keyBindF4() {
    extern bool isProfilerEnabled;
    isProfilerEnabled = !isProfilerEnabled;
    
    for (unsigned int i = 0; i < Engine.console.textDialog.size(); i++) {
        Engine.console.WriteDialog(i, "");
        Engine.console.textDialog[i]->color = Colors.white;
    }
}



extern bool IsGeneEditorOpen();
extern void CloseGeneEditor();
extern bool IsActorInspectorOpen();
extern void CloseActorInspector();

void keyBindEscape() {
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

void RestoreCameraControllerToOrbitalView(Actor* target) {
    if (Engine.sceneMain == nullptr || Engine.sceneMain->camera == nullptr || target == nullptr)
        return;
    
    Camera* camera = Engine.sceneMain->camera;
    
    // Calculate the exact orbital eye position from the target
    glm::vec3 targetPos = target->navigation.GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 orbitalPos = targetPos - (camera->forward * camera->centerStandOff) + camera->up;
    
    // Teleport the RigidBody physics body and controller
    if (Engine.cameraController != nullptr) {
        rp3d::RigidBody* rigidBody = Engine.cameraController->GetComponent<rp3d::RigidBody>();
        if (rigidBody != nullptr) {
            rp3d::Transform bodyTransform = rigidBody->getTransform();
            bodyTransform.setPosition(rp3d::Vector3(orbitalPos.x, orbitalPos.y, orbitalPos.z));
            rigidBody->setTransform(bodyTransform);
            rigidBody->setLinearVelocity(rp3d::Vector3(0.0f, 0.0f, 0.0f));
        }
        Engine.cameraController->SetPosition(orbitalPos);
    }
    
    camera->transform.position = orbitalPos;
    camera->transform.UpdateMatrix();
}
