#include <iostream>
#include <string>

#include "../framework.h"
#include "../../source/Engine/Engine.h"
extern EngineSystemManager  Engine;


void ApplicationTest::TestEngineFunctionality(void) {
    if (hasTestFailed) return;
    
    std::cout << "Engine functionality.... ";
    
    // Create game object
    GameObject* gameObject = Engine.CreateGameObject();
    if (gameObject == nullptr) mLogString += msgFailedObjectCreate;
    if (!Engine.DestroyGameObject(gameObject)) mLogString += msgFailedObjectDestroy;
    if (Engine.GetGameObjectCount() > 0) mLogString += msgFailedAllocatorNotZero;
    
    // Create component
    Component* component = Engine.CreateComponent(ComponentType::Renderer);
    if (component == nullptr) mLogString += msgFailedObjectCreate;
    if (!Engine.DestroyComponent(component)) mLogString += msgFailedObjectDestroy;
    if (Engine.GetComponentCount() > 0) mLogString += msgFailedAllocatorNotZero;
    
    return;
}

