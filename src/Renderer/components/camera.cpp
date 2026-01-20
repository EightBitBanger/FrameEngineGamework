#include <GameEngineFramework/Renderer/components/camera.h>

extern InputSystem Input;

Camera::Camera() : 
    
    up(glm::vec3(0, 1, 0)),
    forward(glm::vec3(0, 0, 0)),
    right(glm::vec3(0, 0, 0)),
    
    viewport(Viewport(0,0,800,600)),
    
    useMouseLook(false),
    isFixedAspect(false),
    isOrthographic(false),
    yaw(0.0f),
    pitch(0.0f),
    
    fov(60.0f),
    aspect(1.33f),
    clipNear(1.0f),
    clipFar(100000.0f),
    
    frustumOverlap(30.0f),
    frustumOffset(100.0f),
    
    mouseLimitPitchMax(180),
    mouseLimitPitchMin(180),
    
    mouseSensitivityYaw(1.3f),
    mouseSensitivityPitch(1.15f)
{
}

void Camera::EnableMouseLook(void)  {useMouseLook = true;}
void Camera::DisableMouseLook(void) {useMouseLook = false;}

float Camera::GetPitch(void) {
    return pitch;
}

float Camera::GetYaw(void) {
    return yaw;
}

