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
    lookAngle(glm::vec2(0, 0)),
    
    fov(60),
    aspect(1.33),
    clipNear(1),
    clipFar(10000000),
    
    mouseLimitPitchMax(180),
    mouseLimitPitchMin(180),
    
    mouseSensitivityYaw(1.1),
    mouseSensitivityPitch(1.2)
{
}

void Camera::EnableMouseLook(void)  {useMouseLook = true;}
void Camera::DisableMouseLook(void) {useMouseLook = false;}

float Camera::GetPitch(void) {
    return glm::degrees( glm::asin( transform.localRotation.y ) );
}

float Camera::GetYaw(void) {
    return glm::degrees( glm::acos( transform.localRotation.x / 
                               cos( 
                      glm::radians( 
                      glm::degrees( 
                         glm::asin( transform.localRotation.y ))))));
}

