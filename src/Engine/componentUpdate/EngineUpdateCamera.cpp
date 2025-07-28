#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateCamera(unsigned int index) {
    Camera* cam = mStreamBuffer[index].camera;
    
    // Mouse look
    if (cam->useMouseLook) {
        // Get mouse delta
        double mouseDiffX = Input.mouseX - Renderer.displayCenter.x;
        double mouseDiffY = Input.mouseY - Renderer.displayCenter.y;
        
        // Reset mouse to center
        Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
        
        // Apply sensitivity and convert to radians
        float sensitivityYaw   = cam->mouseSensitivityYaw   * 0.002f;
        float sensitivityPitch = cam->mouseSensitivityPitch * 0.002f;
        
        cam->yaw   -= static_cast<float>(mouseDiffX) * sensitivityYaw;
        cam->pitch += static_cast<float>(-mouseDiffY) * sensitivityPitch;
        
        // Clamp pitch to prevent flipping
        float pitchLimit = glm::radians(89.0f);
        if (cam->pitch >  pitchLimit) cam->pitch =  pitchLimit;
        if (cam->pitch < -pitchLimit) cam->pitch = -pitchLimit;
    }
    
    // Update camera forward direction from yaw/pitch
    cam->forward.x = cos(cam->pitch) * sin(cam->yaw);
    cam->forward.y = sin(cam->pitch);
    cam->forward.z = cos(cam->pitch) * cos(cam->yaw);
    cam->forward = glm::normalize(cam->forward);
    
    // Update transform rotation (stored as pitch, yaw, roll)
    cam->transform.rotation = glm::vec3(cam->pitch, cam->yaw, 0.0f); // roll is zero for FPS
    
    // Sync camera position with player
    cam->transform.position = mStreamBuffer[index].transform->position;
    
    return;
}

