#include "camera.h"

Camera::Camera() {
    
    up      = glm::vec3(0, 1, 0);
    forward = glm::vec3(0, 0, 0);
    right   = glm::vec3(0, 0, 0);
    
    script = nullptr;
    rigidBody = nullptr;
    
    useMovementKeys = false;
    useMouseLook = false;
    
    cameraSpeed = 100;
    
    fov       = 60;
    aspect    = 1.33;
    clipNear  = 0.03;
    clipFar   = 1000000;
    
    MouseLimitPitchMax = 180;
    MouseLimitPitchMin = 180;
    
    MouseSensitivityYaw   = 0.004;
    MouseSensitivityPitch = 0.0038;
    
}

void Camera::SetMouseCenter(int mouseResetX, int mouseResetY) {CursorSetPosition(mouseResetX, mouseResetY);}
void Camera::EnableMouseLook(void)  {ShowCursor(false); useMouseLook = true;}
void Camera::DisableMouseLook(void) {ShowCursor(true); useMouseLook = false;}

void Camera::MouseLook(float deltaTime, int mouseResetX, int mouseResetY) {
    
    glm::vec2 MousePos = CursorGetPosition();
    CursorSetPosition(mouseResetX, mouseResetY);
    
    float MouseDiffX = (MousePos.x - mouseResetX) * MouseSensitivityYaw * deltaTime;
    float MouseDiffY = (MousePos.y - mouseResetY) * MouseSensitivityPitch * deltaTime;
    
    transform.rotation.x += (float)MouseDiffX;
    transform.rotation.y -= (float)MouseDiffY;
    
    // Yaw limit
    if (transform.rotation.x >= 0.109655)  {transform.rotation.x -= 0.109655;}
    if (transform.rotation.x <= 0.109655)  {transform.rotation.x += 0.109655;}
    
    // Pitch limit
    if (transform.rotation.y >  0.0274f) transform.rotation.y =  0.0274f;
    if (transform.rotation.y < -0.0274f) transform.rotation.y = -0.0274f;
    
    return;
}


float Camera::GetPitch(void) {
    return glm::degrees(glm::asin(transform.rotation.y));
}

float Camera::GetYaw(float pitch) {
    return glm::degrees(glm::acos(transform.rotation.x / cos(glm::radians(pitch))));
}


glm::mat4 Camera::CalculateView(void) {
    
    glm::vec3 pos;
    pos.x = transform.position.x;
    pos.y = transform.position.y;
    pos.z = transform.position.z;
    
    forward.x = cos( transform.rotation.x * 180 / glm::pi<float>() );
    forward.y = tan( transform.rotation.y * 180 / glm::pi<float>() );
    forward.z = sin( transform.rotation.x * 180 / glm::pi<float>() );
    
    forward = glm::normalize(forward);
    
    glm::vec3 angle;
    angle.x = transform.position.x + forward.x;
    angle.y = transform.position.y + forward.y;
    angle.z = transform.position.z + forward.z;
    
    right = glm::normalize(glm::cross(up, forward));
    
    glm::mat4 view = glm::lookAt(pos, angle, up);
    
    return view;
}

glm::mat4 Camera::CalculatePerspectiveMatrix(void) {
    
    glm::mat4 projection = glm::perspective( glm::radians( fov ), aspect, clipNear, clipFar);
    
    return projection;
}

glm::mat4 Camera::CalculateOrthiographicMatrix(Viewport viewport) {
    
    
    // \/ sus* O_o \/
    
    glm::mat4 projection = glm::ortho(viewport.x, viewport.w, viewport.h, viewport.y, -1, 1);
    
    return projection;
}


// Camera physics

void Camera::AddForce(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 nudge = rp3d::Vector3(x, y, z);
    rigidBody->applyLocalForceAtCenterOfMass(nudge);
    return;
}

void Camera::AddTorque(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 twist = rp3d::Vector3(x, y, z);
    rigidBody->applyLocalTorque(twist);
    return;
}

void Camera::SetMass(float mass) {
    assert(rigidBody != nullptr);
    rigidBody->setMass(mass);
}

void Camera::SetLinearDamping(float damping) {
    assert(rigidBody != nullptr);
    rigidBody->setLinearDamping(damping);
}

void Camera::SetAngularDamping(float damping) {
    assert(rigidBody != nullptr);
    rigidBody->setAngularDamping(damping);
}

void Camera::EnableGravity(bool enabled) {
    assert(rigidBody != nullptr);
    rigidBody->enableGravity(enabled);
}


void Camera::CalculatePhysics(void) {
    assert(rigidBody != nullptr);
    rigidBody->updateMassFromColliders();
    rigidBody->updateLocalCenterOfMassFromColliders();
    rigidBody->updateLocalInertiaTensorFromColliders();
    return;
}


void Camera::SetLinearAxisLockFactor(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBody->setLinearLockAxisFactor(lockFactor);
    return;
}

void Camera::SetAngularAxisLockFactor(float x, float y, float z) {
    assert(rigidBody != nullptr);
    rp3d::Vector3 lockFactor(x, y, z);
    rigidBody->setAngularLockAxisFactor(lockFactor);
    return;
}


void Camera::AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z) {
    assert(rigidBody != nullptr);
    assert(boxShape != nullptr);
    
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBody->addCollider(boxShape, offsetTransform);
    
    return;
}

void Camera::AddCollider(ColliderTag* colliderTag, float x, float y, float z) {
    assert(rigidBody != nullptr);
    assert(colliderTag != nullptr);
    if (colliderTag->isStatic) {
        rigidBody->setType(rp3d::BodyType::STATIC);
    } else {
        rigidBody->setType(rp3d::BodyType::DYNAMIC);
    }
    rp3d::Transform offsetTransform;
    offsetTransform.setPosition(rp3d::Vector3(x, y, z));
    
    rigidBody->addCollider(colliderTag->colliderShape, offsetTransform);
    
    return;
}


void Camera::SetRigidBodyStatic(void) {
    assert(rigidBody != nullptr);
    rigidBody->setType(rp3d::BodyType::STATIC);
    return;
}


void Camera::SetRigidBodyDynamic(void) {
    assert(rigidBody != nullptr);
    rigidBody->setType(rp3d::BodyType::DYNAMIC);
    return;
}





