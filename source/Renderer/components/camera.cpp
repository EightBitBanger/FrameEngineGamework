#include "camera.h"

Camera::Camera() {
    
    up      = glm::vec3(0, 1, 0);
    forward = glm::vec3(0, 0, 0);
    right   = glm::vec3(0, 0, 0);
    
    script = nullptr;
    
    useMovementKeys = false;
    useMouseLook = false;
    
    cameraSpeed = 100;
    
    fov       = 60;
    aspect    = 1.33;
    clipNear  = 0.03;
    clipFar   = 1000000;
    
    MouseLimitPitchMax = 180;
    MouseLimitPitchMin = 180;
    
    MouseSensitivityYaw   = 0.004;
    MouseSensitivityPitch = 0.0038;
    
}

void Camera::SetMouseCenter(int mouseResetX, int mouseResetY) {CursorSetPosition(mouseResetX, mouseResetY);}
void Camera::EnableMouseLook(void)  {ShowCursor(false); useMouseLook = true;}
void Camera::DisableMouseLook(void) {ShowCursor(true); useMouseLook = false;}


void Camera::MouseLook(float deltaTime, int mouseResetX, int mouseResetY) {
    
    glm::vec2 MousePos = CursorGetPosition();
    CursorSetPosition(mouseResetX, mouseResetY);
    
    float MouseDiffX = (MousePos.x - mouseResetX) * MouseSensitivityYaw * deltaTime;
    float MouseDiffY = (MousePos.y - mouseResetY) * MouseSensitivityPitch * deltaTime;
    
    transform.rotation.x += (float)MouseDiffX;
    transform.rotation.y -= (float)MouseDiffY;
    
    if (transform.rotation.x >= 0.109655)  {transform.rotation.x -= 0.109655;}
    if (transform.rotation.x <= 0.109655)  {transform.rotation.x += 0.109655;}
    
    if (transform.rotation.y >  0.0274f) transform.rotation.y =  0.0274f;
    if (transform.rotation.y < -0.0274f) transform.rotation.y = -0.0274f;
    
    return;
}

glm::mat4 Camera::CalculateView(void) {
    
    glm::vec3 pos;
    pos.x = transform.position.x;
    pos.y = transform.position.y;
    pos.z = transform.position.z;
    
    forward.x = cos( transform.rotation.x * 180 / glm::pi<float>() );
    forward.y = tan( transform.rotation.y * 180 / glm::pi<float>() );
    forward.z = sin( transform.rotation.x * 180 / glm::pi<float>() );
    
    forward = glm::normalize(forward);
    
    glm::vec3 angle;
    angle.x = transform.position.x + forward.x;
    angle.y = transform.position.y + forward.y;
    angle.z = transform.position.z + forward.z;
    
    right = glm::normalize(glm::cross(up, forward));
    
    glm::mat4 view = glm::lookAt(pos, angle, up);
    
    return view;
}

glm::mat4 Camera::CalculatePerspectiveMatrix(void) {
    
    glm::mat4 projection = glm::perspective( glm::radians( fov ), aspect, clipNear, clipFar);
    
    return projection;
}

glm::mat4 Camera::CalculateOrthiographicMatrix(Viewport viewport) {
    
    
    // \/ sus* O_o \/
    
    glm::mat4 projection = glm::ortho(viewport.x, viewport.w, viewport.h, viewport.y, -1, 1);
    
    return projection;
}


