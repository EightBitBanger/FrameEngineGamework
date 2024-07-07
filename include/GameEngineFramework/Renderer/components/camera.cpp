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
    mouseLookAngle(glm::vec2(0, 0)),
    lookAngle(glm::vec2(0, 0)),
    
    fov(60),
    aspect(1.33),
    clipNear(0.1),
    clipFar(10000000),
    
    frustumOverlap(30.0f),
    frustumOffset(100.0f),
    
    mouseLimitPitchMax(180),
    mouseLimitPitchMin(180),
    
    mouseSensitivityYaw(1.3),
    mouseSensitivityPitch(1.15)
{
}

void Camera::EnableMouseLook(void)  {useMouseLook = true;}
void Camera::DisableMouseLook(void) {useMouseLook = false;}

float Camera::GetPitch(void) {
    return lookAngle.y;
}

float Camera::GetYaw(void) {
    return lookAngle.x;
}

