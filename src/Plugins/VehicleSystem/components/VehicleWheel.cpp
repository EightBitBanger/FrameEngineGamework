#include <GameEngineFramework/Plugins/VehicleSystem/components/VehicleWheel.h>

VehicleWheel::VehicleWheel() :
    mountPoint(0.0f, 0.0f, 0.0f),
    mountRotation(0.0f, 0.0f, 0.0f),
    mountScale(1.0f, 1.0f, 1.0f),
    radius(0.35f),
    mountHeight(0.0f),
    
    wheelRenderer(nullptr),
    
    suspensionRestLength(0.40f),
    suspensionMinLength(0.10f),
    suspensionMaxLength(0.60f),
    
    suspensionStiffness(18000.0f),
    suspensionDampingCompression(2600.0f),
    suspensionDampingRelaxation(3200.0f),
    
    gripLongitudinal(1.0f),
    gripLateral(1.0f),
    
    corneringStiffness(7000.0f),
    
    isPowered(false),
    isSteering(false),
    isHandbrake(false),
    
    isInContact(false),
    currentSuspensionLength(0.0f),
    suspensionVelocity(0.0f),
    
    contactPoint(0.0f, 0.0f, 0.0f),
    contactNormal(0.0f, 1.0f, 0.0f),
    
    hasSmoothedContact(false),
    smoothedPoint({0.0f, 0.0f, 0.0f}),
    smoothedNormal({0.0f, 1.0f, 0.0f}),
    
    mRotationAnimation(0.0f)
{}
