#ifndef APPLICATION_LAYER
#define APPLICATION_LAYER

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../Renderer/RenderSystem.h"
#include "../Input/InputSystem.h"
#include "../Physics/PhysicsSystem.h"
#include "../Serialization/Serialization.h"
#include "../Resources/ResourceManager.h"
#include "../Scripting/ScriptSystem.h"
#include "../Audio/AudioSystem.h"

#include "winproc.h"


__declspec(dllexport) void Start(void);

__declspec(dllexport) void Run(void);

__declspec(dllexport) void TickUpdate(void);

__declspec(dllexport) void Shutdown(void);



class __declspec(dllexport) ApplicationLayer {
    
public:
    
    HWND windowHandle;
    HDC deviceContext;
    
    unsigned int displayWidth;
    unsigned int displayHeight;
    
    WNDCLASSEX wClassEx;
    HINSTANCE hInstance;
    
    bool isPaused;
    bool isActive;
    
    HICON__* hCursor;
    
    ApplicationLayer();
    
    /// Toggle freezing the application loop.
    void Pause(void);
    
    /// Create the window and return its handle.
    HWND CreateWindowHandle(std::string className, std::string windowName, HWND parentHandle = NULL);
    
    /// Destroy the window handle.
    void DestroyWindowHandle(void);
    
    /// Set the position of the window handle.
    void SetWindowPosition(Viewport windowSize);
    
    /// Set the position of the window handle to the screen center.
    void SetWindowCenter(void);
    
    /// Set and scale the position of the window handle to the screen center.
    void SetWindowCenterScale(float width, float height);
    
    Viewport GetWindowArea(void);
    
    /// Show the window on the screen.
    void ShowWindowHandle(void);
    
    /// Hide the window.
    void HideWindowHandle(void);
    
    /// Show the mouse cursor.
    void ShowMouseCursor(void);
    
    /// Hide the mouse cursor.
    void HideMouseCursor(void);
    
    
private:
    
    bool mIsWindowRunning;
    
};

#endif
