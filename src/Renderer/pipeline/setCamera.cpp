#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>
extern MathCore  Math;


bool RenderSystem::setTargetCamera(Camera* currentCamera, glm::vec3& eye, glm::mat4& viewProjection) {
    if (currentCamera == nullptr)
        return false;
    
    glViewport(currentCamera->viewport.x,
            currentCamera->viewport.y,
            currentCamera->viewport.w,
            currentCamera->viewport.h);
    
    float pitch = currentCamera->GetPitch();
    float yaw   = currentCamera->GetYaw();
    
    currentCamera->forward.x = cos(pitch) * sin(yaw);
    currentCamera->forward.y = sin(pitch);
    currentCamera->forward.z = cos(pitch) * cos(yaw);
    currentCamera->forward   = glm::normalize(currentCamera->forward);
    
    const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);
    
    glm::vec3 lookingAt(0.0f);
    glm::vec3 camUp    = worldUp;
    glm::vec3 camRight = glm::vec3(1.0f, 0.0f, 0.0f);
    
    camRight = glm::cross(currentCamera->forward, worldUp);
    if (glm::length2(camRight) < 1e-6f)
        camRight = glm::vec3(1.0f, 0.0f, 0.0f);
    camRight = glm::normalize(camRight);
    
    camUp = glm::cross(camRight, currentCamera->forward);
    if (glm::length2(camUp) < 1e-6f)
        camUp = worldUp;
    else
        camUp = glm::normalize(camUp);
    
    glm::mat4 view(1.0f);
    if (currentCamera->useThirdPersonView) {
        glm::vec3 target = currentCamera->transform.position;
        
        eye = target - currentCamera->forward * currentCamera->centerStandOff + camUp;
        
        lookingAt = target;
        
        view = glm::lookAt(eye, lookingAt, camUp);
        view = glm::translate(view, currentCamera->centerPosition);
    } else {
        eye = currentCamera->transform.position;
        
        lookingAt = eye + currentCamera->forward;
        
        view = glm::lookAt(eye, lookingAt, camUp);
    }
    
    if (!currentCamera->isOrthographic) {
        glm::mat4 projection = glm::perspective(
            glm::radians(currentCamera->fov),
            currentCamera->aspect,
            currentCamera->clipNear,
            currentCamera->clipFar
        );
        
        viewProjection = projection * view;
    } else {
        glm::mat4 projection = glm::ortho(
            0.0f,
            (float)displaySize.x,
            (float)displaySize.y,
            0.0f,
            currentCamera->clipNear,
            currentCamera->clipFar
        );
        
        viewProjection = projection * view;
    }
    
    currentCamera->up    = camUp;
    currentCamera->right = camRight;
    
    return true;
}
