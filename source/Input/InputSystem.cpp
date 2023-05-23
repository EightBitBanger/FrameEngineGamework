#include "InputSystem.h"

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

extern InputSystem::InputSystem(void) : 
    
    LastKeyPressed(-1),
    
    mMouseLeftPressed(false),
    mMouseRightPressed(false),
    mMouseLeftReleased(false),
    mMouseRightReleased(false)
{
    for (int i=0; i <= 255; i++) {mKeyPressed[i] = 0; mKeyReleased[i] = 0; mKeyCurrent[i] = 0;}
}

// Set key state
void  InputSystem::SetKeyPressed(int keyid)  {mKeyPressed[keyid] = true;  mKeyReleased[keyid] = false; mKeyCurrent[keyid] = true;}
void  InputSystem::SetKeyReleased(int keyid) {mKeyPressed[keyid] = false; mKeyReleased[keyid] = true;  mKeyCurrent[keyid] = false;}

// Check key state
bool  InputSystem::CheckKeyPressed(int keyid)  {int currentKey = mKeyPressed[keyid];  mKeyPressed[keyid]  = false; return currentKey;}
bool  InputSystem::CheckKeyReleased(int keyid) {int currentKey = mKeyReleased[keyid]; mKeyReleased[keyid] = false; return currentKey;}
bool  InputSystem::CheckKeyCurrent(int keyid)  {return mKeyCurrent [keyid];}

// Set mouse state
void InputSystem::SetMouseLeftPressed(bool State)   {mMouseLeftPressed = State;}
void InputSystem::SetMouseRightPressed(bool State)  {mMouseRightPressed = State;}
void InputSystem::SetMouseLeftReleased(bool State)  {mMouseLeftReleased = State;}
void InputSystem::SetMouseRightReleased(bool State) {mMouseRightReleased = State;}

// Check mouse state
bool InputSystem::CheckMouseLeftPressed(void)   {return mMouseLeftPressed;}
bool InputSystem::CheckMouseRightPressed(void)  {return mMouseRightPressed;}
bool InputSystem::CheckMouseLeftReleased(void)  {return mMouseLeftReleased;}
bool InputSystem::CheckMouseRightReleased(void) {return mMouseRightReleased;}

// Clear mouse state
void InputSystem::ClearMouseLeft(void)  {mMouseLeftPressed = false; mMouseLeftReleased = false;}
void InputSystem::ClearMouseRight(void) {mMouseRightPressed = false; mMouseRightReleased = false;}

// Clear key state
void  InputSystem::ClearKey(int keyid)         {mKeyPressed[keyid] = false; mKeyReleased[keyid] = false; mKeyCurrent[keyid] = false;}
void  InputSystem::ClearKeyPressed(int keyid)  {mKeyPressed[keyid] = false;}
void  InputSystem::ClearKeyReleased(int keyid) {mKeyReleased[keyid] = false;}
void  InputSystem::ClearKeyCurrent(int keyid)  {mKeyCurrent[keyid] = false;}

// Clear all key states
void  InputSystem::ClearKeys(void)         {for (int i=0; i <= 255; i++) {mKeyPressed[i] = 0; mKeyReleased[i] = 0; mKeyCurrent[i] = 0;}}
void  InputSystem::ClearKeysPressed(void)  {for (int i=0; i <= 255; i++) {mKeyPressed[i] = 0;}}
void  InputSystem::ClearKeysReleased(void) {for (int i=0; i <= 255; i++) {mKeyReleased[i] = 0;}}
void  InputSystem::ClearKeysCurrent(void)  {for (int i=0; i <= 255; i++) {mKeyCurrent[i] = 0;}}
