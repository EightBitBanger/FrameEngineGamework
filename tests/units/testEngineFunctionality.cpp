#include <iostream>
#include <string>

#include "../framework.h"
#include <GameEngineFramework/Engine/Engine.h>

extern EngineSystemManager  Engine;


void TestFramework::TestEngineFunctionality(void) {
    if (hasTestFailed) return;
    
    std::cout << "Engine functionality.... ";
    
    // Test game object
    GameObject* gameObject = Engine.CreateGameObject();
    if (gameObject == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (!Engine.DestroyGameObject(gameObject)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    if (Engine.GetGameObjectCount() > 0) Throw(msgFailedAllocatorNotZero, __FILE__, __LINE__);
    
    // Test mesh renderer component
    Component* component = Engine.CreateComponentMeshRenderer(nullptr, nullptr);
    if (component == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (!Engine.DestroyComponent(component)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    if (Engine.GetComponentCount() > 0) Throw(msgFailedAllocatorNotZero, __FILE__, __LINE__);
    
    // Test light component
    component = Engine.CreateComponentLight(glm::vec3(1, -2, 3));
    Light* lightPoint = (Light*)component->GetComponent();
    if (lightPoint->position != glm::vec3(1, -2, 3)) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (component == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (!Engine.DestroyComponent(component)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    if (Engine.GetComponentCount() > 0) Throw(msgFailedAllocatorNotZero, __FILE__, __LINE__);
    
    // Test camera controller object
    gameObject = Engine.CreateCameraController(glm::vec3(1, -2, 3), glm::vec3(1, 1, 1));
    if (gameObject->transform.position != glm::vec3(1, -2, 3)) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (gameObject == nullptr) Throw(msgFailedObjectCreate, __FILE__, __LINE__);
    if (!Engine.DestroyGameObject(gameObject)) Throw(msgFailedObjectDestroy, __FILE__, __LINE__);
    if (Engine.GetGameObjectCount() > 0) Throw(msgFailedAllocatorNotZero, __FILE__, __LINE__);
    if (Engine.GetComponentCount() > 0) Throw(msgFailedAllocatorNotZero, __FILE__, __LINE__);
    
    return;
}

