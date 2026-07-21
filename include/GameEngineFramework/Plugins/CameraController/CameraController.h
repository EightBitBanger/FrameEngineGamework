#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API GameObject* CreateCameraController(glm::vec3 position);

ENGINE_API void CameraControllerUpdate(void);

#endif
