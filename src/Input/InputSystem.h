#ifndef INPUT_CAPTURE_SYSTEM
#define INPUT_CAPTURE_SYSTEM

#include <glm/glm.hpp>

#include "Keys.h"


class __declspec(dllexport) InputSystem {
    
public:
    
    InputSystem(void);
    
    int  LastKeyPressed;
    
    int mouseX;
    int mouseY;
    
    /// Set the mouse position.
    void SetMousePosition(unsigned int x, unsigned int y);
    
    /// Set a pressed key state on the key ID index.
    void SetKeyPressed(int keyid);
    /// Set a released key state on the key ID index.
    void SetKeyReleased(int keyid);
    
    /// Check if a key is currently pressed.
    bool CheckKeyPressed(int keyid);
    /// Check if a key is currently released.
    bool CheckKeyReleased(int keyid);
    /// Check if a key is currently being held down.
    bool CheckKeyCurrent(int keyid);
    
    /// Set the left mouse button to a pressed state.
    void SetMouseLeftPressed(bool State);
    /// Set the right mouse button to a pressed state.
    void SetMouseRightPressed(bool State);
    /// Set the left mouse button to a release state.
    void SetMouseLeftReleased(bool State);
    /// Set the right mouse button to a release state.
    void SetMouseRightReleased(bool State);
    
    /// Check if the left mouse button is pressed.
    bool CheckMouseLeftPressed(void);
    /// Check if the right mouse button is pressed.
    bool CheckMouseRightPressed(void);
    /// Check if the left mouse button is release.
    bool CheckMouseLeftReleased(void);
    /// Check if the right mouse button is release.
    bool CheckMouseRightReleased(void);
    
    /// Clear the state of the left mouse button.
    void ClearMouseLeft(void);
    /// Clear the state of the right mouse button.
    void ClearMouseRight(void);
    
    /// Clear the key state of the key index ID.
    void ClearKey(int keyid);
    /// Clear the pressed key state of the key index ID.
    void ClearKeyPressed(int keyid);
    /// Clear the released key state of the key index ID.
    void ClearKeyReleased(int keyid);
    /// Clear the current key state of the key index ID.
    void ClearKeyCurrent(int keyid);
    
    /// Clear all key states.
    void ClearKeys(void);
    /// Clear pressed key states.
    void ClearKeysPressed(void);
    /// Clear released key states.
    void ClearKeysReleased(void);
    /// Clear current key states.
    void ClearKeysCurrent(void);
    
private:
    
    bool mKeyPressed  [256];
    bool mKeyReleased [256];
    bool mKeyCurrent  [256];
    
    bool mMouseLeftPressed;
    bool mMouseRightPressed;
    bool mMouseLeftReleased;
    bool mMouseRightReleased;
    
};

#endif
