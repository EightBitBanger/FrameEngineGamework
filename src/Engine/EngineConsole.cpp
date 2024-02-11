//
// Update console

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern StringType        String;
ENGINE_API extern Logger            Log;
ENGINE_API extern Timer             PhysicsTime;
ENGINE_API extern Timer             Time;

ENGINE_API extern Serialization     Serializer;
ENGINE_API extern ResourceManager   Resources;
ENGINE_API extern ScriptSystem      Scripting;
ENGINE_API extern RenderSystem      Renderer;
ENGINE_API extern PhysicsSystem     Physics;
ENGINE_API extern AudioSystem       Audio;
ENGINE_API extern InputSystem       Input;
ENGINE_API extern MathCore          Math;
ENGINE_API extern ActorSystem       AI;

ENGINE_API extern PlatformLayer     Platform;



void EngineSystemManager::EnableConsole(void) {
    
    mIsConsoleEnabled = true;
    mConsoleInputObject->isActive = true;
    mConsolePanelObject->isActive = true;
    
    return;
}

void EngineSystemManager::DisableConsole(void) {
    
    mIsConsoleEnabled = false;
    mConsoleInputObject->isActive = false;
    mConsolePanelObject->isActive = false;
    
    return;
}

void EngineSystemManager::ConsoleRegisterCommand(std::string name, void(*function)(std::vector<std::string>)) {
    ConsoleCommand command;
    command.name = name;
    command.function = function;
    mConsoleCommands.push_back( command );
    return;
}

void EngineSystemManager::ConsoleClearInputString(void) {
    mConsoleInput->text = "";
    mConsoleString = "";
    return;
}

void EngineSystemManager::ConsoleClearLog(void) {
    for (unsigned int i=0; i < CONSOLE_NUMBER_OF_ELEMENTS; i++) 
        mConsoleText[i]->text = "";
    return;
}

void EngineSystemManager::ConsoleShiftUp(std::string text) {
    
    // Shift up the texts
    for (unsigned int i=CONSOLE_NUMBER_OF_ELEMENTS - 1; i > 0; i--) 
        mConsoleText[i]->text = mConsoleText[i - 1]->text;
    
    // Shift up game object is active states
    for (unsigned int i=CONSOLE_NUMBER_OF_ELEMENTS - 1; i > 0; i--) 
        mConsoleTextObjects[i]->isActive = mConsoleTextObjects[i - 1]->isActive;
    
    // Shift up transparency levels
    for (unsigned int i=CONSOLE_NUMBER_OF_ELEMENTS - 1; i > 0; i--) {
        
        MeshRenderer* meshRendererA = mConsoleTextObjects[i]->GetComponent<MeshRenderer>();
        MeshRenderer* meshRendererB = mConsoleTextObjects[i - 1]->GetComponent<MeshRenderer>();
        
        meshRendererA->material->ambient.g = meshRendererB->material->ambient.g;
        
    }
    
    // Shift up the timers
    for (unsigned int i=CONSOLE_NUMBER_OF_ELEMENTS - 1; i > 0; i--) 
        mConsoleTimers[i] = mConsoleTimers[i - 1];
    
    // Submit new line of text after the up shift
    mConsoleText[0]->text = text;
    mConsoleTextObjects[0]->isActive = true;
    mConsoleTimers[0] = 600;
    
    MeshRenderer* meshRenderer = mConsoleTextObjects[0]->GetComponent<MeshRenderer>();
    meshRenderer->material->ambient.g = 1;
    
    return;
}


void EngineSystemManager::UpdateConsole(void) {
    
    // Time out the console text elements
    for (unsigned int i=0; i < CONSOLE_NUMBER_OF_ELEMENTS; i++) {
        
        mConsoleTimers[i]--;
        
        if (mConsoleTimers[i] < 250) {
            
            float fadeBias = mConsoleTimers[i] * 0.007;
            
            MeshRenderer* meshRenderer = mConsoleTextObjects[i]->GetComponent<MeshRenderer>();
            meshRenderer->material->ambient.g = fadeBias;
            
        }
        
        if (mConsoleTimers[i] < 1) 
            mConsoleTextObjects[i]->isActive = false;
        
    }
    
    
    if (!mIsConsoleEnabled) 
        return;
    
    // Check shifted upper case
    bool isShifted = false;
    if (Input.CheckKeyCurrent(VK_SHIFT)) 
        isShifted = true;
    
    if (Input.lastKeyPressed != -1) {
        
        // Letter case
        if (((Input.lastKeyPressed > 64) & (Input.lastKeyPressed < 91))) {
            
            char lastChar = (char)Input.lastKeyPressed;
            
            if (!isShifted) 
                lastChar += 0x20;
            
            mConsoleString += lastChar;
            
            Input.lastKeyPressed = -1;
        }
        
        // Numbers
        if ( (Input.lastKeyPressed > 47) & (Input.lastKeyPressed < 58) ) {
            
            char lastChar = (char)Input.lastKeyPressed;
            
            mConsoleString += lastChar;
            
            Input.lastKeyPressed = -1;
        }
        
        // Space
        if (Input.lastKeyPressed == VK_SPACE) {
            
            mConsoleString += VK_SPACE;
            
            Input.lastKeyPressed = -1;
        }
        
        // Backspace
        if (Input.lastKeyPressed == VK_BACK) {
            
            Input.lastKeyPressed = -1;
            
            int length = mConsoleString.size() - 1;
            if (length >= 0) 
                mConsoleString.resize(length);
        }
        
        // Return - run the command
        if (Input.lastKeyPressed == VK_RETURN) {
            
            if (mConsoleString.size() < 1) 
                return;
            
            std::vector<std::string> command = String.Explode(mConsoleString, ' ');
            
            for (unsigned int i=0; i < mConsoleCommands.size(); i++) {
                
                if (mConsoleCommands[i].name != command[0]) 
                    continue;
                
                mConsoleCommands[i].function( command );
                
                continue;
            }
            
            Input.lastKeyPressed = -1;
            mConsoleString = "";
            
            DisableConsole();
            Platform.isPaused = false;
            
            Platform.HideMouseCursor();
            
            sceneMain->camera->EnableMouseLook();
            
            // Reset mouse position
            Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
            
        }
        
    }
    
    mConsoleInput->text = mConsolePrompt + mConsoleString;
    
    return;
}




