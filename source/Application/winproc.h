#ifndef WINDOW_PROCEDURE
#define WINDOW_PROCEDURE

#include "../configuration.h"

#include "../Input/InputSystem.h"
#include "../Renderer/RenderSystem.h"

#include "applicationlayer.h"

LRESULT CALLBACK WindowProc(HWND wHnd, UINT Messages, WPARAM wParam, LPARAM lParam);

#endif
