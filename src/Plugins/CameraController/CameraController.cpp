#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/Engine.h>

#include <GameEngineFramework/Plugins/plugins.h>

#include <GameEngineFramework/Plugins/CameraController/CameraController.h>

GameObject* CreateCameraController(glm::vec3 position) {
    GameObject* cameraController = Engine.Create<GameObject>();
    cameraController->AddComponent(Engine.CreateComponent<Camera>());
    cameraController->AddComponent(Engine.CreateComponent<rp3d::RigidBody>());
    
    rp3d::RigidBody* rigidBody = cameraController->GetComponent<RigidBody>();
    
    rp3d::Vector3 bodyPosition(position.x, position.y, position.z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    
    rp3d::Transform bodyTransform(bodyPosition, quat);
    rigidBody->setTransform(bodyTransform);
    
    cameraController->SetAngularAxisLockFactor(0, 0, 0);
    cameraController->SetPosition(position);
    
    Engine.cameraController = cameraController;
    Engine.sceneMain->camera = Engine.cameraController->GetComponent<Camera>();
    
    Engine.sceneMain->camera->mouseSensitivityPitch = 2.0f;
    Engine.sceneMain->camera->mouseSensitivityYaw   = 2.0f;
    
    Engine.sceneMain->camera->clipNear = 0.8f;
    
    rigidBody->enableGravity(false);
    
    // Create a box collider for collisions.
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(glm::vec3(1, 1, 1));
    rp3d::Collider* collider = Engine.cameraController->AddColliderBox(boxShape, 0, 0, 0);
    collider->setCollisionCategoryBits((unsigned short)LayerMask::Ground);
    
    rigidBody->updateMassPropertiesFromColliders();
    Engine.cameraController->SetLinearDamping(7.0f);
    
    return cameraController;
}

void CameraControllerUpdate(void) {
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera == nullptr) 
        return;
        
    const float forceAccelerate = 42.0f;
    const float fovMaxPullback  = 3.0f;
    const float fovBase         = 60.0f;
    
    const float fovSmoothFactor = 0.14f; 
    
    glm::vec3 force(0);
    
    // No movement when paused
    if (!Platform.isPaused) {
        // WASD Directional
        if (Input.CheckKeyCurrent(VK_W)) { force += mainCamera->forward; }
        if (Input.CheckKeyCurrent(VK_S)) { force -= mainCamera->forward; }
        if (Input.CheckKeyCurrent(VK_A)) { force -= mainCamera->right; }
        if (Input.CheckKeyCurrent(VK_D)) { force += mainCamera->right; }
        
        // Space/Shift Elevation
        if (Input.CheckKeyCurrent(VK_SPACE)) { force += mainCamera->up; }
        if (Input.CheckKeyCurrent(VK_SHIFT)) { force -= mainCamera->up; }
    }
    
    force *= forceAccelerate * forceAccelerate;
    Engine.cameraController->AddForce(force.x, force.y, force.z);
    
    rp3d::RigidBody* rigidBody = Engine.cameraController->GetComponent<rp3d::RigidBody>();
    rp3d::Vector3 forceVec3 = rigidBody->getLinearVelocity();
    glm::vec3 velocity = glm::vec3(forceVec3.x, forceVec3.y, forceVec3.z);
    
    // Calculate forward velocity component using Dot Product
    float forwardSpeed = glm::dot(velocity, mainCamera->forward);
    
    // Only pull back when moving forward
    float targetPullback = 0.0f;
    if (forwardSpeed > 0.0f) {
        targetPullback = forwardSpeed * 0.1f;
        if (targetPullback > fovMaxPullback) {
            targetPullback = fovMaxPullback;
        }
    }
    
    // Smoothly interpolate (lerp) from current FOV to target FOV
    float targetFov = fovBase + targetPullback;
    mainCamera->fov = glm::mix(mainCamera->fov, targetFov, fovSmoothFactor);
}
