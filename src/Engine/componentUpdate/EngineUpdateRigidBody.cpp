#include <GameEngineFramework/Engine/EngineSystems.h>

void EngineSystemManager::UpdateRigidBody(unsigned int index) {
    if (mStreamBuffer[index].components[EngineComponents::RigidBody] == nullptr) 
        return;
    
    rp3d::Transform bodyTransform = ((RigidBody*)mStreamBuffer[index].components[EngineComponents::RigidBody])->getTransform();
    rp3d::Vector3 bodyPosition = bodyTransform.getPosition();
    rp3d::Quaternion quaterion = bodyTransform.getOrientation();
    
    ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->position = glm::vec3(bodyPosition.x, 
                                                                                                     bodyPosition.y, 
                                                                                                     bodyPosition.z);
    
    ((Transform*)mStreamBuffer[index].components[EngineComponents::Transform])->rotation = glm::quat(quaterion.w, 
                                                                                                     quaterion.x, 
                                                                                                     quaterion.y, 
                                                                                                     quaterion.z);
    
    return;
}

