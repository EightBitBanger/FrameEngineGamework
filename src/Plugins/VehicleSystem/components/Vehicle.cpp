#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/VehicleSystem/components/Vehicle.h>

static float ClampFloat(float v, float a, float b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

Vehicle::Vehicle() :
    isActive(true),
    mChassis(nullptr),
    mPlayerController(nullptr),
    
    mEngineForceInput(0.0f),
    mBrakeForceInput(0.0f),
    mHandbrakeForceInput(0.0f),
    mSteeringInput(0.0f),
    
    mMaxEnginePower(20000.0f),
    mMaxBreakingPower(25000.0f),
    
    mSteeringSmoothed(0.0f),
    mSteeringSmoothRate(10.0f),
    mMaxSteeringAngle(8.0f),
    
    mSteerHighSpeed(25.0f),
    mSteerMinFactor(0.35f),
    mBrakeBiasFront(0.65f),
    
    mTractionLoss(0.75f),
    mTractionMultiplier(8.0f),
    
    mAntiRollFront(7000.0f),
    mAntiRollRear(9000.0f),
    mMaxSuspensionWeightFactor(3.0f),
    mEnableAckermann(true),
    mAckermannWheelbase(0.0f),
    mAckermannTrackWidth(0.0f),
    mDragCoefficient(0.35f),
    mDownforceCoefficient(0.8f),
    mYawStability(0.0f)
{
}

void Vehicle::SetRigidBody(rp3d::RigidBody* body) {
    mChassis = body;
}

void Vehicle::AddMeshRenderer(MeshRenderer* renderer) {
    mRenderers.push_back(renderer);
}

int Vehicle::AddWheel(const VehicleWheel& wheelDefinition) {
    mWheels.push_back(wheelDefinition);
    return (int)mWheels.size() - 1;
}


void Vehicle::ApplyEngineForce(float engineForce) {
    mEngineForceInput = engineForce;
}

void Vehicle::ApplyBreakForce(float brakeForce) {
    mBrakeForceInput  = brakeForce;
}

void Vehicle::ApplySteeringForce(float steering) {
    mSteeringInput = ClampFloat(steering, -1.0f, 1.0f);
}

void Vehicle::ApplyHandBreakForce(float handbrakeForce) {
    mHandbrakeForceInput = handbrakeForce;
}


void Vehicle::SetSteeringRadius(float maxSteeringRadius) {
    mMaxSteeringAngle = maxSteeringRadius;
}

void Vehicle::SetSteeringSmoothingRate(float smoothRate) {
    mSteeringSmoothRate = smoothRate;
}

void Vehicle::SetSteeringSpeedCurve(float highSpeed, float minFactor) {
    mSteerHighSpeed = highSpeed;
    mSteerMinFactor = ClampFloat(minFactor, 0.0f, 1.0f);
}

void Vehicle::SetBrakeBias(float bias) {
    mBrakeBiasFront = ClampFloat(bias, 0.0f, 1.0f);
}

void Vehicle::SetAntiRoll(float frontStiffness, float rearStiffness) {
    mAntiRollFront = frontStiffness;
    mAntiRollRear  = rearStiffness;
}

void Vehicle::SetMaxEnginePower(float maxPower) {
    mMaxEnginePower = maxPower;
}

void Vehicle::SetMaxBreakingForce(float maxBreaking) {
    mMaxBreakingPower = maxBreaking;
}

void Vehicle::EnableAckermann(void) {
    mEnableAckermann = true;
}

void Vehicle::DisableAckermann(void) {
    mEnableAckermann = false;
}

void Vehicle::SetAckermann(float wheelbase, float trackWidth) {
    if (wheelbase < 0.0f) wheelbase = 0.0f;
    if (trackWidth < 0.0f) trackWidth = 0.0f;
    mAckermannWheelbase = wheelbase;
    mAckermannTrackWidth = trackWidth;
}

void Vehicle::SetAero(float dragCoefficient, float downforceCoefficient) {
    mDragCoefficient = dragCoefficient;
    mDownforceCoefficient = downforceCoefficient;
}

void Vehicle::SetYawStability(float yawStability) {
    mYawStability = yawStability;
}

void Vehicle::SetSuspensionClamp(float maxWeightFactor) {
    mMaxSuspensionWeightFactor = maxWeightFactor;
}

void Vehicle::SetTractionLoss(float loss) {
    mTractionLoss = loss;
}

void Vehicle::SetTractionMultiplier(float mul) {
    mTractionMultiplier = mul;
}

void Vehicle::SetPlayerController(GameObject* gameObject) {
    mPlayerController = gameObject;
}

unsigned int Vehicle::GetNumberOfWheels() {
    return mWheels.size();
}

VehicleWheel& Vehicle::GetWheel(unsigned int index) {
    return mWheels[index];
}
