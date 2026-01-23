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
    
}

void FuncList(std::vector<std::string> args) {
    
}


void FuncSave(std::vector<std::string> args) {
    
}


void FuncLoad(std::vector<std::string> args) {
    
}



void FuncRemove(std::vector<std::string> args) {
    
}


void FuncClear(std::vector<std::string> args) {
    
}


void FuncSeed(std::vector<std::string> args) {
    
}


void FuncSummon(std::vector<std::string> args) {
    
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

