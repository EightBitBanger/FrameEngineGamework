#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateCamera(unsigned int index) {
    if (mStreamBuffer[index].components[EngineComponents::Camera] == nullptr) 
        return;
    
    Camera* cam = (Camera*)mStreamBuffer[index].components[EngineComponents::Camera];
    
    if (cam->useMouseLook) {
        double mouseDiffX = Input.mouseX - Renderer.displayCenter.x;
        double mouseDiffY = Input.mouseY - Renderer.displayCenter.y;
        
        Input.SetMousePosition(Renderer.displayCenter.x, Renderer.displayCenter.y);
        
        float sensitivityYaw   = cam->mouseSensitivityYaw   * 0.002f;
        float sensitivityPitch = cam->mouseSensitivityPitch * 0.002f;
        
        cam->yaw   -= static_cast<float>(mouseDiffX) * sensitivityYaw;
        cam->pitch += static_cast<float>(-mouseDiffY) * sensitivityPitch;
        
        // Clamp pitch to prevent flipping
        float pitchLimit = glm::radians(89.0f);
        if (cam->pitch >  pitchLimit) cam->pitch =  pitchLimit;
        if (cam->pitch < -pitchLimit) cam->pitch = -pitchLimit;
    }
    
    cam->forward.x = cos(cam->pitch) * sin(cam->yaw);
    cam->forward.y = sin(cam->pitch);
    cam->forward.z = cos(cam->pitch) * cos(cam->yaw);
    cam->forward = glm::normalize(cam->forward);
    
    cam->transform.rotation = glm::vec3(cam->pitch, cam->yaw, 0.0f);
    
    cam->transform.position = ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->position;
}

