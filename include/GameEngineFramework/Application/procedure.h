#ifndef APPLICATION_WINDOW_PROCEDURE
#define APPLICATION_WINDOW_PROCEDURE

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/Application/Platform.h>


#ifdef PLATFORM_WINDOWS
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <windows.h>
#endif

#ifdef PLATFORM_WINDOWS
extern "C" ENGINE_API LRESULT CALLBACK WindowProc(HWND wHnd, UINT Messages, WPARAM wParam, LPARAM lParam);
#endif


#endif
