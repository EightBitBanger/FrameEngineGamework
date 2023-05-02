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

#include "../../Scripting/components/script.h"
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
    
    /// Get the camera pitch angle.
    float GetPitch(void);
    /// Get the camera yaw angle.
    float GetYaw(float pitch);
    
    
    
private:
    
};


#endif


