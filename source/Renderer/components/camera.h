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

#include "../../Input/InputSystem.h"
#include "../../std/timer.h"

struct Camera {
    
    Transform transform;
    
    glm::vec3 up;
    glm::vec3 forward;
    glm::vec3 right;
    
    Script* script;
    
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
    
    void SetMouseCenter(int reset_x, int reset_y);
    
    void EnableMouseLook(void);
    void DisableMouseLook(void);
    
    void Update(void);
    void MouseLook(float delta, int reset_x, int reset_y);
    
    void EnableMovementInput(void);
    void DisableMovementInput(void);
    
    
    glm::mat4 CalculateView(void);
    glm::mat4 CalculatePerspectiveMatrix(void);
    glm::mat4 CalculateOrthiographicMatrix(Viewport viewport);
    
};

#endif
