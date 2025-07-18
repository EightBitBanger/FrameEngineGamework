#ifndef INPUT_CAPTURE_SYSTEM
#define INPUT_CAPTURE_SYSTEM

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/Input/keys.h>

#include <glm/glm.hpp>
#include <unordered_map>

class ENGINE_API InputSystem {
    
public:
    
    InputSystem(void);
    
    int lastKeyPressed;
    int lastKeyReleased;
    
    /// Mouse X position in screen coordinates.
    int mouseX;
    
    /// Mouse Y position in screen coordinates.
    int mouseY;
    
    /// Mouse scroll wheel delta.
    float mouseWheelDelta;
    
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
    /// Set the middle mouse button to a pressed state.
    void SetMouseMiddlePressed(bool state);
    
    /// Set the left mouse button to a release state.
    void SetMouseLeftReleased(bool State);
    /// Set the right mouse button to a release state.
    void SetMouseRightReleased(bool State);
    /// Set the middle mouse button to a released state.
    void SetMouseMiddleReleased(bool State);
    
    /// Check if the left mouse button is pressed.
    bool CheckMouseLeftPressed(void);
    /// Check if the right mouse button is pressed.
    bool CheckMouseRightPressed(void);
    /// Check if the middle mouse button is pressed.
    bool CheckMouseMiddlePressed(void);
    
    /// Check if the left mouse button is release.
    bool CheckMouseLeftReleased(void);
    /// Check if the right mouse button is release.
    bool CheckMouseRightReleased(void);
    /// Check if the middle mouse button is released.
    bool CheckMouseMiddleReleased(void);
    
    /// Clear the state of the left mouse button.
    void ClearMouseLeft(void);
    /// Clear the state of the right mouse button.
    void ClearMouseRight(void);
    /// Clear the state of the middle mouse button.
    void ClearMouseMiddle(void);
    
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
    
    /// Bind a key state to a function callback.
    void BindKeyPressToFunction(int keyid, void(*function)());
    void BindKeyCurrentToFunction(int keyid, void(*function)());
    void BindKeyReleaseToFunction(int keyid, void(*function)());
    
    void UpdateKeyBindings(void);
    
private:
    
    std::unordered_map<int, void(*)()> mKeyCallbackPress;
    std::unordered_map<int, void(*)()> mKeyCallbackCurrent;
    std::unordered_map<int, void(*)()> mKeyCallbackRelease;
    
    bool mKeyPressed  [256];
    bool mKeyReleased [256];
    bool mKeyCurrent  [256];
    
    bool mMouseLeftPressed;
    bool mMouseRightPressed;
    bool mMouseMiddlePressed;
    
    bool mMouseLeftReleased;
    bool mMouseRightReleased;
    bool mMouseMiddleReleased;
    
};

#endif
