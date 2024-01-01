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
    if (mStreamBuffer[index].camera->useMouseLook) {
        
        float MouseDiffX = (Input.mouseX - Renderer.displayCenter.x) * mStreamBuffer[index].camera->MouseSensitivityYaw;
        float MouseDiffY = (Input.mouseY - Renderer.displayCenter.y) * mStreamBuffer[index].camera->MouseSensitivityPitch;
        
        Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
        
        mStreamBuffer[index].camera->lookAngle.x += MouseDiffX * 0.01;
        mStreamBuffer[index].camera->lookAngle.y -= MouseDiffY * 0.01;
        
        // Yaw limit
        if (mStreamBuffer[index].camera->lookAngle.x >= 0.109655) {mStreamBuffer[index].camera->lookAngle.x -= 0.109655;}
        if (mStreamBuffer[index].camera->lookAngle.x <= 0.109655) {mStreamBuffer[index].camera->lookAngle.x += 0.109655;}
        
        // Pitch limit
        if (mStreamBuffer[index].camera->lookAngle.y >  0.0274f) mStreamBuffer[index].camera->lookAngle.y =  0.0274f;
        if (mStreamBuffer[index].camera->lookAngle.y < -0.0274f) mStreamBuffer[index].camera->lookAngle.y = -0.0274f;
        
    }
    
    // Restore looking angle
    mStreamBuffer[index].camera->transform.localRotation.x = mStreamBuffer[index].camera->lookAngle.x;
    mStreamBuffer[index].camera->transform.localRotation.y = mStreamBuffer[index].camera->lookAngle.y;
    
    // Check camera panel
    if (mStreamBuffer[index].panel != nullptr) {
        
        //
        // TODO: Add ability to align the camera with a panel canvas here
        //
        
    } else {
        
        mStreamBuffer[index].camera->transform.position = mStreamBuffer[index].transform->position;
        
    }
    
    return;
}

