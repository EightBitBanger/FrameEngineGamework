#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/functions.h>
#include <GameEngineFramework/Plugins/plugins.h>


void EventLostFocus(void) {
    Engine.console.Enable();
    
    if (Engine.cameraController != nullptr) {
        Camera* mainCamera = Engine.cameraController->GetComponent<Camera>();
        mainCamera->DisableMouseLook();
    }
    
    Input.ClearKeys();
    Platform.ShowMouseCursor();
    
    if (!Platform.isPaused) 
        Platform.Pause();
}

void EventOnResize(void) {
    Inventory.OnWindowResize(Platform.windowArea.w, Platform.windowArea.h);
}

void FuncList(std::vector<std::string> args) {
    std::vector<std::string> dirList = fs.DirectoryGetList("worlds");
    
    if (dirList.size() == 0) {
        Engine.console.Print("No worlds");
        return;
    }
    
    for (unsigned int i=0; i < dirList.size(); i++) 
        Engine.console.Print( dirList[i] );
}


void FuncSave(std::vector<std::string> args) {
    if (args[0] != "") 
        GameWorld.world.name = args[0];
    
    if (GameWorld.SaveWorld()) {
        Engine.console.Print("Saving: " + GameWorld.world.name);
        return;
    }
    
    Engine.console.Print("Error saving world: " + GameWorld.world.name);
}


void FuncLoad(std::vector<std::string> args) {
    if (args[0] != "") 
        GameWorld.world.name = args[0];
    
    if (GameWorld.LoadWorld()) {
        Engine.console.Print("Loading: " + GameWorld.world.name);
        return;
    }
    Engine.console.Print("Generating: " + GameWorld.world.name);
    
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
    
    if (GameWorld.DestroyWorld( args[0] )) {
        Engine.console.Print("Deleted: " + args[0]);
        return;
    }
    Engine.console.Print("Error deleting world: " + args[0]);
}


void FuncClear(std::vector<std::string> args) {
    GameWorld.ClearWorld();
    
    Engine.cameraController->SetPosition(0, 0, 0);
    Camera* camera = Engine.cameraController->GetComponent<Camera>();
    camera->yaw = 0.0f;
    camera->pitch = 0.0f;
    
    Engine.console.Print("World cleared");
}


void FuncSeed(std::vector<std::string> args) {
    if (args[0] == "") {
        Engine.console.Print("World seed: " + Int.ToString(GameWorld.worldSeed));
        return;
    }
    GameWorld.worldSeed = String.ToInt(args[0]);
    Engine.console.Print("World seed: " + Int.ToString(GameWorld.worldSeed));
}


void FuncSummon(std::vector<std::string> args) {
    for (uint8_t i=0; i < 1; i++) {
        
        unsigned int entityType = 0;
        
        if (args[0] == "Sheep")   {entityType = 1;}
        if (args[0] == "Bear")    {entityType = 2;}
        if (args[0] == "Horse")   {entityType = 3;}
        if (args[0] == "Bovine")  {entityType = 4;}
        
        if (entityType == 0) {
            Engine.console.Print("Unknown actor type");
            return;
        }
        
        glm::vec3 randomOffset = Engine.sceneMain->camera->transform.GetPosition();
        
        randomOffset.x += Random.Range(0, 20) - Random.Range(0, 20);
        randomOffset.z += Random.Range(0, 20) - Random.Range(0, 20);
        
        Actor* actor = GameWorld.SummonActor( glm::vec3(randomOffset.x, randomOffset.y, randomOffset.z) );
        
        //GameObject* actorObject = (GameObject*)actor->user.GetUserDataA();
        //actorObject->SetPosition(randomOffset.x, randomOffset.y, randomOffset.z);
        
        unsigned long int actorAge = 900 + (Random.Range(0, 500) - Random.Range(0, 500));
        
        actor->physical.SetAge( actorAge );
        
        switch (entityType) {
            
            default:
            case 1: AI.genomes.presets.Sheep( actor ); break;
            case 2: AI.genomes.presets.Bear( actor ); break;
            case 3: AI.genomes.presets.Horse( actor ); break;
            case 4: AI.genomes.presets.Bovine( actor ); break;
            
        }
        
        continue;
    }
}


void FuncTime(std::vector<std::string> args) {
    if (args[0] == "set") {
        std::string msgTimeSetTo = "Time set to ";
        
        if (args[1] == "day") {
            Weather.SetTime(7000);
            Engine.console.Print(msgTimeSetTo + "day");
            return;
        }
        
        if (args[1] == "noon") {
            Weather.SetTime(12000);
            Engine.console.Print(msgTimeSetTo + "noon");
            return;
        }
        
        if (args[1] == "night") {
            Weather.SetTime(17000);
            Engine.console.Print(msgTimeSetTo + "night");
            return;
        }
        
        if (args[1] == "midnight") {
            Weather.SetTime(0);
            Engine.console.Print(msgTimeSetTo + "midnight");
            return;
        }
        
        if (!String.IsNumeric(args[1])) {
            Engine.console.Print("Invalid time " + args[1]);
            return;
        }
        
        int time = String.ToInt(args[1]);
        
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
    Engine.console.Enable();
    Engine.sceneMain->camera->DisableMouseLook();
    Input.ClearKeys();
    Platform.ShowMouseCursor();
}

void MainMenuDisable(void) {
    // Clear the console
    Engine.console.Disable();
    Engine.console.ClearInput();
    
    Engine.sceneMain->camera->EnableMouseLook();
    
    // Reset mouse position
    Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Platform.HideMouseCursor();
    
    // Reset timers
    Time.Update();
    PhysicsTime.Update();
}


// Key bindings

bool isFullScreen = false;
void keyBindFullscreen(void) {
    isFullScreen = !isFullScreen;
    if (isFullScreen) {
        Platform.WindowEnableFullscreen();
    } else {
        Platform.WindowDisableFullscreen();
    }
}

void keyBindEscape(void) {
    Platform.Pause();
    if (Platform.isPaused == true) {
        MainMenuEnable();
        return;
    }
    MainMenuDisable();
}

void keyBindF4() {
    extern bool isProfilerEnabled;
    isProfilerEnabled = !isProfilerEnabled;
    
    if (!isProfilerEnabled) 
        for (unsigned int i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) 
            Engine.console.WriteDialog(i, "");
    for (unsigned int i=0; i < 20; i++) 
        Engine.console.WriteDialog(i, "");
}

