#include "camera.h"

Camera::Camera() : 
    
    up(glm::vec3(0, 1, 0)),
    forward(glm::vec3(0, 0, 0)),
    right(glm::vec3(0, 0, 0)),
    
    viewport(Viewport(0,0,800,600)),
    
    useMouseLook(false),
    isFixedAspect(false),
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

void Camera::SetMouseCenter(int mouseResetX, int mouseResetY) {CursorSetPosition(mouseResetX, mouseResetY);}
void Camera::EnableMouseLook(void)  {useMouseLook = true;}
void Camera::DisableMouseLook(void) {useMouseLook = false;}

void Camera::MouseLook(float deltaTime, int mouseResetX, int mouseResetY) {
    
    glm::vec2 MousePos = CursorGetPosition();
    CursorSetPosition(mouseResetX, mouseResetY);
    
    float MouseDiffX = (MousePos.x - mouseResetX) * MouseSensitivityYaw * deltaTime;
    float MouseDiffY = (MousePos.y - mouseResetY) * MouseSensitivityPitch * deltaTime;
    
    lookAngle.x += MouseDiffX * 0.001f;
    lookAngle.y -= MouseDiffY * 0.001f;
    
    // Yaw limit
    if (lookAngle.x >= 0.109655)  {lookAngle.x -= 0.109655;}
    if (lookAngle.x <= 0.109655)  {lookAngle.x += 0.109655;}
    
    // Pitch limit
    if (lookAngle.y >  0.0274f) lookAngle.y =  0.0274f;
    if (lookAngle.y < -0.0274f) lookAngle.y = -0.0274f;
    
    // Apply rotation to the transform
    transform.orientation.x = lookAngle.x;
    transform.orientation.y = lookAngle.y;
    
    return;
}

float Camera::GetPitch(void) {
    return glm::degrees(glm::asin(transform.orientation.y));
}

float Camera::GetYaw(void) {
    return glm::degrees(glm::acos(transform.orientation.x / cos(glm::radians( glm::degrees(glm::asin(transform.orientation.y)) ))));
}
