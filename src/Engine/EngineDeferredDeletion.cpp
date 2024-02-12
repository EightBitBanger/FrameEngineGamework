//
// Deferred deletion algorithm

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern StringType        String;
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

ENGINE_API extern PlatformLayer     Platform;



void EngineSystemManager::ProcessDeferredDeletion(void) {
    
    // Game objects
    for (unsigned int i=0; i < mGarbageObjects.size(); i++) {
        
        GameObject* objectPtr = mGarbageObjects[i];
        
        DestroyGameObject( objectPtr );
        
        continue;
    }
    mGarbageObjects.clear();
    
    // Rigid bodies
    if (mGarbageRigidBodies.size() > 0) {
        
        RigidBody* rigidBodyPtr = mGarbageRigidBodies.end();
        mGarbageRigidBodies.erase( mGarbageRigidBodies.end() );
        
        Physics.DestroyRigidBody( rigidBodyPtr );
        
    }
    
    return;
}


