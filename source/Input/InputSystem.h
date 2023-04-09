//
// Input capture and processing system
#ifndef INPUT_SYSTEM_
#define INPUT_SYSTEM_

#include "Keys.h"

void CursorSetPosition(int xx, int yy);
glm::vec2 CursorGetPosition(void);


class InputSystem{
    
    bool KeyPressed  [256];
    bool KeyReleased [256];
    bool KeyCurrent  [256];
    
    bool MouseLeftPressed;
    bool MouseRightPressed;
    bool MouseLeftReleased;
    bool MouseRightReleased;
    
public:
    
    int  LastKeyPressed;
    
    InputSystem(void);
    
    // Set key state
    void SetKeyPressed(int keyid);
    void SetKeyReleased(int keyid);
    
    // Check key state
    bool CheckKeyPressed(int keyid);
    bool CheckKeyReleased(int keyid);
    bool CheckKeyCurrent(int keyid);
    
    // Set mouse state
    void SetMouseLeftPressed(bool State);
    void SetMouseRightPressed(bool State);
    void SetMouseLeftReleased(bool State);
    void SetMouseRightReleased(bool State);
    
    // Check mouse state
    bool CheckMouseLeftPressed(void);
    bool CheckMouseRightPressed(void);
    bool CheckMouseLeftReleased(void);
    bool CheckMouseRightReleased(void);
    
    // Clear mouse state
    void ClearMouseLeft(void);
    void ClearMouseRight(void);
    
    // Clear key state
    void ClearKey(int keyid);
    void ClearKeyPressed(int keyid);
    void ClearKeyReleased(int keyid);
    void ClearKeyCurrent(int keyid);
    
    // Clear all key states
    void ClearKeys(void);
    void ClearKeysPressed(void);
    void ClearKeysReleased(void);
    void ClearKeysCurrent(void);
    
};

#endif

