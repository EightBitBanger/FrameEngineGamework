#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateRigidBody(unsigned int index) {
    
    rp3d::Transform bodyTransform = mStreamBuffer[index].rigidBody->getTransform();
    rp3d::Vector3 bodyPosition = bodyTransform.getPosition();
    rp3d::Quaternion quaterion = bodyTransform.getOrientation();
    
    mStreamBuffer[index].transform->position = glm::vec3(bodyPosition.x, 
                                                         bodyPosition.y, 
                                                         bodyPosition.z);
    
    mStreamBuffer[index].transform->rotation = glm::quat(quaterion.w, 
                                                         quaterion.x, 
                                                         quaterion.y, 
                                                         quaterion.z);
    
    return;
}

