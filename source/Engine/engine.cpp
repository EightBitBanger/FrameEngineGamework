#include "engine.h"

Timer        PhysicsTime;
Timer        Time;
Logger       Log;
RandomGen    Random;
ColorPreset  Colors;

InputSystem       Input;
PhysicsSystem     Physics;
RenderSystem      Renderer;
ResourceManager   Resources;

ApplicationLayer      Application;
EngineSystemManager   Engine;


EngineSystemManager::EngineSystemManager(void) {
    return;
}

Entity* EngineSystemManager::CreateGameObject(void) {
    Entity* newEntity = Renderer.CreateEntity();
    newEntity->script = Renderer.CreateScript();
    
    return newEntity;
}



