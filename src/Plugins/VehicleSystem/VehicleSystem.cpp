#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/VehicleSystem/VehicleSystem.h>

#include <cmath>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

static float ClampFloat(float v, float a, float b) {
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

static rp3d::Vector3 NormalizeOr(const rp3d::Vector3& v, const rp3d::Vector3& fallback) {
    rp3d::decimal len = v.length();
    if (len > rp3d::decimal(0.000001)) {
        return v / len;
    }
    rp3d::decimal flen = fallback.length();
    if (flen > rp3d::decimal(0.000001)) {
        return fallback / flen;
    }
    return rp3d::Vector3(0.0, 0.0, 0.0);
}

static bool SampleWheelContactPatch(
    const rp3d::Vector3& mountWorld,
    const rp3d::Vector3& wheelForward,
    const rp3d::Vector3& wheelRight,
    float castStart,
    float castLength,
    float minGroundNormalY,
    glm::vec3& outPoint,
    glm::vec3& outNormal) {

    const float patch = 0.18f;

    const glm::vec3 up(0.0f, 1.0f, 0.0f);

    glm::vec3 f((float)wheelForward.x, (float)wheelForward.y, (float)wheelForward.z);
    glm::vec3 r((float)wheelRight.x,   (float)wheelRight.y,   (float)wheelRight.z);

    glm::vec3 base(mountWorld.x, mountWorld.y, mountWorld.z);
    glm::vec3 start = base + up * castStart;

    const glm::vec3 offsets[5] = {
        glm::vec3(0.0f),
        f * patch,
        -f * patch,
        r * patch,
        -r * patch
    };

    glm::vec3 accP(0.0f);
    glm::vec3 accN(0.0f);
    float accW = 0.0f;

    for (int i = 0; i < 5; i++) {

        Hit hit;
        glm::vec3 from = start + offsets[i];
        glm::vec3 dir  = glm::vec3(0.0f, -1.0f, 0.0f);

        if (!Physics.Raycast(from, dir, castLength, hit, LayerMask::Ground)) {
            continue;
        }

        glm::vec3 n = hit.normal;
        float nLenSq = n.x*n.x + n.y*n.y + n.z*n.z;
        if (nLenSq > 0.000001f) {
            float inv = 1.0f / std::sqrt(nLenSq);
            n.x *= inv; n.y *= inv; n.z *= inv;
        } else {
            n = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (n.y < 0.0f) n = -n;
        if (n.y < minGroundNormalY) {
            continue;
        }

        glm::vec3 p = hit.point;

        glm::vec3 d = p - from;
        float dist = std::sqrt(d.x*d.x + d.y*d.y + d.z*d.z);

        float w = 1.0f / (0.25f + dist);

        accP += p * w;
        accN += n * w;
        accW += w;
    }

    if (accW <= 0.0f) {
        return false;
    }

    outPoint = accP * (1.0f / accW);

    float nLenSq = accN.x*accN.x + accN.y*accN.y + accN.z*accN.z;
    if (nLenSq > 0.000001f) {
        float inv = 1.0f / std::sqrt(nLenSq);
        outNormal = accN * inv;
    } else {
        outNormal = glm::vec3(0.0f, 1.0f, 0.0f);
    }

    return true;
}

/// Configure chassis mass and inertia
void VehicleSystem::SetupVehicleChassis(rp3d::RigidBody* chassis, float desiredMassKg) {

    if (chassis == NULL) {
        return;
    }

    chassis->updateMassPropertiesFromColliders();

    rp3d::decimal currentMass = chassis->getMass();
    if (currentMass <= rp3d::decimal(0.0)) {
        currentMass = rp3d::decimal(1.0);
    }

    chassis->setLocalCenterOfMass(rp3d::Vector3(0.0, 0.0, 0.0));

    rp3d::decimal massScale = rp3d::decimal(desiredMassKg) / currentMass;

    chassis->setMass(rp3d::decimal(desiredMassKg));

    rp3d::Vector3 inertia = chassis->getLocalInertiaTensor();
    inertia.x *= massScale;
    inertia.y *= massScale;
    inertia.z *= massScale;

    rp3d::decimal minInertia = rp3d::decimal(0.1);
    if (inertia.x < minInertia) inertia.x = minInertia;
    if (inertia.y < minInertia) inertia.y = minInertia;
    if (inertia.z < minInertia) inertia.z = minInertia;

    chassis->setLocalInertiaTensor(inertia);

    chassis->setLinearDamping(rp3d::decimal(0.15));
    chassis->setAngularDamping(rp3d::decimal(0.35));
}

VehicleSystem::VehicleSystem() {}

void VehicleSystem::Initiate() {}

Vehicle* VehicleSystem::CreateVehicle() {
    Vehicle* vehicle = mVehicleAllocator.Create();
    return vehicle;
}

bool VehicleSystem::DestroyVehicle(Vehicle* vehicle) {

    if (vehicle == NULL) {
        return false;
    }

    for (unsigned int i = 0; i < mVehicles.size(); i++) {
        if (mVehicles[i] != vehicle) continue;
        mVehicles.erase(mVehicles.begin() + i);
        break;
    }

    return mVehicleAllocator.Destroy(vehicle);
}

void VehicleSystem::AddVehicleToActiveList(Vehicle* vehicle) {

    if (vehicle == NULL) {
        return;
    }

    for (unsigned int i = 0; i < vehicle->mRenderers.size(); i++) {
        vehicle->mRenderers[i]->isActive = true;
    }

    mVehicles.push_back(vehicle);
}

bool VehicleSystem::RemoveVehicleFromActiveList(Vehicle* vehicle) {

    if (vehicle == NULL) {
        return false;
    }

    for (unsigned int i = 0; i < mVehicles.size(); i++) {

        Vehicle* v = mVehicles[i];
        if (v != vehicle) continue;

        for (unsigned int r = 0; r < vehicle->mRenderers.size(); r++) {
            vehicle->mRenderers[r]->isActive = false;
        }

        mVehicles.erase(mVehicles.begin() + i);
        return true;
    }

    return false;
}

void VehicleSystem::Update(float deltaTime) {

    if (deltaTime <= 0.0f) {
        return;
    }

    for (unsigned int vIndex = 0; vIndex < mVehicles.size(); vIndex++) {

        Vehicle* vehicle = mVehicles[vIndex];
        VehicleFrameContext ctx;

        if (!BeginVehicleUpdate(ctx, vehicle, deltaTime)) {
            continue;
        }

        UpdateSteeringAndDrive(ctx);
        
        ApplyAeroAndStability(ctx);
        
        PrepareWheelData(ctx);
        
        SolveWheelContactsAndSuspension(ctx);
        
        ApplyAntiRoll(ctx);
        
        SolveTireForces(ctx);
        
        RenderVehicle(ctx);
    }
}


bool VehicleSystem::BeginVehicleUpdate(VehicleFrameContext& ctx, Vehicle* vehicle, float deltaTime) {

    if (vehicle == NULL) return false;
    if (!vehicle->isActive) return false;
    if (vehicle->mChassis == NULL) return false;

    ctx.deltaTime = deltaTime;
    ctx.gravityAccel = 9.81f;

    ctx.vehicle = vehicle;
    ctx.chassis = vehicle->mChassis;

    ctx.chassisTransform = ctx.chassis->getTransform();
    ctx.chassisPos = ctx.chassisTransform.getPosition();
    ctx.chassisRot = ctx.chassisTransform.getOrientation();

    ctx.chassisUp      = NormalizeOr(ctx.chassisRot * rp3d::Vector3(0.0, 1.0, 0.0), rp3d::Vector3(0.0, 1.0, 0.0));
    ctx.chassisForward = NormalizeOr(ctx.chassisRot * rp3d::Vector3(0.0, 0.0, 1.0), rp3d::Vector3(0.0, 0.0, 1.0));
    ctx.chassisRight   = NormalizeOr(ctx.chassisRot * rp3d::Vector3(1.0, 0.0, 0.0), rp3d::Vector3(1.0, 0.0, 0.0));

    ctx.vLinear  = ctx.chassis->getLinearVelocity();
    ctx.vAngular = ctx.chassis->getAngularVelocity();

    ctx.chassisMass = (float)ctx.chassis->getMass();
    if (ctx.chassisMass < 1.0f) ctx.chassisMass = 1.0f;

    ctx.speed = (float)ctx.vLinear.length();

    // Defaults (filled later)
    ctx.steeringCenter = 0.0f;
    ctx.steerFL = 0.0f;
    ctx.steerFR = 0.0f;

    ctx.driveForceTotal = 0.0f;
    ctx.brakeForceTotalFinal = 0.0f;
    ctx.handbrakeForceTotal = 0.0f;

    ctx.wheelCount = 0;
    ctx.poweredWheelCount = 0;
    ctx.frontWheelCount = 0;
    ctx.rearWheelCount = 0;

    ctx.frontLeft = -1;
    ctx.frontRight = -1;
    ctx.rearLeft = -1;
    ctx.rearRight = -1;

    ctx.midZ = 0.0f;
    ctx.weightPerWheel = 0.0f;

    return true;
}

void VehicleSystem::UpdateSteeringAndDrive(VehicleFrameContext& ctx) {

    Vehicle* vehicle = ctx.vehicle;

    float steerAlpha = vehicle->mSteeringSmoothRate * ctx.deltaTime;
    if (steerAlpha > 1.0f) steerAlpha = 1.0f;
    vehicle->mSteeringSmoothed = vehicle->mSteeringSmoothed + (vehicle->mSteeringInput - vehicle->mSteeringSmoothed) * steerAlpha;

    float steerT = (vehicle->mSteerHighSpeed > 0.001f) ? (ctx.speed / vehicle->mSteerHighSpeed) : 0.0f;
    if (steerT > 1.0f) steerT = 1.0f;

    float steerFactor = 1.0f - steerT * (1.0f - vehicle->mSteerMinFactor);
    ctx.steeringCenter = vehicle->mSteeringSmoothed * vehicle->mMaxSteeringAngle * steerFactor;

    float accelForce = vehicle->mMaxEnginePower * vehicle->mEngineForceInput;
    float decelForce = vehicle->mMaxBreakingPower * vehicle->mBrakeForceInput;
    ctx.handbrakeForceTotal = vehicle->mHandbrakeForceInput;

    float forwardSpeed = (float)(ctx.vLinear.dot(ctx.chassisForward));

    const float shiftSpeed = 0.8f;
    const float reverseScale = 0.70f;

    float driveForceTotal = 0.0f;
    float brakeForceTotalFinal = 0.0f;

    if (forwardSpeed > shiftSpeed) {
        driveForceTotal = accelForce;
        brakeForceTotalFinal = decelForce;
    }
    else if (forwardSpeed < -shiftSpeed) {
        driveForceTotal = -decelForce * reverseScale;
        brakeForceTotalFinal = accelForce;
    }
    else {
        if (accelForce > 0.0f && decelForce > 0.0f) {
            brakeForceTotalFinal = accelForce + decelForce;
        }
        else if (accelForce > 0.0f) {
            driveForceTotal = accelForce;
        }
        else if (decelForce > 0.0f) {
            driveForceTotal = -decelForce * reverseScale;
        }
    }

    ctx.driveForceTotal = driveForceTotal;
    ctx.brakeForceTotalFinal = brakeForceTotalFinal;
}

void VehicleSystem::ApplyAeroAndStability(VehicleFrameContext& ctx) {

    Vehicle* vehicle = ctx.vehicle;

    if (ctx.speed > 0.01f) {

        if (vehicle->mDragCoefficient > 0.0f) {
            float dragForceMag = vehicle->mDragCoefficient * ctx.speed * ctx.speed;
            rp3d::Vector3 dragDir = NormalizeOr(ctx.vLinear, ctx.chassisForward);
            ctx.chassis->applyWorldForceAtCenterOfMass(dragDir * rp3d::decimal(-dragForceMag));
        }

        if (vehicle->mDownforceCoefficient > 0.0f) {
            float downForceMag = vehicle->mDownforceCoefficient * ctx.speed * ctx.speed;
            ctx.chassis->applyWorldForceAtCenterOfMass(ctx.chassisUp * rp3d::decimal(-downForceMag));
        }
    }

    if (vehicle->mYawStability > 0.0f) {
        rp3d::decimal yawRate = ctx.vAngular.dot(ctx.chassisUp);
        ctx.chassis->applyWorldTorque(ctx.chassisUp * (rp3d::decimal(-vehicle->mYawStability) * yawRate));
    }
}

void VehicleSystem::PrepareWheelData(VehicleFrameContext& ctx) {

    Vehicle* vehicle = ctx.vehicle;

    const unsigned int wheelCount = (unsigned int)vehicle->mWheels.size();
    ctx.wheelCount = wheelCount;

    if (wheelCount == 0) {
        return;
    }

    if (mContactPoint.size() < wheelCount) {
        mContactPoint.resize(wheelCount);
        mContactNormal.resize(wheelCount);
        mWheelForward.resize(wheelCount);
        mWheelRight.resize(wheelCount);
        mNormalForce.resize(wheelCount);
        mCompression.resize(wheelCount);
        mInContact.resize(wheelCount);
    }

    if (mEdgeGrip.size() < wheelCount) {
        mEdgeGrip.resize(wheelCount);
    }

    float minZ =  9999999.0f;
    float maxZ = -9999999.0f;

    for (unsigned int w = 0; w < wheelCount; w++) {
        const VehicleWheel& wheel = vehicle->mWheels[w];
        if (wheel.mountPoint.z < minZ) minZ = wheel.mountPoint.z;
        if (wheel.mountPoint.z > maxZ) maxZ = wheel.mountPoint.z;
    }

    ctx.midZ = (minZ + maxZ) * 0.5f;

    ctx.frontLeft  = -1;
    ctx.frontRight = -1;
    ctx.rearLeft   = -1;
    ctx.rearRight  = -1;

    ctx.poweredWheelCount = 0;
    ctx.frontWheelCount   = 0;
    ctx.rearWheelCount    = 0;

    for (unsigned int w = 0; w < wheelCount; w++) {

        const VehicleWheel& wheel = vehicle->mWheels[w];

        const bool isFront = (wheel.mountPoint.z >= ctx.midZ);
        const bool isLeft  = (wheel.mountPoint.x < 0.0f);

        if (wheel.isPowered) ctx.poweredWheelCount++;

        if (isFront) {
            ctx.frontWheelCount++;
            if (isLeft) ctx.frontLeft = (int)w;
            else ctx.frontRight = (int)w;
        } else {
            ctx.rearWheelCount++;
            if (isLeft) ctx.rearLeft = (int)w;
            else ctx.rearRight = (int)w;
        }
    }

    if (ctx.poweredWheelCount == 0) ctx.poweredWheelCount = 1;
    if (ctx.frontWheelCount == 0) ctx.frontWheelCount = 1;
    if (ctx.rearWheelCount == 0) ctx.rearWheelCount = 1;

    float wheelCountF = (float)wheelCount;
    ctx.weightPerWheel = (ctx.chassisMass * ctx.gravityAccel) / wheelCountF;

    float wheelbase = vehicle->mAckermannWheelbase;
    float track     = vehicle->mAckermannTrackWidth;

    if (vehicle->mEnableAckermann && (wheelbase <= 0.001f || track <= 0.001f)) {
        if (ctx.frontLeft >= 0 && ctx.frontRight >= 0 && ctx.rearLeft >= 0 && ctx.rearRight >= 0) {
            float frontZ = (vehicle->mWheels[(unsigned int)ctx.frontLeft].mountPoint.z +
                            vehicle->mWheels[(unsigned int)ctx.frontRight].mountPoint.z) * 0.5f;
            float rearZ  = (vehicle->mWheels[(unsigned int)ctx.rearLeft].mountPoint.z +
                            vehicle->mWheels[(unsigned int)ctx.rearRight].mountPoint.z) * 0.5f;
            wheelbase = std::fabs(frontZ - rearZ);

            float leftX  = vehicle->mWheels[(unsigned int)ctx.frontLeft].mountPoint.x;
            float rightX = vehicle->mWheels[(unsigned int)ctx.frontRight].mountPoint.x;
            track = std::fabs(rightX - leftX);
        }
    }

    float steerFL = ctx.steeringCenter;
    float steerFR = ctx.steeringCenter;

    if (vehicle->mEnableAckermann && wheelbase > 0.001f && track > 0.001f) {
        float absSteer = std::fabs(ctx.steeringCenter);
        if (absSteer > 0.0005f) {
            float turnRadius = wheelbase / std::tan(absSteer);

            float inner = std::atan(wheelbase / (turnRadius - track * 0.5f));
            float outer = std::atan(wheelbase / (turnRadius + track * 0.5f));

            if (ctx.steeringCenter > 0.0f) {
                steerFR = inner;
                steerFL = outer;
            } else {
                steerFL = -inner;
                steerFR = -outer;
            }
        }
    }

    ctx.steerFL = steerFL;
    ctx.steerFR = steerFR;

    for (unsigned int w = 0; w < wheelCount; w++) {
        mNormalForce[w] = 0.0f;
        mCompression[w] = 0.0f;
        mInContact[w]   = 0;
        mEdgeGrip[w]    = 1.0f;
    }
}

void VehicleSystem::SolveWheelContactsAndSuspension(VehicleFrameContext& ctx) {

    Vehicle* vehicle = ctx.vehicle;

    if (ctx.wheelCount == 0) {
        return;
    }

    const rp3d::Vector3 suspUp(0.0, 1.0, 0.0);

    for (unsigned int wIndex = 0; wIndex < ctx.wheelCount; wIndex++) {

        VehicleWheel& wheel = vehicle->mWheels[wIndex];

        float wheelSteer = 0.0f;
        if (wheel.isSteering) {
            if ((int)wIndex == ctx.frontLeft)  wheelSteer = ctx.steerFL;
            else if ((int)wIndex == ctx.frontRight) wheelSteer = ctx.steerFR;
            else wheelSteer = ctx.steeringCenter;
        }

        float c = std::cos(wheelSteer);
        float s = std::sin(wheelSteer);

        rp3d::Vector3 wheelForward = ctx.chassisForward * rp3d::decimal(c) + ctx.chassisRight * rp3d::decimal(s);
        rp3d::Vector3 wheelRight   = ctx.chassisRight   * rp3d::decimal(c) - ctx.chassisForward * rp3d::decimal(s);

        wheelForward = NormalizeOr(wheelForward, ctx.chassisForward);
        wheelRight   = NormalizeOr(wheelRight, ctx.chassisRight);

        const float rayMargin  = 0.06f;
        const float surfaceEps = 0.02f;

        rp3d::Vector3 localConn(wheel.mountPoint.x, wheel.mountPoint.y, wheel.mountPoint.z);
        rp3d::Vector3 mountWorld = ctx.chassisTransform * localConn;

        float castStart  = wheel.suspensionMaxLength + wheel.radius + rayMargin;
        float castLength = castStart + wheel.suspensionMinLength + wheel.radius + 0.20f;

        const float minGroundNormalY = 0.35f;

        glm::vec3 patchPoint(0.0f);
        glm::vec3 patchNormal(0.0f, 1.0f, 0.0f);

        if (!SampleWheelContactPatch(mountWorld, wheelForward, wheelRight, castStart, castLength, minGroundNormalY, patchPoint, patchNormal)) {
            wheel.isInContact = false;
            wheel.suspensionVelocity = 0.0f;
            wheel.hasSmoothedContact = false;
            continue;
        }

        glm::vec3 prevSmoothN = wheel.smoothedNormal;

        float smoothRate = 30.0f;
        float alpha = 1.0f - std::exp(-smoothRate * ctx.deltaTime);
        alpha = ClampFloat(alpha, 0.0f, 1.0f);

        if (!wheel.hasSmoothedContact) {
            wheel.smoothedPoint  = patchPoint;
            wheel.smoothedNormal = patchNormal;
            wheel.hasSmoothedContact = true;
            prevSmoothN = wheel.smoothedNormal;
        } else {
            wheel.smoothedPoint = wheel.smoothedPoint + (patchPoint - wheel.smoothedPoint) * alpha;

            glm::vec3 n = wheel.smoothedNormal + (patchNormal - wheel.smoothedNormal) * alpha;
            float nLenSq = n.x*n.x + n.y*n.y + n.z*n.z;
            if (nLenSq > 0.000001f) {
                float inv = 1.0f / std::sqrt(nLenSq);
                n.x *= inv; n.y *= inv; n.z *= inv;
                wheel.smoothedNormal = n;
            } else {
                wheel.smoothedNormal = glm::vec3(0.0f, 1.0f, 0.0f);
            }
        }

        float ndot = prevSmoothN.x*wheel.smoothedNormal.x + prevSmoothN.y*wheel.smoothedNormal.y + prevSmoothN.z*wheel.smoothedNormal.z;
        ndot = ClampFloat(ndot, -1.0f, 1.0f);

        float edgeSlip = (ndot - 0.85f) / 0.15f;
        edgeSlip = ClampFloat(edgeSlip, 0.0f, 1.0f);
        edgeSlip = 0.25f + 0.75f * edgeSlip;
        mEdgeGrip[wIndex] = edgeSlip;

        glm::vec3 hitP = wheel.smoothedPoint;
        glm::vec3 hitN = wheel.smoothedNormal;
        if (hitN.y < 0.0f) {
            hitN = -hitN;
        }

        rp3d::Vector3 contactN(hitN.x, hitN.y, hitN.z);
        rp3d::Vector3 hitPoint(hitP.x, hitP.y, hitP.z);

        rp3d::Vector3 wheelCenterTarget = hitPoint + contactN * rp3d::decimal(wheel.radius);

        float suspLenUnclamped = (float)((mountWorld - wheelCenterTarget).dot(suspUp));

        float penetration = 0.0f;
        if (suspLenUnclamped < wheel.suspensionMinLength) {
            penetration = wheel.suspensionMinLength - suspLenUnclamped;
        }

        float suspLen = ClampFloat(suspLenUnclamped, wheel.suspensionMinLength, wheel.suspensionMaxLength);

        float prevLen = wheel.currentSuspensionLength;
        wheel.currentSuspensionLength = suspLen;
        wheel.suspensionVelocity = (wheel.currentSuspensionLength - prevLen) / ctx.deltaTime;

        float compression = wheel.suspensionRestLength - wheel.currentSuspensionLength;
        float springForce = compression * wheel.suspensionStiffness;

        float damperCoeff = (wheel.suspensionVelocity < 0.0f)
            ? wheel.suspensionDampingCompression
            : wheel.suspensionDampingRelaxation;

        float damperForce = -wheel.suspensionVelocity * damperCoeff;

        float suspForce = springForce + damperForce;
        if (suspForce < 0.0f) suspForce = 0.0f;

        if (penetration > 0.0f) {
            const float hardScale = 2.5f;
            float hardForce = penetration * wheel.suspensionStiffness * hardScale;

            rp3d::Vector3 r = hitPoint - ctx.chassisPos;
            rp3d::Vector3 vPoint = ctx.vLinear + ctx.vAngular.cross(r);
            float vUp = (float)vPoint.dot(suspUp);

            float hardDamp = (-vUp) * wheel.suspensionDampingCompression;

            suspForce += hardForce + hardDamp;
        }

        float maxSusp = ctx.weightPerWheel * vehicle->mMaxSuspensionWeightFactor;
        if (suspForce > maxSusp) suspForce = maxSusp;

        ctx.chassis->applyWorldForceAtWorldPosition(suspUp * rp3d::decimal(suspForce), hitPoint);

        rp3d::Vector3 contactPointRaised = hitPoint + contactN * rp3d::decimal(surfaceEps);

        wheel.isInContact = true;
        wheel.contactPoint  = glm::vec3(contactPointRaised.x, contactPointRaised.y, contactPointRaised.z);
        wheel.contactNormal = glm::vec3(hitN.x, hitN.y, hitN.z);

        mContactPoint[wIndex]  = contactPointRaised;
        mContactNormal[wIndex] = contactN;

        rp3d::Vector3 forwardInPlane = wheelForward - contactN * wheelForward.dot(contactN);
        forwardInPlane = NormalizeOr(forwardInPlane, ctx.chassisForward - contactN * ctx.chassisForward.dot(contactN));

        rp3d::Vector3 rightInPlane = contactN.cross(forwardInPlane);
        rightInPlane = NormalizeOr(rightInPlane, ctx.chassisRight);

        mWheelForward[wIndex] = forwardInPlane;
        mWheelRight[wIndex]   = rightInPlane;

        mNormalForce[wIndex] = suspForce;
        mCompression[wIndex] = compression;
        mInContact[wIndex] = 1;
    }
}

void VehicleSystem::ApplyAntiRoll(VehicleFrameContext& ctx) {

    Vehicle* vehicle = ctx.vehicle;

    if (ctx.wheelCount == 0) {
        return;
    }

    const rp3d::Vector3 suspUp(0.0, 1.0, 0.0);

    // Front
    if (ctx.frontLeft >= 0 && ctx.frontRight >= 0) {

        const int L = ctx.frontLeft;
        const int R = ctx.frontRight;

        if (mInContact[(unsigned int)L] || mInContact[(unsigned int)R]) {

            float diff = mCompression[(unsigned int)L] - mCompression[(unsigned int)R];
            float anti = diff * vehicle->mAntiRollFront;

            float maxAnti = ctx.weightPerWheel * 2.0f;
            anti = ClampFloat(anti, -maxAnti, maxAnti);

            if (mInContact[(unsigned int)L]) {
                ctx.chassis->applyWorldForceAtWorldPosition(suspUp * rp3d::decimal(-anti), mContactPoint[(unsigned int)L]);
            }
            if (mInContact[(unsigned int)R]) {
                ctx.chassis->applyWorldForceAtWorldPosition(suspUp * rp3d::decimal( anti), mContactPoint[(unsigned int)R]);
            }
        }
    }

    // Rear
    if (ctx.rearLeft >= 0 && ctx.rearRight >= 0) {

        const int L = ctx.rearLeft;
        const int R = ctx.rearRight;

        if (mInContact[(unsigned int)L] || mInContact[(unsigned int)R]) {

            float diff = mCompression[(unsigned int)L] - mCompression[(unsigned int)R];
            float anti = diff * vehicle->mAntiRollRear;

            float maxAnti = ctx.weightPerWheel * 2.0f;
            anti = ClampFloat(anti, -maxAnti, maxAnti);

            if (mInContact[(unsigned int)L]) {
                ctx.chassis->applyWorldForceAtWorldPosition(suspUp * rp3d::decimal(-anti), mContactPoint[(unsigned int)L]);
            }
            if (mInContact[(unsigned int)R]) {
                ctx.chassis->applyWorldForceAtWorldPosition(suspUp * rp3d::decimal( anti), mContactPoint[(unsigned int)R]);
            }
        }
    }
}

void VehicleSystem::SolveTireForces(VehicleFrameContext& ctx) {

    Vehicle* vehicle = ctx.vehicle;

    if (ctx.wheelCount == 0) {
        return;
    }

    for (unsigned int wIndex = 0; wIndex < ctx.wheelCount; wIndex++) {

        VehicleWheel& wheel = vehicle->mWheels[wIndex];
        if (!mInContact[wIndex]) continue;

        rp3d::Vector3 hitPoint = mContactPoint[wIndex];
        rp3d::Vector3 fwd   = mWheelForward[wIndex];
        rp3d::Vector3 right = mWheelRight[wIndex];

        rp3d::Vector3 r = hitPoint - ctx.chassisPos;
        rp3d::Vector3 vPoint = ctx.vLinear + ctx.vAngular.cross(r);

        rp3d::decimal vLong = vPoint.dot(fwd);
        rp3d::decimal vLat  = vPoint.dot(right);

        float N = mNormalForce[wIndex];
        if (N <= 0.0f) continue;
        
        float tractionLoss = 1.0f + (ctx.speed * ctx.speed * 400.0f);
        
        
        if (ctx.speed < 0.1f) {
            vehicle->mChassis->setLinearDamping(30.0f);
        } else {
            vehicle->mChassis->setLinearDamping(0.3f);
            
        }
        
        if (ctx.speed < 0.3f) {
            vehicle->mChassis->setAngularDamping(100.0f);
        } else {
            vehicle->mChassis->setAngularDamping(0.0f);
        }
        
        float tractionT = ctx.speed / tractionLoss;
        tractionT = ClampFloat(tractionT, 0.0f, 1.0f);
        
        const float lowLongMin = 0.25f;
        const float lowLongMax = 0.95f;

        float lowLong = lowLongMin + (lowLongMax - lowLongMin) * vehicle->mTractionMultiplier;
        float longTractionFactor = lowLong + (1.0f - lowLong) * tractionT;

        float maxFx = wheel.gripLongitudinal * N * longTractionFactor;
        float maxFy = wheel.gripLateral * N * mEdgeGrip[wIndex];

        const bool isFront = (vehicle->mWheels[wIndex].mountPoint.z >= ctx.midZ);

        float desiredFx = 0.0f;

        if (wheel.isPowered) {
            desiredFx += ctx.driveForceTotal / (float)ctx.poweredWheelCount;
        }

        float brakeShare = isFront ? vehicle->mBrakeBiasFront : (1.0f - vehicle->mBrakeBiasFront);
        float perAxleBrake = ctx.brakeForceTotalFinal * brakeShare;
        float perWheelBrake = isFront ? (perAxleBrake / (float)ctx.frontWheelCount) : (perAxleBrake / (float)ctx.rearWheelCount);

        if ((float)vLong > 0.05f) desiredFx -= perWheelBrake;
        else if ((float)vLong < -0.05f) desiredFx += perWheelBrake;
        else desiredFx += (float)(-vLong) * ctx.chassisMass * 2.0f;

        if (!isFront && ctx.handbrakeForceTotal != 0.0f) {
            float perWheelHB = ctx.handbrakeForceTotal / (float)ctx.rearWheelCount;
            if ((float)vLong > 0.05f) desiredFx -= perWheelHB;
            else if ((float)vLong < -0.05f) desiredFx += perWheelHB;
        }

        float vLongAbs = (float)std::fabs((double)vLong);
        float slipAngle = std::atan2((float)vLat, vLongAbs + 0.5f);

        float desiredFy = -slipAngle * wheel.corneringStiffness;

        float nx = 0.0f;
        float ny = 0.0f;

        if (maxFx > 0.0001f) nx = desiredFx / maxFx;
        if (maxFy > 0.0001f) ny = desiredFy / maxFy;

        float ellipse = nx*nx + ny*ny;
        if (ellipse > 1.0f) {
            float scale = 1.0f / std::sqrt(ellipse);
            desiredFx *= scale;
            desiredFy *= scale;
        }

        desiredFx = ClampFloat(desiredFx, -maxFx, maxFx);
        desiredFy = ClampFloat(desiredFy, -maxFy, maxFy);

        rp3d::Vector3 tireForce = fwd * rp3d::decimal(desiredFx) + right * rp3d::decimal(desiredFy);
        ctx.chassis->applyWorldForceAtWorldPosition(tireForce, hitPoint);
    }
}

void VehicleSystem::RenderVehicle(VehicleFrameContext& ctx) {
    Vehicle* vehicle = ctx.vehicle;
    if (vehicle == NULL) return;
    
    const glm::vec3 chassisPos = glm::vec3(ctx.chassisPos.x, ctx.chassisPos.y, ctx.chassisPos.z);
    
    glm::quat chassisRot = glm::quat(ctx.chassisRot.w, ctx.chassisRot.x, ctx.chassisRot.y, ctx.chassisRot.z);
    chassisRot = glm::normalize(chassisRot);
    
    const glm::mat3 chassisRotMtx = glm::mat3_cast(chassisRot);
    
    if (vehicle->mPlayerController != nullptr) {
        Transform* transform = vehicle->mPlayerController->GetComponent<Transform>();
        transform->position = chassisPos;
    }
    
    const unsigned int wheelCount = (unsigned int)vehicle->mWheels.size();
    for (unsigned int wIndex = 0; wIndex < wheelCount; wIndex++) {
        
        VehicleWheel& wheel = vehicle->mWheels[wIndex];
        if (wheel.wheelRenderer == NULL) continue;
        
        float wheelSteerRad = 0.0f;
        if (wheel.isSteering) {
            if ((int)wIndex == ctx.frontLeft)       wheelSteerRad = ctx.steerFL;
            else if ((int)wIndex == ctx.frontRight) wheelSteerRad = ctx.steerFR;
            else                                    wheelSteerRad = ctx.steeringCenter;
        }
        
        const glm::vec3 mountWorld   = chassisPos + (chassisRotMtx * wheel.mountPoint);
        const glm::vec3 suspDirWorld = chassisRotMtx * glm::vec3(0.0f, -1.0f, 0.0f);
        
        float springLen = wheel.currentSuspensionLength;
        if (springLen <= 0.0f) springLen = wheel.suspensionRestLength;
        
        if (wheel.suspensionMaxLength > wheel.suspensionMinLength) {
            if (springLen < wheel.suspensionMinLength) springLen = wheel.suspensionMinLength;
            if (springLen > wheel.suspensionMaxLength) springLen = wheel.suspensionMaxLength;
        }
        
        const glm::vec3 wheelWorldPos = mountWorld + (suspDirWorld * (springLen + wheel.radius));
        
        glm::vec3 renderPos = wheelWorldPos;
        renderPos.y += wheel.radius;
        
        rp3d::Vector3 fwd = ctx.chassisForward;
        
        if (mWheelForward.size() > wIndex && mInContact.size() > wIndex && mInContact[wIndex]) {
            fwd = mWheelForward[wIndex];
        } else {
            float c = std::cos(wheelSteerRad);
            float s = std::sin(wheelSteerRad);
            fwd = NormalizeOr(ctx.chassisForward * rp3d::decimal(c) + ctx.chassisRight * rp3d::decimal(s), ctx.chassisForward);
        }
        
        rp3d::Vector3 wheelCenterWorld(renderPos.x, renderPos.y, renderPos.z);
        rp3d::Vector3 r = wheelCenterWorld - ctx.chassisPos;
        rp3d::Vector3 vPoint = ctx.vLinear + ctx.vAngular.cross(r);
        
        float vLong = (float)vPoint.dot(fwd);
        
        const bool doSpin = true;
        
        if (doSpin && wheel.radius > 0.001f) {
            float spinDegPerSec = glm::degrees(-vLong / wheel.radius);
            
            wheel.mRotationAnimation += spinDegPerSec * ctx.deltaTime;
            
            if (wheel.mRotationAnimation >  360.0f || wheel.mRotationAnimation < -360.0f) {
                wheel.mRotationAnimation = std::fmod(wheel.mRotationAnimation, 360.0f);
            }
        }
        
        wheel.wheelRenderer->transform.SetIdentity();
        wheel.wheelRenderer->transform.position.x = renderPos.x;
        wheel.wheelRenderer->transform.position.y = renderPos.y + wheel.mountHeight;
        wheel.wheelRenderer->transform.position.z = renderPos.z;
        
        wheel.wheelRenderer->transform.rotation.x = chassisRot.x;
        wheel.wheelRenderer->transform.rotation.y = chassisRot.y;
        wheel.wheelRenderer->transform.rotation.z = chassisRot.z;
        wheel.wheelRenderer->transform.rotation.w = chassisRot.w;
        
        if (wheel.isSteering) {
            wheel.wheelRenderer->transform.RotateEuler(0.0f, glm::degrees(wheelSteerRad), 0.0f);
        }
        
        wheel.wheelRenderer->transform.RotateEuler(
            wheel.mountRotation.x,
            wheel.mountRotation.y,
            wheel.mountRotation.z
        );
        
        if (doSpin) wheel.wheelRenderer->transform.RotateEuler(0.0f, -wheel.mRotationAnimation, 0.0f);
        
        wheel.wheelRenderer->transform.scale = wheel.mountScale * (wheel.radius * glm::pi<float>());
        
        wheel.wheelRenderer->transform.UpdateMatrix();
    }
    
    for (unsigned int i = 0; i < vehicle->mRenderers.size(); i++) {
        MeshRenderer* renderer = vehicle->mRenderers[i];
        if (renderer == NULL) continue;
        
        renderer->transform.position = chassisPos;
        
        renderer->transform.rotation.x = ctx.chassisRot.x;
        renderer->transform.rotation.y = ctx.chassisRot.y;
        renderer->transform.rotation.z = ctx.chassisRot.z;
        renderer->transform.rotation.w = ctx.chassisRot.w;
        
        renderer->transform.UpdateMatrix();
    }
}

