#ifndef VEHICLE_SYSTEM_H
#define VEHICLE_SYSTEM_H

#include <vector>
#include <glm/glm.hpp>

#include <GameEngineFramework/MemoryAllocation/poolallocator.h>
#include <GameEngineFramework/Physics/PhysicsSystem.h>

#include <GameEngineFramework/Plugins/VehicleSystem/components/Vehicle.h>

class ENGINE_API VehicleSystem {
public:
    
    VehicleSystem();
    
    /// Initiate the vehicle system.
    void Initiate();
    
    /// Create a vehicle
    Vehicle* CreateVehicle();
    /// Destroy a vehicle object
    bool DestroyVehicle(Vehicle* vehicle);
    
    /// Add a vehicle to the active list.
    void AddVehicleToActiveList(Vehicle* vehicle);
    /// Remove a vehicle to the active list.
    bool RemoveVehicleFromActiveList(Vehicle* vehicle);
    
    /// Update the handling simulation by the physics system`s fixed deltaTime.
    void Update(float deltaTime);
    
    /// Call once after all colliders are added to the chassis body.
    static void SetupVehicleChassis(rp3d::RigidBody* chassis, float desiredMassKg);
    
private:

    struct VehicleFrameContext {
        float deltaTime;
        float gravityAccel;

        Vehicle* vehicle;
        rp3d::RigidBody* chassis;

        rp3d::Transform  chassisTransform;
        rp3d::Vector3    chassisPos;
        rp3d::Quaternion chassisRot;

        rp3d::Vector3 chassisUp;
        rp3d::Vector3 chassisForward;
        rp3d::Vector3 chassisRight;

        rp3d::Vector3 vLinear;
        rp3d::Vector3 vAngular;

        float chassisMass;
        float speed;

        float steeringCenter;
        float steerFL;
        float steerFR;

        float driveForceTotal;
        float brakeForceTotalFinal;
        float handbrakeForceTotal;

        unsigned int wheelCount;
        unsigned int poweredWheelCount;
        unsigned int frontWheelCount;
        unsigned int rearWheelCount;

        int frontLeft;
        int frontRight;
        int rearLeft;
        int rearRight;

        float midZ;
        float weightPerWheel;
    };

    bool BeginVehicleUpdate(VehicleFrameContext& ctx, Vehicle* vehicle, float deltaTime);
    void UpdateSteeringAndDrive(VehicleFrameContext& ctx);
    void ApplyAeroAndStability(VehicleFrameContext& ctx);
    void PrepareWheelData(VehicleFrameContext& ctx);
    void SolveWheelContactsAndSuspension(VehicleFrameContext& ctx);
    void ApplyAntiRoll(VehicleFrameContext& ctx);
    void SolveTireForces(VehicleFrameContext& ctx);
    void RenderVehicle(VehicleFrameContext& ctx);

    PoolAllocator<Vehicle> mVehicleAllocator;
    std::vector<Vehicle*> mVehicles;
    
    std::vector<rp3d::Vector3> mContactPoint;
    std::vector<rp3d::Vector3> mContactNormal;
    std::vector<rp3d::Vector3> mWheelForward;
    std::vector<rp3d::Vector3> mWheelRight;
    
    std::vector<float> mNormalForce;
    std::vector<float> mCompression;
    std::vector<float> mEdgeGrip;
    
    std::vector<unsigned char> mInContact;
};

#endif
