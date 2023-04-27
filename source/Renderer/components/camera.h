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
    
    /// Script component pointer.
    Script* script;
    
    /// Rigid body component pointer.
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
    
    /// Set the position the mouse will reset to on mouse look update.
    void SetMouseCenter(int mouseResetX, int mouseResetY);
    
    /// Enable mouse look.
    void EnableMouseLook(void);
    /// Disable mouse look.
    void DisableMouseLook(void);
    /// Update the mouse look state by the deltaTime. The mouse will reset to mouseResetX and mouseResetY.
    void MouseLook(float deltaTime, int mouseResetX, int mouseResetY);
    
    
    glm::mat4 CalculateView(void);
    glm::mat4 CalculatePerspectiveMatrix(void);
    glm::mat4 CalculateOrthiographicMatrix(Viewport viewport);
    
    /// Add directional force to the associated rigid body.
    void AddForce(float x, float y, float z);
    /// Add rotational torque to the associated rigid body.
    void AddTorque(float x, float y, float z);
    
    /// Add a collider from a resource tag at the offset position x, y, z.
    void AddCollider(ColliderTag* colliderTag, float x, float y, float z);
    /// Add a box collider shape at the offset position x, y, z.
    void AddColliderBox(rp3d::BoxShape* boxShape, float x, float y, float z);
    
    /// Set the rigid body mass.
    void SetMass(float mass);
    /// Set the physical resistance to movement.
    void SetLinearDamping(float damping);
    /// Set the physical resistance to rotation.
    void SetAngularDamping(float damping);
    
    /// Set the lock multiplier for linear movement.
    void SetLinearAxisLockFactor(float x, float y, float z);
    /// Set the lock multiplier for angular rotation.
    void SetAngularAxisLockFactor(float x, float y, float z);
    
    /// Set the gravity state.
    void EnableGravity(bool enabled);
    
    /// Calculate the physical weight proportions by the size and offset of colliders.
    void CalculatePhysics(void);
    
    /// Make the rigid body non movable.
    void SetRigidBodyStatic(void);
    /// Make the rigid body dynamicly movable.
    void SetRigidBodyDynamic(void);
    
};

#endif
