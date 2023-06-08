#include <iostream>
#include <string>

#include "../framework.h"
#include "../../source/Engine/Engine.h"
extern EngineSystemManager  Engine;


void TestFramework::TestEngineFunctionality(void) {
    if (hasTestFailed) return;
    
    std::cout << "Engine functionality.... ";
    
    // Test game object
    GameObject* gameObject = Engine.CreateGameObject();
    if (gameObject == nullptr) mLogString += msgFailedObjectCreate;
    if (!Engine.DestroyGameObject(gameObject)) mLogString += msgFailedObjectDestroy;
    if (Engine.GetGameObjectCount() > 0) mLogString += msgFailedAllocatorNotZero;
    
    // Test mesh renderer component
    Component* component = Engine.CreateComponentMeshRenderer(nullptr, nullptr);
    if (component == nullptr) mLogString += msgFailedObjectCreate;
    if (!Engine.DestroyComponent(component)) mLogString += msgFailedObjectDestroy;
    if (Engine.GetComponentCount() > 0) mLogString += msgFailedAllocatorNotZero;
    
    // Test light component
    component = Engine.CreateComponentLight(glm::vec3(1, -2, 3));
    Light* lightPoint = (Light*)component->GetComponent();
    if (lightPoint->transform.position != glm::vec3(1, -2, 3)) mLogString += msgFailedObjectCreate;
    if (component == nullptr) mLogString += msgFailedObjectCreate;
    if (!Engine.DestroyComponent(component)) mLogString += msgFailedObjectDestroy;
    if (Engine.GetComponentCount() > 0) mLogString += msgFailedAllocatorNotZero;
    
    // Test camera controller object
    gameObject = Engine.CreateCameraController(glm::vec3(1, -2, 3));
    if (gameObject->transform.position != glm::vec3(1, -2, 3)) mLogString += msgFailedObjectCreate;
    if (gameObject == nullptr) mLogString += msgFailedObjectCreate;
    if (!Engine.DestroyGameObject(gameObject)) mLogString += msgFailedObjectDestroy;
    if (Engine.GetGameObjectCount() > 0) mLogString += msgFailedAllocatorNotZero;
    if (Engine.GetComponentCount() > 0) mLogString += msgFailedAllocatorNotZero;
    
    return;
}

