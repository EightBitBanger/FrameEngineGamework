#include <iostream>
#include <string>

#include "../unitTest.h"
#include "../../source/Engine/Engine.h"
extern EngineSystemManager  Engine;


void ApplicationTest::TestEngineFunctionality(void) {
    if (hasTestFailed) return;
    
    std::cout << "Engine functionality.... ";
    
    // Create game object
    GameObject* gameObject = Engine.CreateGameObject();
    if (gameObject == nullptr) mLogString += msgFailedObjectCreation;
    if (!Engine.DestroyGameObject(gameObject)) mLogString += msgFailedObjectDestruction;
    if (Engine.GetGameObjectCount() > 0) mLogString += msgFailedObjectAllocator;
    
    // Create component
    Component* component = Engine.CreateComponent(ComponentType::Renderer);
    if (component == nullptr) mLogString += msgFailedObjectCreation;
    if (!Engine.DestroyComponent(component)) mLogString += msgFailedObjectDestruction;
    if (Engine.GetComponentCount() > 0) mLogString += msgFailedObjectAllocator;
    
    return;
}

