#include <GameEngineFramework/Engine/EngineSystems.h>

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
        chunkManager.world.name = args[0];
    
    if (chunkManager.SaveWorld()) {
        
        Engine.Print("Saving: " + chunkManager.world.name);
        
        return;
    }
    
    Engine.Print("Error saving world: " + chunkManager.world.name);
    
    return;
}


void FuncLoad(std::vector<std::string> args) {
    
    if (args[0] != "") 
        chunkManager.world.name = args[0];
    
    if (chunkManager.LoadWorld()) {
        
        Engine.Print("Loading: " + chunkManager.world.name);
        
        return;
    }
    
    Engine.Print("Generating: " + chunkManager.world.name);
    
    chunkManager.worldSeed = Random.Range(100, 10000000) - Random.Range(100, 10000000);
    
    chunkManager.SaveWorld();
    
    return;
}


void FuncRemove(std::vector<std::string> args) {
    
    if (args[0] == "") 
        return;
    
    if (chunkManager.DestroyWorld( args[0] )) {
        
        Engine.Print("Deleted: " + args[0]);
        
        return;
    }
    
    Engine.Print("Error deleting world: " + args[0]);
    
    return;
}


void FuncClear(std::vector<std::string> args) {
    
    chunkManager.ClearWorld();
    
    Engine.cameraController->SetPosition(0, 0, 0);
    Camera* camera = Engine.cameraController->GetComponent<Camera>();
    
    camera->mouseLookAngle = glm::vec2(0, 0);
    
    Engine.Print("World cleared");
    
    return;
}


void FuncSeed(std::vector<std::string> args) {
    
    if (args[0] == "") {
        
        Engine.Print("World seed: " + Int.ToString(chunkManager.worldSeed));
        
        return;
    }
    
    chunkManager.worldSeed = String.ToInt(args[0]);
    
    Engine.Print("World seed: " + Int.ToString(chunkManager.worldSeed));
    
    return;
}


void FuncSummon(std::vector<std::string> args) {
    
    for (uint8_t i=0; i < 24; i++) {
        
        unsigned int entityType = 0;
        
        if (args[0] == "Sheep")   {entityType = 1;}
        if (args[0] == "Bear")    {entityType = 2;}
        
        if (entityType == 0) {
            
            Engine.Print("Unknown actor type");
            
            return;
        }
        
        glm::vec3 randomOffset = Engine.sceneMain->camera->transform.GetPosition();
        
        randomOffset.x += Random.Range(0, 20) - Random.Range(0, 20);
        randomOffset.z += Random.Range(0, 20) - Random.Range(0, 20);
        
        GameObject* newActorObject = chunkManager.SpawnActor( randomOffset.x, randomOffset.y, randomOffset.z );
        
        Actor* newActor = newActorObject->GetComponent<Actor>();
        
        unsigned long int actorAge = 900 + (Random.Range(0, 500) - Random.Range(0, 500));
        
        newActor->SetAge( actorAge );
        
        switch (entityType) {
            
            default:
            case 1: AI.genomes.presets.Sheep( newActor ); break;
            case 2: AI.genomes.presets.Bear( newActor ); break;
            
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
            
            weather.SetTime(7000);
            
            Engine.Print(msgTimeSetTo + "day");
            
            return;
        }
        
        if (args[1] == "noon") {
            
            weather.SetTime(12000);
            
            Engine.Print(msgTimeSetTo + "noon");
            
            return;
        }
        
        if (args[1] == "night") {
            
            weather.SetTime(17000);
            
            Engine.Print(msgTimeSetTo + "night");
            
            return;
        }
        
        if (args[1] == "midnight") {
            
            weather.SetTime(0);
            
            Engine.Print(msgTimeSetTo + "midnight");
            
            return;
        }
        
        if (!String.IsNumeric(args[1])) {
            
            Engine.Print("Invalid time " + args[1]);
            
            return;
        }
        
        int time = String.ToInt(args[1]);
        
        weather.SetTime((float)time);
        
        Engine.Print(msgTimeSetTo + args[1]);
        
        return;
    }
        
    
    return;
}

void FuncWeather(std::vector<std::string> args) {
    
    std::string msgWeatherSet = "Weather ";
    
    if (args[0] == "clear") {
        weather.SetWeatherNext(WeatherType::Clear);
        Engine.Print(msgWeatherSet + "clear");
        return;
    }
    if (args[0] == "rain") {
        weather.SetWeatherNext(WeatherType::Rain);
        Engine.Print(msgWeatherSet + "rain");
        return;
    }
    if (args[0] == "snow") {
        weather.SetWeatherNext(WeatherType::Snow);
        Engine.Print(msgWeatherSet + "snow");
        return;
    }
    
    Engine.Print(msgWeatherSet + " invalid");
    
    return;
}

