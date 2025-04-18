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

void EngineSystemManager::WriteDialog(unsigned int index, std::string text) {
    
    mProfilerText[ index ]->text = text;
    
    return;
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

void EngineSystemManager::Print(std::string text, unsigned int fadeTimer) {
    
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
    
    if (fadeTimer == 0) {
        
        mConsoleTimers[0] = mConsoleFadeOutTimer;
        
    } else {
        
        mConsoleTimers[0] = fadeTimer;
    }
    
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
        
        char lastChar = (char)Input.lastKeyPressed;
        Input.lastKeyPressed = -1;
        
        // Add last character typed
        if (lastChar > 0x20 && lastChar < 0x7a) {
            
            // Lowercase the character
            if (lastChar > 0x40 && lastChar < 0x5b) 
                lastChar += 0x20;
            
            mConsoleString += lastChar;
        }
        
        // Space
        if (lastChar == VK_SPACE) 
            mConsoleString += VK_SPACE;
        
        // Backspace
        if (lastChar == VK_BACK) {
            int length = mConsoleString.size() - 1;
            if (length >= 0) 
                mConsoleString.resize(length);
        }
        
        // Return - run the command
        if (lastChar == VK_RETURN) {
            
            if (mConsoleString.size() < 1) 
                return;
            
            mConsoleString += " ";
            
            std::vector<std::string> command = String.Explode(mConsoleString, ' ');
            
            // Get arguments
            std::vector<std::string> args;
            for (unsigned int i=0; i < command.size()-1; i++) {
                args.push_back( command[i+1] );
            }
            
            // Send in blank strings to prevent issues with functions accessing arguments
            while (args.size() < 10) {
                args.push_back("");
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
                
                Print("Function not found");
                
            }
            
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




