//
// Update rigid body components

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


void EngineSystemManager::UpdateRigidBody(unsigned int index) {
    
    // Get the rigid body transform
    rp3d::Transform bodyTransform = mStreamBuffer[index].rigidBody->getTransform();
    rp3d::Vector3 bodyPosition = bodyTransform.getPosition();
    rp3d::Quaternion quaterion = bodyTransform.getOrientation();
    
    mStreamBuffer[index].transform->position = glm::vec3(bodyPosition.x, 
                                                         bodyPosition.y, 
                                                         bodyPosition.z);
    
    mStreamBuffer[index].transform->rotation = glm::quat(quaterion.w, 
                                                         quaterion.x, 
                                                         quaterion.y, 
                                                         quaterion.z);
    
    return;
}

