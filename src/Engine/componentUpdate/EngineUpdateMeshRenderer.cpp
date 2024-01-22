//
// Update mesh renderer components

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern Logger            Log;
ENGINE_API extern Timer             PhysicsTime;
ENGINE_API extern Timer             Time;

ENGINE_API extern Serialization     Serializer;
ENGINE_API extern ResourceManager   Resources;
ENGINE_API extern ScriptSystem      Scripting;
ENGINE_API extern RenderSystem      Renderer;
ENGINE_API extern PhysicsSystem     Physics;
ENGINE_API extern AudioSystem       Audio;
ENGINE_API extern InputSystem       Input;
ENGINE_API extern MathCore          Math;
ENGINE_API extern ActorSystem       AI;

ENGINE_API extern PlatformLayer         Platform;
ENGINE_API extern EngineSystemManager   Engine;


void EngineSystemManager::UpdateMeshRenderer(unsigned int index) {
    
    mStreamBuffer[index].meshRenderer->transform.position  = mStreamBuffer[index].transform->position;
    mStreamBuffer[index].meshRenderer->transform.rotation  = mStreamBuffer[index].transform->rotation;
    mStreamBuffer[index].meshRenderer->transform.scale     = mStreamBuffer[index].transform->scale;
    
    mStreamBuffer[index].meshRenderer->transform.matrix = mStreamBuffer[index].transform->matrix;
    
    return;
}

