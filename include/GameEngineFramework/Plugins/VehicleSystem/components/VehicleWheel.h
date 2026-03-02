#ifndef VEHICLE_WHEEL_H
#define VEHICLE_WHEEL_H

#include <GameEngineFramework/Renderer/RenderSystem.h>

#include <glm/glm.hpp>
#include <GameEngineFramework/configuration.h>

class ENGINE_API VehicleWheel {
    friend class VehicleSystem;
    friend class Vehicle;
    
public:
    
    glm::vec3 mountPoint;
    glm::vec3 mountRotation;
    glm::vec3 mountScale;
    float radius;
    
    float mountHeight;
    
    MeshRenderer* wheelRenderer;
    
    float suspensionRestLength;
    float suspensionMinLength;
    float suspensionMaxLength;
    
    float suspensionStiffness;
    float suspensionDampingCompression;
    float suspensionDampingRelaxation;
    
    float gripLongitudinal;
    float gripLateral;
    
    float corneringStiffness;
    
    bool isPowered;
    bool isSteering;
    bool isHandbrake;
    
    bool  isInContact;
    float currentSuspensionLength;
    float suspensionVelocity;
    
    glm::vec3 contactPoint;
    glm::vec3 contactNormal;
    
    bool hasSmoothedContact;
    glm::vec3 smoothedPoint;
    glm::vec3 smoothedNormal;
    
    VehicleWheel();
    
private:
    
    float mRotationAnimation;
    
};

#endif
