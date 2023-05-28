#include <iostream>
#include <string>

#include "../unitTest.h"
#include "../../source/Engine/Engine.h"
extern EngineSystemManager  Engine;


void ApplicationTest::TestEngineFunctionality(void) {
    if (hasTestFailed) return;
    
    std::string msgFailedObjectCreation = "engine failing to create an object\n";
    std::string msgFailedObjectDestruction = "engine failing to destroy an object\n";
    std::string msgFailedObjectListNotZero = "engine object list still contains objects\n";
    
    std::cout << "Engine functionality.... ";
    
    // Create game object
    GameObject* gameObject = Engine.CreateGameObject();
    if (gameObject == nullptr) mLogString += msgFailedObjectCreation;
    if (!Engine.DestroyGameObject(gameObject)) mLogString += msgFailedObjectDestruction;
    if (Engine.GetGameObjectCount() > 0) mLogString += msgFailedObjectListNotZero;
    
    // Create component
    Component* component = Engine.CreateComponent(ComponentType::Renderer);
    if (component == nullptr) mLogString += msgFailedObjectCreation;
    if (!Engine.DestroyComponent(component)) mLogString += msgFailedObjectDestruction;
    if (Engine.GetComponentCount() > 0) mLogString += msgFailedObjectListNotZero;
    
    // Finalize the test
    if (mLogString != "") {
        std::cout  << msgFailed << std::endl;
        std::cout << mLogString << std::endl;
        mLogString="";
    } else {
        std::cout << msgPassed << std::endl;
    }
    
    return;
}

