//#define _WIN32_WINNT  0x500
#include <SDKDDKVer.h>
#include <windows.h>

#include <glm/glm.hpp>

#include "InputSystem.h"
#include "Keys.h"

void CursorSetPosition(int xx, int yy) {
    SetCursorPos( xx, yy );
    return;
}

glm::vec2 CursorGetPosition(void) {
    POINT Pos;
    glm::vec2 MousePos;
    
    GetCursorPos(&Pos);
    
    MousePos.x = Pos.x;
    MousePos.y = Pos.y;
    
    return MousePos;
}


extern InputSystem::InputSystem(void)    {
    
    for (int i=0; i <= 255; i++) {KeyPressed[i] = 0; KeyReleased[i] = 0; KeyCurrent[i] = 0;}
    
    LastKeyPressed = -1;
    
    MouseLeftPressed = false;
    MouseRightPressed = false;
    MouseLeftReleased = false;
    MouseRightReleased = false;
    
}

// Set key state
void  InputSystem::SetKeyPressed(int keyid)  {KeyPressed[keyid] = true;  KeyReleased[keyid] = false; KeyCurrent[keyid] = true;}
void  InputSystem::SetKeyReleased(int keyid) {KeyPressed[keyid] = false; KeyReleased[keyid] = true;  KeyCurrent[keyid] = false;}

// Check key state
bool  InputSystem::CheckKeyPressed(int keyid)  {int currentKey = KeyPressed[keyid];  KeyPressed[keyid]  = false; return currentKey;}
bool  InputSystem::CheckKeyReleased(int keyid) {int currentKey = KeyReleased[keyid]; KeyReleased[keyid] = false; return currentKey;}
bool  InputSystem::CheckKeyCurrent(int keyid)  {return KeyCurrent [keyid];}

// Set mouse state
void InputSystem::SetMouseLeftPressed(bool State)   {MouseLeftPressed = State;}
void InputSystem::SetMouseRightPressed(bool State)  {MouseRightPressed = State;}
void InputSystem::SetMouseLeftReleased(bool State)  {MouseLeftReleased = State;}
void InputSystem::SetMouseRightReleased(bool State) {MouseRightReleased = State;}

// Check mouse state
bool InputSystem::CheckMouseLeftPressed(void)   {return MouseLeftPressed;}
bool InputSystem::CheckMouseRightPressed(void)  {return MouseRightPressed;}
bool InputSystem::CheckMouseLeftReleased(void)  {return MouseLeftReleased;}
bool InputSystem::CheckMouseRightReleased(void) {return MouseRightReleased;}

// Clear mouse state
void InputSystem::ClearMouseLeft(void)  {MouseLeftPressed = false; MouseLeftReleased = false;}
void InputSystem::ClearMouseRight(void) {MouseRightPressed = false; MouseRightReleased = false;}

// Clear key state
void  InputSystem::ClearKey(int keyid)         {KeyPressed[keyid] = false; KeyReleased[keyid] = false; KeyCurrent[keyid] = false;}
void  InputSystem::ClearKeyPressed(int keyid)  {KeyPressed[keyid] = false;}
void  InputSystem::ClearKeyReleased(int keyid) {KeyReleased[keyid] = false;}
void  InputSystem::ClearKeyCurrent(int keyid)  {KeyCurrent[keyid] = false;}

// Clear all key states
void  InputSystem::ClearKeys(void)         {for (int i=0; i <= 255; i++) {KeyPressed[i] = 0; KeyReleased[i] = 0; KeyCurrent[i] = 0;}}
void  InputSystem::ClearKeysPressed(void)  {for (int i=0; i <= 255; i++) {KeyPressed[i] = 0;}}
void  InputSystem::ClearKeysReleased(void) {for (int i=0; i <= 255; i++) {KeyReleased[i] = 0;}}
void  InputSystem::ClearKeysCurrent(void)  {for (int i=0; i <= 255; i++) {KeyCurrent[i] = 0;}}

