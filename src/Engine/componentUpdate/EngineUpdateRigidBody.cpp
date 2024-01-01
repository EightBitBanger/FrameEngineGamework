//
// Update rigid body components

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

ENGINE_API extern ApplicationLayer      Application;
ENGINE_API extern EngineSystemManager   Engine;


void EngineSystemManager::UpdateRigidBody(unsigned int index) {
    
    // Get the rigid body transform
    rp3d::Transform bodyTransform = streamBuffer[index].rigidBody->getTransform();
    rp3d::Vector3 bodyPosition = bodyTransform.getPosition();
    rp3d::Quaternion quaterion = bodyTransform.getOrientation();
    
    Transform currentTransform;
    currentTransform.position.x = bodyPosition.x;
    currentTransform.position.y = bodyPosition.y;
    currentTransform.position.z = bodyPosition.z;
    
    currentTransform.orientation.w = quaterion.w;
    currentTransform.orientation.x = quaterion.x;
    currentTransform.orientation.y = quaterion.y;
    currentTransform.orientation.z = quaterion.z;
    
    // Update the game object transform
    streamBuffer[index].gameObject->mTransformCache->position    = currentTransform.position;
    streamBuffer[index].gameObject->mTransformCache->orientation = currentTransform.orientation;
    
    // Source the transformation matrix
    bodyTransform.getOpenGLMatrix( &currentTransform.matrix[0][0] );
    
    return;
}

