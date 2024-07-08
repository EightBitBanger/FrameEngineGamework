//
// Update camera components

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


void EngineSystemManager::UpdateCamera(unsigned int index) {
    
    // Update mouse looking
    if (mStreamBuffer[index].camera->useMouseLook) {
        
        double mouseDiffX = Input.mouseX - Renderer.displayCenter.x;
        double mouseDiffY = Input.mouseY - Renderer.displayCenter.y;
        
        Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
        
        double lookAngleX = mouseDiffX * mStreamBuffer[index].camera->mouseSensitivityYaw * 0.002f;
        double lookAngleY = mouseDiffY * mStreamBuffer[index].camera->mouseSensitivityPitch * 0.002f;
        
        mStreamBuffer[index].camera->transform.RotateEuler(lookAngleX, -lookAngleY, 0);
        
        mStreamBuffer[index].camera->mouseLookAngle.x += lookAngleX / 128.0f;
        mStreamBuffer[index].camera->mouseLookAngle.y -= lookAngleY / 128.0f;
        
        // Yaw limit
        if (mStreamBuffer[index].camera->mouseLookAngle.x >= 0.109655) {mStreamBuffer[index].camera->mouseLookAngle.x -= 0.109655;}
        if (mStreamBuffer[index].camera->mouseLookAngle.x <= 0.109655) {mStreamBuffer[index].camera->mouseLookAngle.x += 0.109655;}
        
        // Pitch limit
        if (mStreamBuffer[index].camera->mouseLookAngle.y >  0.0274f) mStreamBuffer[index].camera->mouseLookAngle.y =  0.0274f;
        if (mStreamBuffer[index].camera->mouseLookAngle.y < -0.0274f) mStreamBuffer[index].camera->mouseLookAngle.y = -0.0274f;
        
    }
    
    // Calculate degree angle from camera looking angle
    // Yaw
    mStreamBuffer[index].camera->lookAngle.x = (glm::degrees( mStreamBuffer[index].camera->transform.rotation.x ) - 6.28277f) * 1.907f * 30.0f;
    mStreamBuffer[index].camera->lookAngle.x = Math.Round( +mStreamBuffer[index].camera->lookAngle.x );
    mStreamBuffer[index].camera->transform.rotation.x = mStreamBuffer[index].camera->mouseLookAngle.x;
    // Pitch
    mStreamBuffer[index].camera->transform.rotation.y = mStreamBuffer[index].camera->mouseLookAngle.y;
    mStreamBuffer[index].camera->lookAngle.y = (((glm::degrees( mStreamBuffer[index].camera->transform.rotation.y ) - 6.28277f) * 1.91f * 30.0f) + 360.0f);
    mStreamBuffer[index].camera->lookAngle.y = Math.Round( +mStreamBuffer[index].camera->lookAngle.y );
    
    // Check camera panel
    if (mStreamBuffer[index].panel != nullptr) {
        
        //
        // TODO: Add ability to align the camera view port with a panel canvas here
        //
        
    } else {
        
        mStreamBuffer[index].camera->transform.position = mStreamBuffer[index].transform->position;
        
    }
    
    return;
}

