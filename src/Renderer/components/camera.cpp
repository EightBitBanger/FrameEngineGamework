#include "camera.h"
extern InputSystem Input;

Camera::Camera() : 
    
    up(glm::vec3(0, 1, 0)),
    forward(glm::vec3(0, 0, 0)),
    right(glm::vec3(0, 0, 0)),
    
    viewport(Viewport(0,0,800,600)),
    
    useMouseLook(false),
    isFixedAspect(false),
    isOrthographic(false),
    lookAngle(glm::vec2(0, 0)),
    
    fov(60),
    aspect(1.33),
    clipNear(1),
    clipFar(10000000),
    
    MouseLimitPitchMax(180),
    MouseLimitPitchMin(180),
    
    MouseSensitivityYaw(0.004),
    MouseSensitivityPitch(0.0038)
{
}

void Camera::EnableMouseLook(void)  {useMouseLook = true;}
void Camera::DisableMouseLook(void) {useMouseLook = false;}

float Camera::GetPitch(void) {
    return glm::degrees(glm::asin(transform.orientation.y));
}

float Camera::GetYaw(void) {
    return glm::degrees(glm::acos(transform.orientation.x / cos(glm::radians( glm::degrees(glm::asin(transform.orientation.y)) ))));
}