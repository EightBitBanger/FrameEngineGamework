#include <GameEngineFramework/Engine/EngineSystems.h>
#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Plugins/plugins.h>
#include <GameEngineFramework/Plugins/CameraController/CameraController.h>

CameraType globalCameraType = CameraType::Free;

// RTS Camera Settings
namespace RTSConfig {
    const float panSpeed       = 55.0f;
    const float zoomSpeed      = 120.0f;
    const float minHeight      = 1.0f;
    const float maxHeight      = 60.0f;
    const float defaultFov     = 60.0f;
    const float edgeScrollZone = 15.0f;
}

namespace FlyCamConfig {
    const float moveSpeed      = 24.0f; // World units per second
    const float responsiveness = 0.1f; // Velocity smoothing (1.0 = instant, lower = smoother)
    const float defaultFov     = 60.0f;
}

GameObject* CreateCameraController(glm::vec3 position, CameraType type) {
    globalCameraType = type;
    
    GameObject* cameraController = Engine.Create<GameObject>();
    cameraController->AddComponent(Engine.CreateComponent<Camera>());
    cameraController->AddComponent(Engine.CreateComponent<rp3d::RigidBody>());
    
    rp3d::RigidBody* rigidBody = cameraController->GetComponent<rp3d::RigidBody>();
    
    rp3d::Vector3 bodyPosition(position.x, position.y, position.z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    rp3d::Transform bodyTransform(bodyPosition, quat);
    rigidBody->setTransform(bodyTransform);
    
    cameraController->SetAngularAxisLockFactor(0, 0, 0);
    cameraController->SetPosition(position);
    
    Engine.cameraController = cameraController;
    Engine.sceneMain->camera = cameraController->GetComponent<Camera>();
    
    Engine.sceneMain->camera->mouseSensitivityPitch = 2.0f;
    Engine.sceneMain->camera->mouseSensitivityYaw   = 2.0f;
    Engine.sceneMain->camera->clipNear              = 0.8f;
    Engine.sceneMain->camera->fov                   = FlyCamConfig::defaultFov;
    
    // Disable physics gravity and damping interference
    rigidBody->enableGravity(false);
    cameraController->SetLinearDamping(0.0f);
    
    // NOTE: Do not attach a ground box collider here. 
    // Physical colliders on fly cameras cause catastrophic contact repulsion with terrain.
    
    return cameraController;
}

static void UpdateFreeCamera(Camera* mainCamera) {
    static glm::vec3 currentVelocity(0.0f);
    glm::vec3 moveDir(0.0f);
    
    if (!Platform.isPaused) {
        // True 6DOF Fly Movement: W/S follow view vector, A/D strafe, Space/Shift elevate
        if (Input.CheckKeyCurrent(VK_W))     { moveDir += mainCamera->forward; }
        if (Input.CheckKeyCurrent(VK_S))     { moveDir -= mainCamera->forward; }
        if (Input.CheckKeyCurrent(VK_A))     { moveDir -= mainCamera->right; }
        if (Input.CheckKeyCurrent(VK_D))     { moveDir += mainCamera->right; }
        if (Input.CheckKeyCurrent(VK_SPACE)) { moveDir += glm::vec3(0.0f, 1.0f, 0.0f); }
        if (Input.CheckKeyCurrent(VK_SHIFT)) { moveDir -= glm::vec3(0.0f, 1.0f, 0.0f); }
    }
    
    glm::vec3 targetVelocity(0.0f);
    if (glm::length(moveDir) > 0.0001f) {
        targetVelocity = glm::normalize(moveDir) * FlyCamConfig::moveSpeed;
    }
    
    // Smooth velocity towards target; glides to a clean stop on key release
    currentVelocity = glm::mix(currentVelocity, targetVelocity, FlyCamConfig::responsiveness);
    
    rp3d::RigidBody* rigidBody = Engine.cameraController->GetComponent<rp3d::RigidBody>();
    rigidBody->setLinearVelocity(rp3d::Vector3(currentVelocity.x, currentVelocity.y, currentVelocity.z));
}

static void UpdateRTSCamera(Camera* mainCamera) {
    glm::vec3 targetVelocity(0.0f);
    
    if (!Platform.isPaused) {
        glm::vec3 flatForward(mainCamera->forward.x, 0.0f, mainCamera->forward.z);
        glm::vec3 flatRight(mainCamera->right.x, 0.0f, mainCamera->right.z);
        
        if (glm::length(flatForward) > 0.0001f) flatForward = glm::normalize(flatForward);
        if (glm::length(flatRight) > 0.0001f)   flatRight   = glm::normalize(flatRight);
        
        glm::vec3 panDir(0.0f);
        if (Input.CheckKeyCurrent(VK_W) || Input.CheckKeyCurrent(VK_UP))    { panDir += flatForward; }
        if (Input.CheckKeyCurrent(VK_S) || Input.CheckKeyCurrent(VK_DOWN))  { panDir -= flatForward; }
        if (Input.CheckKeyCurrent(VK_A) || Input.CheckKeyCurrent(VK_LEFT))  { panDir -= flatRight; }
        if (Input.CheckKeyCurrent(VK_D) || Input.CheckKeyCurrent(VK_RIGHT)) { panDir += flatRight; }
        
        if (glm::length(panDir) > 0.0001f) {
            targetVelocity += glm::normalize(panDir) * RTSConfig::panSpeed;
        }
        
        // Vertical elevation
        if (Input.CheckKeyCurrent(VK_SPACE)) { targetVelocity.y += RTSConfig::panSpeed; }
        if (Input.CheckKeyCurrent(VK_SHIFT)) { targetVelocity.y -= RTSConfig::panSpeed; }
        
        // Scroll zoom along look vector
        if (!Input.CheckKeyCurrent(VK_CONTROL) && Input.mouseWheelDelta != 0.0f) {
            targetVelocity += mainCamera->forward * (Input.mouseWheelDelta * RTSConfig::zoomSpeed);
            Input.mouseWheelDelta = 0.0f;
        }
    }
    
    rp3d::RigidBody* rigidBody = Engine.cameraController->GetComponent<rp3d::RigidBody>();
    rigidBody->setLinearVelocity(rp3d::Vector3(targetVelocity.x, targetVelocity.y, targetVelocity.z));
    
    // Height clamping
    rp3d::Transform currentTransform = rigidBody->getTransform();
    rp3d::Vector3 currentPos = currentTransform.getPosition();
    
    if (currentPos.y < RTSConfig::minHeight) {
        currentPos.y = RTSConfig::minHeight;
        currentTransform.setPosition(currentPos);
        rigidBody->setTransform(currentTransform);
    } else if (currentPos.y > RTSConfig::maxHeight) {
        currentPos.y = RTSConfig::maxHeight;
        currentTransform.setPosition(currentPos);
        rigidBody->setTransform(currentTransform);
    }
}

void CameraControllerUpdate(void) {
    Camera* mainCamera = Engine.sceneMain->camera;
    if (mainCamera == nullptr) 
        return;
    
    // Shared mouse look controls (Hold Right Mouse to look)
    /*
    if (Input.CheckMouseRightPressed()) {
        Input.ClearMouseRight();
        Platform.HideMouseCursor();
        mainCamera->EnableMouseLook();
        Renderer.displayCenter.x = Input.mouseX;
        Renderer.displayCenter.y = Input.mouseY;
    }
    if (Input.CheckMouseRightReleased()) {
        Platform.ShowMouseCursor();
        mainCamera->DisableMouseLook();
    }
    */
    
    if (globalCameraType == CameraType::RTS) {
        UpdateRTSCamera(mainCamera);
    } else {
        UpdateFreeCamera(mainCamera);
    }
}
