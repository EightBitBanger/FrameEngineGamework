// Vehicle.h
#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include <glm/glm.hpp>

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Renderer/components/MeshRenderer.h>

#include <GameEngineFramework/Plugins/VehicleSystem/components/VehicleWheel.h>

class ENGINE_API Vehicle {
    friend class VehicleSystem;
    
public:
    
    bool isActive;
    
    Vehicle();
    
    /// Set the physical body for this vehicle.
    void SetRigidBody(rp3d::RigidBody* body);
    
    /// Set the camera to follow this vehicle.
    void SetPlayerController(GameObject* gameObject);
    
    /// Add a mesh renderer to this vehicle.
    void AddMeshRenderer(MeshRenderer* renderer);
    
    /// Add a wheel definition, returns wheel index.
    int AddWheel(const VehicleWheel& wheelDefinition);
    
    /// Set the amount of engine force. 0.0 - 1.0
    void ApplyEngineForce(float engineForce);
    
    /// Set the amount of breaking force. 0.0 - 1.0
    void ApplyBreakForce(float brakeForce);
    
    /// Set the amount of steering in a given direction.
    /// Less than 0.0 - right. Greater than 0.0 - left
    void ApplySteeringForce(float steering);
    
    /// Set the amount of hand break force.
    void ApplyHandBreakForce(float handbrakeForce);
    
    /// Set the steering damping speed curve.
    void SetSteeringSpeedCurve(float highSpeed, float minFactor);
    
    /// Set the smoothing rate for change in steering.
    void SetSteeringSmoothingRate(float smoothRate);
    
    /// Set the maximum steering radius.
    void SetSteeringRadius(float maxSteeringRadius);
    
    /// Set the braking bias from front to rear. 0 = rear, 1 = front
    void SetBrakeBias(float bias);
    
    /// Anti-roll stiffness (front/rear).
    void SetAntiRoll(float frontStiffness, float rearStiffness);
    
    /// Set the maximum amount of power the engine can produce.
    void SetMaxEnginePower(float maxPower);
    
    /// Set the maximum amount of breaking force the breaks can produce.
    void SetMaxBreakingForce(float maxBreaking);
    
    /// Enable the Ackermann steering approximation.
    void EnableAckermann(void);
    
    /// Disable the Ackermann steering approximation.
    void DisableAckermann(void);
    
    /// Set Ackermann geometry (wheelbase and track width in your units).
    void SetAckermann(float wheelbase, float trackWidth);
    
    /// Aero drag and downforce dynamic.
    void SetAero(float dragCoefficient, float downforceCoefficient);
    
    /// Optional yaw damping torque scale. 0 disables
    void SetYawStability(float yawStability);
    
    /// Clamp suspension force relative to static wheel weight.
    void SetSuspensionClamp(float maxWeightFactor);
    
    /// Traction bias: 0 = equal split, 1 = load-based split
    void SetTractionLoss(float loss);
    void SetTractionMultiplier(float mul);
    
    /// Get the number of wheels on this vehicle.
    unsigned int GetNumberOfWheels();
    
    /// Get a reference to an attached wheel.
    VehicleWheel& GetWheel(unsigned int index);
    
private:
    
    rp3d::RigidBody* mChassis;
    
    GameObject* mPlayerController;
    
    std::vector<MeshRenderer*> mRenderers;
    std::vector<VehicleWheel>  mWheels;
    
    float mEngineForceInput;
    float mBrakeForceInput;
    float mHandbrakeForceInput;
    float mSteeringInput;
    
    float mMaxEnginePower;
    float mMaxBreakingPower;
    
    float mSteeringSmoothed;
    float mSteeringSmoothRate;
    
    float mMaxSteeringAngle;
    float mSteerHighSpeed;
    float mSteerMinFactor;
    
    float mBrakeBiasFront;
    
    float mTractionLoss;
    float mTractionMultiplier;
    
    float mAntiRollFront;
    float mAntiRollRear;
    
    float mMaxSuspensionWeightFactor;
    
    bool  mEnableAckermann;
    float mAckermannWheelbase;
    float mAckermannTrackWidth;
    
    float mDragCoefficient;
    float mDownforceCoefficient;
    
    float mYawStability;
};

#endif
