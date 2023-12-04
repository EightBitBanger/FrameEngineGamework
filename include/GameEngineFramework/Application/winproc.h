#ifndef __APPLICATION_WINDOW_PROCEDURE__
#define __APPLICATION_WINDOW_PROCEDURE__

#include "../configuration.h"

#include "../Input/InputSystem.h"
#include "../Renderer/RenderSystem.h"

#include "applicationlayer.h"

extern "C" ENGINE_API LRESULT CALLBACK WindowProc(HWND wHnd, UINT Messages, WPARAM wParam, LPARAM lParam);

#endif
