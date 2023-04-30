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


Entity* EngineSystemManager::CreateGameObject(void) {
    Entity* newEntity = Renderer.CreateEntity();
    newEntity->script = Scripting.CreateScript();
    
    return newEntity;
}

void EngineSystemManager::DestroyGameObject(Entity* entityPtr) {
    assert(entityPtr != nullptr);
    if (entityPtr->script != nullptr) Scripting.DestroyScript(entityPtr->script);
    
    Renderer.DestroyEntity(entityPtr);
    return;
}



