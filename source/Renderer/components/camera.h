#ifndef COMPONENT_CAMERA
#define COMPONENT_CAMERA

#include <SDKDDKVer.h>
#include <windows.h>

#define GLEW_STATIC
#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../types/bufferlayout.h"
#include "../types/transform.h"
#include "../types/color.h"
#include "../types/viewport.h"

#include "script.h"

#include "../../Physics/PhysicsSystem.h"

#include "../../Resources/assets/colliderTag.h"

#include "../../Input/InputSystem.h"
#include "../../std/timer.h"

class Camera {
    
public:
    
    Transform transform;
    
    glm::vec3 up;
    glm::vec3 forward;
    glm::vec3 right;
    
    Script* script;
    
    rp3d::RigidBody* rigidBody;
    
    bool useMovementKeys;
    bool useMouseLook;
    
    float cameraSpeed;
    
    float fov;
    float aspect;
    float clipNear;
    float clipFar;
    
    int MouseLimitPitchMax;
    int MouseLimitPitchMin;
    
    float MouseSensitivityYaw;
    float MouseSensitivityPitch;
    
    Camera();
    
    void SetMouseCenter(int mouseResetX, int mouseResetY);
    
    void EnableMouseLook(void);
    void DisableMouseLook(void);
    
    void MouseLook(float deltaTime, int mouseResetX, int mouseResetY);
    
    
    glm::mat4 CalculateView(void);
    glm::mat4 CalculatePerspectiveMatrix(void);
    glm::mat4 CalculateOrthiographicMatrix(Viewport viewport);
    
    
    void AddForce(float x, float y, float z);
    void AddTorque(float x, float y, float z);
    
    void AddCollider(ColliderTag* colliderTag, float x, float y, float z);
    void AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z);
    
    void SetMass(float mass);
    void SetLinearDamping(float damping);
    void SetAngularDamping(float damping);
    
    void SetLinearAxisLockFactor(float x, float y, float z);
    void SetAngularAxisLockFactor(float x, float y, float z);
    
    void EnableGravity(bool enabled);
    
    void CalculatePhysics(void);
    
    void SetRigidBodyStatic(void);
    void SetRigidBodyDynamic(void);
    
};

#endif
