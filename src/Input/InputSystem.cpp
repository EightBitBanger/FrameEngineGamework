#include <GameEngineFramework/Input/InputSystem.h>
#include <GameEngineFramework/Application/Platform.h>
#include <SDL3/SDL.h>

#ifdef PLATFORM_WINDOWS
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <windows.h>
#endif


extern InputSystem::InputSystem(void) : 
    
    lastKeyPressed(-1),
    lastKeyReleased(-1),
    
    mouseX(0),
    mouseY(0),
    
    mouseWheelDelta(0),
    
    mMouseLeftPressed(false),
    mMouseRightPressed(false),
    mMouseLeftReleased(false),
    mMouseRightReleased(false)
{
    for (int i=0; i <= 255; i++) {mKeyPressed[i] = 0; mKeyReleased[i] = 0; mKeyCurrent[i] = 0;}
}

void InputSystem::SetMousePosition(unsigned int x, unsigned int y) {
#ifdef PLATFORM_WINDOWS
    SetCursorPos( (int)x, (int)y );
#endif
    
#ifdef PLATFORM_LINUX
    extern PlatformLayer Platform;
    SDL_WarpMouseInWindow( (SDL_Window*)Platform.windowHandle, x, y);
#endif
    mouseX = x;
    mouseY = y;
}

// Set key state
void InputSystem::SetKeyPressed(int keyid)  {mKeyPressed[keyid] = true;  mKeyReleased[keyid] = false; mKeyCurrent[keyid] = true;}
void InputSystem::SetKeyReleased(int keyid) {mKeyPressed[keyid] = false; mKeyReleased[keyid] = true;  mKeyCurrent[keyid] = false;}

// Check key state
bool InputSystem::CheckKeyPressed(int keyid)  {int currentKey = mKeyPressed[keyid];  mKeyPressed[keyid]  = false; return currentKey;}
bool InputSystem::CheckKeyReleased(int keyid) {int currentKey = mKeyReleased[keyid]; mKeyReleased[keyid] = false; return currentKey;}
bool InputSystem::CheckKeyCurrent(int keyid)  {return mKeyCurrent [keyid];}

// Set mouse state
void InputSystem::SetMouseLeftPressed(bool State)   {mMouseLeftPressed = State;}
void InputSystem::SetMouseRightPressed(bool State)  {mMouseRightPressed = State;}
void InputSystem::SetMouseMiddlePressed(bool state) {mMouseMiddlePressed = state;}
void InputSystem::SetMouseLeftReleased(bool State)  {mMouseLeftReleased = State;}
void InputSystem::SetMouseRightReleased(bool State) {mMouseRightReleased = State;}
void InputSystem::SetMouseMiddleReleased(bool state) {mMouseMiddleReleased = state;}

// Check mouse state
bool InputSystem::CheckMouseLeftPressed(void)    {return mMouseLeftPressed;}
bool InputSystem::CheckMouseRightPressed(void)   {return mMouseRightPressed;}
bool InputSystem::CheckMouseMiddlePressed(void)  {return mMouseMiddlePressed;}
bool InputSystem::CheckMouseLeftReleased(void)   {return mMouseLeftReleased;}
bool InputSystem::CheckMouseRightReleased(void)  {return mMouseRightReleased;}
bool InputSystem::CheckMouseMiddleReleased(void) {return mMouseMiddleReleased;}

// Clear mouse state
void InputSystem::ClearMouseLeft(void)   {mMouseLeftPressed = false; mMouseLeftReleased = false;}
void InputSystem::ClearMouseRight(void)  {mMouseRightPressed = false; mMouseRightReleased = false;}
void InputSystem::ClearMouseMiddle(void) {mMouseMiddlePressed = false; mMouseMiddleReleased = false;}

// Clear key state
void InputSystem::ClearKey(int keyid)         {mKeyPressed[keyid] = false; mKeyReleased[keyid] = false; mKeyCurrent[keyid] = false;}
void InputSystem::ClearKeyPressed(int keyid)  {mKeyPressed[keyid] = false;}
void InputSystem::ClearKeyReleased(int keyid) {mKeyReleased[keyid] = false;}
void InputSystem::ClearKeyCurrent(int keyid)  {mKeyCurrent[keyid] = false;}

// Clear all key states
void InputSystem::ClearKeys(void)         {for (int i=0; i <= 255; i++) {mKeyPressed[i] = 0; mKeyReleased[i] = 0; mKeyCurrent[i] = 0;}}
void InputSystem::ClearKeysPressed(void)  {for (int i=0; i <= 255; i++) {mKeyPressed[i] = 0;}}
void InputSystem::ClearKeysReleased(void) {for (int i=0; i <= 255; i++) {mKeyReleased[i] = 0;}}
void InputSystem::ClearKeysCurrent(void)  {for (int i=0; i <= 255; i++) {mKeyCurrent[i] = 0;}}

void InputSystem::BindKeyPressToFunction(int keyid, void(*function)()) {
    mKeyCallbackPress[keyid] = function;
}

void InputSystem::BindKeyCurrentToFunction(int keyid, void(*function)()) {
    mKeyCallbackCurrent[keyid] = function;
}

void InputSystem::BindKeyReleaseToFunction(int keyid, void(*function)()) {
    mKeyCallbackRelease[keyid] = function;
}

void InputSystem::UpdateKeyBindings(void) {
    for (std::unordered_map<int, void(*)()>::iterator it = mKeyCallbackPress.begin(); it != mKeyCallbackPress.end(); ++it) {
        if (mKeyPressed[it->first]) {
            mKeyPressed[it->first] = 0;
            it->second();
        }
    }
    for (std::unordered_map<int, void(*)()>::iterator it = mKeyCallbackCurrent.begin(); it != mKeyCallbackCurrent.end(); ++it) {
        if (mKeyCurrent[it->first]) {
            it->second();
        }
    }
    for (std::unordered_map<int, void(*)()>::iterator it = mKeyCallbackRelease.begin(); it != mKeyCallbackRelease.end(); ++it) {
        if (mKeyReleased[it->first]) {
            mKeyReleased[it->first] = 0;
            it->second();
        }
    }
}

