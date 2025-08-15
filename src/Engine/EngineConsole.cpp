#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/EngineSystems.h>
#include <map>

std::map<std::string, void(*)(std::vector<std::string>)> functionList;

void ConsoleReturnCallback(std::string& console_text) {
    std::vector<std::string> arguments = String.Explode(console_text, ' ');
    
    // Provide blanks to prevent access crashes 
    // by the clients functions
    if (arguments.size() < CONSOLE_NUMBER_OF_ELEMENTS) 
        for (unsigned int i=0; i < CONSOLE_NUMBER_OF_ELEMENTS; i++) 
            arguments.push_back("");
    
    std::string command = arguments[0];
    
    // Check blank function name
    if (command == "") 
        return;
    
    // Remove the first element (it is the function name)
    arguments.erase( arguments.begin() );
    
    std::map<std::string, void(*)(std::vector<std::string>)>::iterator it = functionList.find( command );
    extern EngineSystemManager Engine;
    
    // Check command exists
    if (it == functionList.end()) {
        std::string commandNotFound = "Unknown command '" + command + "'";
        Engine.console.Print(commandNotFound);
        Engine.console.ClearInput();
        return;
    }
    
    // Run the command
    functionList[command](arguments);
    
    // Close the command console
    if (Engine.console.doCloseConsoleAfterCommand) {
        Engine.console.Disable();
        Platform.Pause();
        
        if (Engine.cameraController != nullptr) {
            Camera* mainCamera = Engine.cameraController->GetComponent<Camera>();
            mainCamera->EnableMouseLook();
        }
        Engine.console.ClearInput();
        Platform.HideMouseCursor();
    }
}


bool EngineSystemManager::CommandConsole::RegisterCommand(std::string name, void(*function_ptr)(std::vector<std::string>)) {
    functionList[name] = function_ptr;
    return true;
}

void EngineSystemManager::CommandConsole::Enable(void) {
    input->isActive = true;
    for (unsigned int i=0; i < CONSOLE_NUMBER_OF_ELEMENTS; i++) {
        Text* text = textElements[i];
        text->isActive = true;
    }
}

void EngineSystemManager::CommandConsole::Disable(void) {
    input->isActive = false;
}

void EngineSystemManager::CommandConsole::ShiftUp(void) {
    for (unsigned int i=(CONSOLE_NUMBER_OF_ELEMENTS-1); i > 0; i--) {
        textElements[i]->text = textElements[i - 1]->text;
        textElements[i]->color = textElements[i - 1]->color;
        textElements[i]->isActive = textElements[i - 1]->isActive;
    }
}

void EngineSystemManager::CommandConsole::Print(std::string text) {
    ShiftUp();
    textElements[0]->isActive = true;
    textElements[0]->color.a = 3.0f;
    textElements[0]->text = text;
}

void EngineSystemManager::CommandConsole::WriteDialog(unsigned int index, std::string text) {
    textDialog[index]->isActive = true;
    textDialog[index]->color.a = 3.0f;
    textDialog[index]->text = text;
}

void EngineSystemManager::CommandConsole::ClearInput(void) {
    input->text = '\0';
}

