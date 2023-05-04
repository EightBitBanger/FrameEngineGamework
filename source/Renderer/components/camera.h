#ifndef __COMPONENT_CAMERA
#define __COMPONENT_CAMERA

#include "../types/transform.h"

#include "../../Input/InputSystem.h"


class Camera {
    
public:
    
    /// Transformation matrix.
    Transform transform;
    
    /// Up angle orientation reference.
    glm::vec3 up;
    /// Forward looking angle.
    glm::vec3 forward;
    /// Right angle to the cameras forward angle.
    glm::vec3 right;
    
    /// Whether mouse look is calculated on update.
    bool useMouseLook;
    
    /// Speed of mo
    float cameraSpeed;
    
    /// Field of view.
    float fov;
    /// View aspect ratio.
    float aspect;
    /// Near camera clipping distance.
    float clipNear;
    /// Far camera clipping distance.
    float clipFar;
    
    /// Maximum pitch angle limit. (in radians)
    int MouseLimitPitchMax;
    /// Minimum pitch angle limit. (in radians)
    int MouseLimitPitchMin;
    
    /// Mouse horizontal movement sensitivity. (in radians)
    float MouseSensitivityYaw;
    /// Mouse vertical movement sensitivity. (in radians)
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


