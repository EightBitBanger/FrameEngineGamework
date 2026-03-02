#include <GameEngineFramework/Renderer/components/camera.h>

extern InputSystem Input;

Camera::Camera() : 
    
    up(glm::vec3(0, 1, 0)),
    forward(glm::vec3(0, 0, 0)),
    right(glm::vec3(0, 0, 0)),
    
    viewport(Viewport(0,0,800,600)),
    
    useMouseLook(false),
    useThirdPersonView(false),
    isFixedAspect(false),
    isOrthographic(false),
    yaw(0.0f),
    pitch(0.0f),
    
    fov(60.0f),
    aspect(1.33f),
    clipNear(0.5f),
    clipFar(100000.0f),
    
    frustumOverlap(30.0f),
    frustumOffset(100.0f),
    
    mouseLimitPitchMax(89.9f),
    mouseLimitPitchMin(-89.9f),
    
    mouseSensitivityYaw(1.3f),
    mouseSensitivityPitch(1.15f),
    centerStandOff(8.0f),
    centerPosition({0.0f,0.0f,0.0f})
{
}

void Camera::EnableMouseLook(void)  {useMouseLook = true;}
void Camera::DisableMouseLook(void) {useMouseLook = false;}

void Camera::EnableThirdPersonLook() {useThirdPersonView = true;}
void Camera::DisableThirdPersonLook() {useThirdPersonView = false;}


float Camera::GetPitch(void) {
    return pitch;
}

float Camera::GetYaw(void) {
    return yaw;
}

