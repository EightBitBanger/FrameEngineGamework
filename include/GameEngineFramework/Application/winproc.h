#ifndef __APPLICATION_WINDOW_PROCEDURE__
#define __APPLICATION_WINDOW_PROCEDURE__

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/Application/ApplicationLayer.h>

extern "C" ENGINE_API LRESULT CALLBACK WindowProc(HWND wHnd, UINT Messages, WPARAM wParam, LPARAM lParam);

#endif
