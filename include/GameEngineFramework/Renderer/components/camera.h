#ifndef __COMPONENT_CAMERA
#define __COMPONENT_CAMERA

#include <GameEngineFramework/Engine/types/viewport.h>
#include <GameEngineFramework/Transform/Transform.h>

#include <GameEngineFramework/Input/InputSystem.h>


class ENGINE_API Camera {
    
public:
    
    /// Transformation matrix.
    Transform transform;
    
    
    /// Up angle orientation reference.
    glm::vec3 up;
    
    /// Forward looking angle.
    glm::vec3 forward;
    
    /// Right angle to the cameras forward angle.
    glm::vec3 right;
    
    
    /// Camera view angle.
    Viewport viewport;
    
    
    /// Should mouse look be calculated on update.
    bool useMouseLook;
    
    /// Should the camera aspect be synchronized to the window aspect.
    bool isFixedAspect;
    
    /// Should the camera be rendered orthographically or perspective.
    bool isOrthographic;
    
    /// Camera mouse looking angle.
    glm::vec2 mouseLookAngle;
    
    /// Current camera looking angles in degrees.
    glm::vec2 lookAngle;
    
    
    /// Field of view.
    float fov;
    
    /// View aspect ratio.
    float aspect;
    
    /// Near camera clipping distance.
    float clipNear;
    
    /// Far camera clipping distance.
    float clipFar;
    
    
    /// How far in degrees the frustum angles should overlap when culling.
    float frustumOverlap;
    
    /// Distance behind the camera where culling should begin.
    /// This prevents clipping when culling large objects.
    float frustumOffset;
    
    
    /// Maximum pitch angle limit. (in radians)
    int mouseLimitPitchMax;
    
    /// Minimum pitch angle limit. (in radians)
    int mouseLimitPitchMin;
    
    
    /// Mouse horizontal movement sensitivity. (in radians)
    double mouseSensitivityYaw;
    
    /// Mouse vertical movement sensitivity. (in radians)
    double mouseSensitivityPitch;
    
    
    /// Enable mouse look.
    void EnableMouseLook(void);
    
    /// Disable mouse look.
    void DisableMouseLook(void);
    
    
    /// Get the camera pitch angle.
    float GetPitch(void);
    
    /// Get the camera yaw angle.
    float GetYaw(void);
    
    
    friend class RenderSystem;
    
    Camera();
    
    
private:
    
    
    
};


#endif


