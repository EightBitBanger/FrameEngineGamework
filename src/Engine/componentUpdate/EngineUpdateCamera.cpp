//
// Update camera components

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


void EngineSystemManager::UpdateCamera(unsigned int index) {
    
    // Update mouse looking
    if (streamBuffer[index].camera->useMouseLook) {
        
        float MouseDiffX = (Input.mouseX - Renderer.displayCenter.x) * streamBuffer[index].camera->MouseSensitivityYaw;
        float MouseDiffY = (Input.mouseY - Renderer.displayCenter.y) * streamBuffer[index].camera->MouseSensitivityPitch;
        
        Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
        
        streamBuffer[index].camera->lookAngle.x += MouseDiffX * 0.01;
        streamBuffer[index].camera->lookAngle.y -= MouseDiffY * 0.01;
        
        // Yaw limit
        if (streamBuffer[index].camera->lookAngle.x >= 0.109655) {streamBuffer[index].camera->lookAngle.x -= 0.109655;}
        if (streamBuffer[index].camera->lookAngle.x <= 0.109655) {streamBuffer[index].camera->lookAngle.x += 0.109655;}
        
        // Pitch limit
        if (streamBuffer[index].camera->lookAngle.y >  0.0274f) streamBuffer[index].camera->lookAngle.y =  0.0274f;
        if (streamBuffer[index].camera->lookAngle.y < -0.0274f) streamBuffer[index].camera->lookAngle.y = -0.0274f;
        
    }
    
    // Restore looking angle
    streamBuffer[index].camera->transform.orientation.x = streamBuffer[index].camera->lookAngle.x;
    streamBuffer[index].camera->transform.orientation.y = streamBuffer[index].camera->lookAngle.y;
    
    // Check camera panel
    if (streamBuffer[index].panel != nullptr) {
        
        //
        // TODO: Add ability to align the camera with a panel canvas here
        //
        
    } else {
        
        streamBuffer[index].camera->transform.position = streamBuffer[index].gameObject->mTransformCache->position;
        
    }
    
    return;
}

