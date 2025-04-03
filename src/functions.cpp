#include <GameEngineFramework/Engine/EngineSystems.h>

#include <GameEngineFramework/functions.h>

#include <GameEngineFramework/plugins.h>


void EventLostFocus(void) {
    
    Engine.EnableConsole();
    
    Camera* mainCamera = Engine.cameraController->GetComponent<Camera>();
    
    mainCamera->DisableMouseLook();
    
    Input.ClearKeys();
    
    Platform.ShowMouseCursor();
    
    if (!Platform.isPaused) 
        Platform.Pause();
    
    return;
}


void FuncList(std::vector<std::string> args) {
    
    std::vector<std::string> dirList = fs.DirectoryGetList("worlds");
    
    if (dirList.size() == 0) {
        
        Engine.Print("No worlds");
        
        return;
    }
    
    for (unsigned int i=0; i < dirList.size(); i++) 
        Engine.Print( dirList[i] );
    
    return;
}


void FuncSave(std::vector<std::string> args) {
    
    if (args[0] != "") 
        GameWorld.world.name = args[0];
    
    if (GameWorld.SaveWorld()) {
        
        Engine.Print("Saving: " + GameWorld.world.name);
        
        return;
    }
    
    Engine.Print("Error saving world: " + GameWorld.world.name);
    
    return;
}


void FuncLoad(std::vector<std::string> args) {
    
    if (args[0] != "") 
        GameWorld.world.name = args[0];
    
    if (GameWorld.LoadWorld()) {
        
        Engine.Print("Loading: " + GameWorld.world.name);
        
        return;
    }
    
    Engine.Print("Generating: " + GameWorld.world.name);
    
    GameWorld.worldSeed = Random.Range(100, 10000000) - Random.Range(100, 10000000);
    
    Weather.SetTime(9000);
    Weather.SetWeather(WeatherType::Clear);
    
    GameWorld.SaveWorld();
    
    return;
}


void FuncRemove(std::vector<std::string> args) {
    
    if (args[0] == "") 
        return;
    
    if (GameWorld.DestroyWorld( args[0] )) {
        
        Engine.Print("Deleted: " + args[0]);
        
        return;
    }
    
    Engine.Print("Error deleting world: " + args[0]);
    
    return;
}


void FuncClear(std::vector<std::string> args) {
    
    GameWorld.ClearWorld();
    
    Engine.cameraController->SetPosition(0, 0, 0);
    Camera* camera = Engine.cameraController->GetComponent<Camera>();
    
    camera->mouseLookAngle = glm::vec2(0, 0);
    
    Engine.Print("World cleared");
    
    return;
}


void FuncSeed(std::vector<std::string> args) {
    
    if (args[0] == "") {
        
        Engine.Print("World seed: " + Int.ToString(GameWorld.worldSeed));
        
        return;
    }
    
    GameWorld.worldSeed = String.ToInt(args[0]);
    
    Engine.Print("World seed: " + Int.ToString(GameWorld.worldSeed));
    
    return;
}


void FuncSummon(std::vector<std::string> args) {
    
    for (uint8_t i=0; i < 1; i++) {
        
        unsigned int entityType = 0;
        
        if (args[0] == "Sheep")   {entityType = 1;}
        if (args[0] == "Bear")    {entityType = 2;}
        if (args[0] == "Horse")   {entityType = 3;}
        if (args[0] == "Bovine")  {entityType = 4;}
        
        if (entityType == 0) {
            
            Engine.Print("Unknown actor type");
            
            return;
        }
        
        glm::vec3 randomOffset = Engine.sceneMain->camera->transform.GetPosition();
        
        randomOffset.x += Random.Range(0, 20) - Random.Range(0, 20);
        randomOffset.z += Random.Range(0, 20) - Random.Range(0, 20);
        
        GameObject* newActorObject = GameWorld.SpawnActor( randomOffset.x, randomOffset.y, randomOffset.z );
        
        Actor* newActor = newActorObject->GetComponent<Actor>();
        
        unsigned long int actorAge = 900 + (Random.Range(0, 500) - Random.Range(0, 500));
        
        newActor->physical.SetAge( actorAge );
        
        switch (entityType) {
            
            default:
            case 1: AI.genomes.presets.Sheep( newActor ); break;
            case 2: AI.genomes.presets.Bear( newActor ); break;
            case 3: AI.genomes.presets.Horse( newActor ); break;
            case 4: AI.genomes.presets.Bovine( newActor ); break;
            
        }
        
        continue;
    }
    
    Engine.Print("Actor summoned");
    
    return;
}


void FuncTime(std::vector<std::string> args) {
    
    if (args[0] == "set") {
        
        std::string msgTimeSetTo = "Time set to ";
        
        if (args[1] == "day") {
            Weather.SetTime(7000);
            Engine.Print(msgTimeSetTo + "day");
            return;
        }
        
        if (args[1] == "noon") {
            Weather.SetTime(12000);
            Engine.Print(msgTimeSetTo + "noon");
            return;
        }
        
        if (args[1] == "night") {
            Weather.SetTime(17000);
            Engine.Print(msgTimeSetTo + "night");
            return;
        }
        
        if (args[1] == "midnight") {
            Weather.SetTime(0);
            Engine.Print(msgTimeSetTo + "midnight");
            return;
        }
        
        if (!String.IsNumeric(args[1])) {
            Engine.Print("Invalid time " + args[1]);
            return;
        }
        
        int time = String.ToInt(args[1]);
        
        Weather.SetTime((float)time);
        
        Engine.Print(msgTimeSetTo + args[1]);
        
        return;
    }
        
    
    return;
}

void FuncWeather(std::vector<std::string> args) {
    
    std::string msgWeatherSet = "Weather ";
    
    if (args[0] == "clear") {
        Weather.SetWeatherNext(WeatherType::Clear);
        Engine.Print(msgWeatherSet + "clear");
        return;
    }
    if (args[0] == "rain") {
        Weather.SetWeatherNext(WeatherType::Rain);
        Engine.Print(msgWeatherSet + "rain");
        return;
    }
    if (args[0] == "snow") {
        Weather.SetWeatherNext(WeatherType::Snow);
        Engine.Print(msgWeatherSet + "snow");
        return;
    }
    
    Engine.Print(msgWeatherSet + " invalid");
    
    return;
}


// Button callbacks


void ButtonLoadWorld(Button* currentButton) {
    
    // Mouse hovering over button check
    //if (!currentButton->isHovering) {Engine.WriteDialog(14, "");} else {Engine.WriteDialog(14, "Hovering");}
    
    if (!currentButton->isHovering || !Input.CheckMouseLeftPressed()) 
        return;
    
    MainMenuDisable();
    
    Platform.isPaused = false;
    
    std::vector<std::string> arg;
    arg.push_back("");
    
    FuncLoad(arg);
    
    return;
}

void ButtonSaveWorld(Button* currentButton) {
    
    if (!currentButton->isHovering || !Input.CheckMouseLeftPressed()) 
        return;
    
    GameWorld.SaveWorld();
    
    return;
}

void ButtonClearWorld(Button* currentButton) {
    
    if (!currentButton->isHovering || !Input.CheckMouseLeftPressed()) 
        return;
    
    GameWorld.ClearWorld();
    
    Weather.SetWeather( WeatherType::Clear );
    
    return;
}

void ButtonQuitApplication(Button* currentButton) {
    
    if (!currentButton->isHovering || !Input.CheckMouseLeftPressed()) 
        return;
    
    Platform.isActive = false;
    
    return;
}

void MainMenuEnable(void) {
    
    Engine.EnableConsole();
    
    Engine.sceneMain->camera->DisableMouseLook();
    
    Input.ClearKeys();
    
    Platform.ShowMouseCursor();
    
    // Enable menu UI
    //menuPanelObject->Activate();
    //versionTextObject->Activate();
    //loadWorldButton->Activate();
    //saveWorldButton->Activate();
    //clearWorldButton->Activate();
    //quitButton->Activate();
    
    return;
}

void MainMenuDisable(void) {
    
    Engine.DisableConsole();
    Engine.ConsoleClearInputString();
    
    Engine.sceneMain->camera->EnableMouseLook();
    
    // Reset mouse position
    Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Platform.HideMouseCursor();
    
    // Reset timers
    Time.Update();
    PhysicsTime.Update();
    
    // Disable menu UI
    //menuPanelObject->Deactivate();
    //versionTextObject->Deactivate();
    //loadWorldButton->Deactivate();
    //saveWorldButton->Deactivate();
    //clearWorldButton->Deactivate();
    //quitButton->Deactivate();
    
    return;
}

