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
    
    // Point of origin
    eye.x = currentCamera->transform.position.x;
    eye.y = currentCamera->transform.position.y;
    eye.z = currentCamera->transform.position.z;
    
    // Forward looking angle
    currentCamera->forward.x = cos( (currentCamera->transform.rotation.x * 180) / glm::pi<float>() );
    currentCamera->forward.y = tan( (currentCamera->transform.rotation.y * 180) / glm::pi<float>() );
    currentCamera->forward.z = sin( (currentCamera->transform.rotation.x * 180) / glm::pi<float>() );
    
    glm::vec3 lookingAngle;
    lookingAngle.x = eye.x + currentCamera->forward.x;
    lookingAngle.y = eye.y + currentCamera->forward.y;
    lookingAngle.z = eye.z + currentCamera->forward.z;
    
    // View angle
    glm::mat4 view = glm::lookAt(eye, lookingAngle, currentCamera->up);
    
    // Calculate perspective / orthographic angle
    if (!currentCamera->isOrthographic) {
        
        glm::mat4 projection = glm::perspective( glm::radians( currentCamera->fov ), 
                                                 currentCamera->aspect, 
                                                 currentCamera->clipNear, 
                                                 currentCamera->clipFar);
        
        viewProjection = projection * view;
        
    } else {
        
        glm::mat4 projection = glm::ortho(0.0f, 
                                          (float)displaySize.x, 
                                          (float)displaySize.y, 
                                          0.0f, 
                                          currentCamera->clipNear, 
                                          currentCamera->clipFar);
        
        viewProjection = projection * view;
        
    }
    
    // Right angle to the looking angle
    currentCamera->right = glm::normalize(glm::cross(currentCamera->up, currentCamera->forward));
    
    return true;
}

