//
// Input capture and processing system
#ifndef INPUT_SYSTEM_
#define INPUT_SYSTEM_

#include "Keys.h"

void      CursorSetPosition(int xx, int yy) {SetCursorPos( xx, yy );}

glm::vec2 CursorGetPosition(void) {
    POINT Pos;
    glm::vec2 MousePos;
    
    GetCursorPos(&Pos);
    
    MousePos.x = Pos.x;
    MousePos.y = Pos.y;
    
    return MousePos;
}

class InputSystem{
    private:
    
    bool KeyPressed  [256];
    bool KeyReleased [256];
    bool KeyCurrent  [256];
    
    bool MouseLeftPressed;
    bool MouseRightPressed;
    bool MouseLeftReleased;
    bool MouseRightReleased;
    
    public:
    
    int  LastKeyPressed;
    
    InputSystem(void)    {
        
        for (int i=0; i <= 255; i++) {KeyPressed[i] = 0; KeyReleased[i] = 0; KeyCurrent[i] = 0;}
        
        LastKeyPressed = -1;
        
        MouseLeftPressed = false;
        MouseRightPressed = false;
        MouseLeftReleased = false;
        MouseRightReleased = false;
        
    }
    
    // Set key state
    void  SetKeyPressed(int keyid)  {KeyPressed[keyid] = true;  KeyReleased[keyid] = false; KeyCurrent[keyid] = true;}
    void  SetKeyReleased(int keyid) {KeyPressed[keyid] = false; KeyReleased[keyid] = true;  KeyCurrent[keyid] = false;}
    
    // Check key state
    bool  CheckKeyPressed(int keyid)  {int currentKey = KeyPressed[keyid];  KeyPressed[keyid]  = false; return currentKey;}
    bool  CheckKeyReleased(int keyid) {int currentKey = KeyReleased[keyid]; KeyReleased[keyid] = false; return currentKey;}
    bool  CheckKeyCurrent(int keyid)  {return KeyCurrent [keyid];}
    
    // Set mouse state
    void SetMouseLeftPressed    (bool State)  {MouseLeftPressed = State;}
    void SetMouseRightPressed   (bool State)  {MouseRightPressed = State;}
    void SetMouseLeftReleased   (bool State)  {MouseLeftReleased = State;}
    void SetMouseRightReleased  (bool State)  {MouseRightReleased = State;}
    
    // Check mouse state
    bool CheckMouseLeftPressed    (void)  {return MouseLeftPressed;}
    bool CheckMouseRightPressed   (void)  {return MouseRightPressed;}
    bool CheckMouseLeftReleased   (void)  {return MouseLeftReleased;}
    bool CheckMouseRightReleased  (void)  {return MouseRightReleased;}
    
    // Clear mouse state
    void ClearMouseLeft         (void)  {MouseLeftPressed = false; MouseLeftReleased = false;}
    void ClearMouseRight        (void)  {MouseRightPressed = false; MouseRightReleased = false;}
    
    // Clear key state
    void  ClearKey(int keyid)         {KeyPressed[keyid] = false; KeyReleased[keyid] = false; KeyCurrent[keyid] = false;}
    void  ClearKeyPressed(int keyid)  {KeyPressed[keyid] = false;}
    void  ClearKeyReleased(int keyid) {KeyReleased[keyid] = false;}
    void  ClearKeyCurrent(int keyid)  {KeyCurrent[keyid] = false;}
    
    // Clear all key states
    void  ClearKeys(void)         {for (int i=0; i <= 255; i++) {KeyPressed[i] = 0; KeyReleased[i] = 0; KeyCurrent[i] = 0;}}
    void  ClearKeysPressed(void)  {for (int i=0; i <= 255; i++) {KeyPressed[i] = 0;}}
    void  ClearKeysReleased(void) {for (int i=0; i <= 255; i++) {KeyReleased[i] = 0;}}
    void  ClearKeysCurrent(void)  {for (int i=0; i <= 255; i++) {KeyCurrent[i] = 0;}}
    
};

#endif

