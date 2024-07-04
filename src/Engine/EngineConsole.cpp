//
// Process console input

#include <GameEngineFramework/Engine/EngineSystems.h>


void EngineSystemManager::EnableConsole(void) {
    
    mIsConsoleEnabled = true;
    mConsoleInputObject->isActive = true;
    
    if (mShowConsoleBackPanel) 
        mConsolePanelObject->isActive = true;
    
    return;
}

void EngineSystemManager::DisableConsole(void) {
    
    mIsConsoleEnabled = false;
    mConsoleInputObject->isActive = false;
    mConsolePanelObject->isActive = false;
    
    return;
}

bool EngineSystemManager::CheckIsConsoleActive(void) {
    return mIsConsoleEnabled;
}

void EngineSystemManager::EnableProfiler(void) {
    
    mIsProfilerEnabled = true;
    
    for (uint8_t i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) 
        mProfilerTextObjects[i]->isActive = true;
    
    return;
}

void EngineSystemManager::DisableProfiler(void) {
    
    mIsProfilerEnabled = false;
    
    for (uint8_t i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) 
        mProfilerTextObjects[i]->isActive = false;
    
    return;
}

bool EngineSystemManager::CheckIsProfilerActive(void) {
    return mIsProfilerEnabled;
}

void EngineSystemManager::EnableConsoleBackPanel(void) {
    mShowConsoleBackPanel = true;
    return;
}

void EngineSystemManager::DisableConsoleBackPanel(void) {
    mShowConsoleBackPanel = false;
    return;
}

void EngineSystemManager::EnableConsoleCloseOnReturn(void) {
    mConsoleCloseAfterCommandEntered = true;
    return;
}

void EngineSystemManager::DisableConsoleCloseOnReturn(void) {
    mConsoleCloseAfterCommandEntered = false;
    return;
}

void EngineSystemManager::EnableConsoleFadeOutTextElements(void) {
    mConsoleDoFadeOutTexts = true;
    return;
}

void EngineSystemManager::DisableConsoleFadeOutTextElements(void) {
    mConsoleDoFadeOutTexts = false;
    return;
}

void EngineSystemManager::SetConsoleFadeOutTimer(unsigned int numberOfFrames) {
    mConsoleFadeOutTimer = numberOfFrames;
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
    mConsoleTimers[0] = mConsoleFadeOutTimer;
    
    MeshRenderer* meshRenderer = mConsoleTextObjects[0]->GetComponent<MeshRenderer>();
    meshRenderer->material->ambient.g = 1;
    
    return;
}


void EngineSystemManager::UpdateConsole(void) {
    
    // Time out the console text elements
    for (unsigned int i=0; i < CONSOLE_NUMBER_OF_ELEMENTS; i++) {
        
        mConsoleTimers[i]--;
        
        if (mConsoleDoFadeOutTexts) {
            
            if (mConsoleTimers[i] < mConsoleFadeOutTimer - 200) {
                
                float fadeBias = mConsoleTimers[i] * 0.007;
                
                MeshRenderer* meshRenderer = mConsoleTextObjects[i]->GetComponent<MeshRenderer>();
                meshRenderer->material->ambient.g = fadeBias;
                
            }
            
            if (mConsoleTimers[i] < 1) 
                mConsoleTextObjects[i]->isActive = false;
            
        }
        
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
            
            mConsoleString += " ";
            
            std::vector<std::string> command = String.Explode(mConsoleString, ' ');
            
            // Get arguments
            std::vector<std::string> args;
            for (unsigned int i=0; i < command.size()-1; i++) {
                args.push_back( command[i+1] );
            }
            
            // Find the command function
            bool doesFunctionExist = false;
            for (unsigned int i=0; i < mConsoleCommands.size(); i++) {
                
                if (mConsoleCommands[i].name != command[0]) 
                    continue;
                
                mConsoleCommands[i].function( args );
                
                doesFunctionExist = true;
                
                break;
            }
            
            // Check no function exists
            if (!doesFunctionExist) {
                
                ConsoleShiftUp("Function not found");
                
            }
            
            Input.lastKeyPressed = -1;
            mConsoleString = "";
            
            // Check close console after return
            if (mConsoleCloseAfterCommandEntered) {
                
                DisableConsole();
                Platform.isPaused = false;
                
                Platform.HideMouseCursor();
                
                if (sceneMain->camera != nullptr) 
                    sceneMain->camera->EnableMouseLook();
                
                // Reset mouse position
                Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
                
            }
            
        }
        
    }
    
    mConsoleInput->text = mConsolePrompt + mConsoleString;
    
    return;
}




