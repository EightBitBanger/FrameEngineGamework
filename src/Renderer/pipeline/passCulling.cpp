#include <GameEngineFramework/Renderer/rendersystem.h>
#include <GameEngineFramework/Logging/Logging.h>

#include <GameEngineFramework/Types/types.h>

float GetAngle(glm::vec2 pointA, glm::vec2 pointB) {
    
    float diffA = pointA.y - pointB.y;
    float diffB = pointA.x - pointB.x;
    
    float angle = glm::degrees( std::atan2( diffA, diffB ) );
    
    return angle;
}

bool RenderSystem::CullingPass(MeshRenderer* currentEntity, Camera* currentCamera) {
    
    float viewAngle = currentCamera->lookAngle.x;
    
    glm::vec2 to = glm::vec2(currentCamera->transform.position.x, currentCamera->transform.position.z);
    
    float overlap   = currentCamera->frustumOverlap;
    float camOffset = currentCamera->frustumOffset;
    
    bool frontRight = false;
    bool frontLeft  = false;
    bool rearRight  = false;
    bool rearLeft   = false;
    
    // Get camera corner angle
    if ((viewAngle >-10.0f)            & (viewAngle < 90.0f  + overlap)) {frontRight = true; to.x -= camOffset; to.y -= camOffset;}
    if ((viewAngle > 90.0f  - overlap) & (viewAngle < 180.0f + overlap)) {rearRight = true;  to.x += camOffset; if (!frontRight) to.y -= camOffset;}
    if ((viewAngle > 180.0f - overlap) & (viewAngle < 270.0f + overlap)) {rearLeft = true;   if (!rearRight) to.x += camOffset; to.y += camOffset;}
    if ((viewAngle > 270.0f - overlap) & (viewAngle < 370.0f))           {frontLeft = true;  if (!frontRight) to.x -= camOffset; if (!rearLeft) to.y += camOffset;}
    // Overlap less than 0 & greater than 360
    if ((viewAngle > 360.0f - overlap) | (viewAngle < 0.0f + overlap)) {frontRight = true; frontLeft = true;}
    
    // Check angle is visible
    if (frontRight) if ((currentEntity->transform.position.x > to.x) & (currentEntity->transform.position.z > to.y)) return false;
    if (frontLeft)  if ((currentEntity->transform.position.x > to.x) & (currentEntity->transform.position.z < to.y)) return false;
    if (rearRight)  if ((currentEntity->transform.position.x < to.x) & (currentEntity->transform.position.z > to.y)) return false;
    if (rearLeft)   if ((currentEntity->transform.position.x < to.x) & (currentEntity->transform.position.z < to.y)) return false;
    
    return true;
}

