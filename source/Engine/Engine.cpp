#include "engine.h"

Timer        PhysicsTime;
Timer        Time;
Logger       Log;
RandomGen    Random;
ColorPreset  Colors;

InputSystem       Input;
PhysicsSystem     Physics;
RenderSystem      Renderer;
ScriptSystem      Scripting;
ResourceManager   Resources;

ApplicationLayer      Application;
EngineSystemManager   Engine;


EngineSystemManager::EngineSystemManager(void) {
    
    return;
}


GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = gameObject.Create();
    return newGameObject;
}

void EngineSystemManager::DestroyGameObject(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    gameObject.Destroy(gameObjectPtr);
    return;
}


