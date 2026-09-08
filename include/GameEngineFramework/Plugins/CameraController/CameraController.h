#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <GameEngineFramework/Engine/Engine.h>

enum class CameraType {
    Free,
    RTS
};

ENGINE_API GameObject* CreateCameraController(glm::vec3 position, CameraType type);

ENGINE_API void CameraControllerUpdate(void);

#endif
