#ifndef APPLICATION_LAYER
#define APPLICATION_LAYER

#include <windows.h>

#include "../Renderer/RenderSystem.h"
#include "../Input/InputSystem.h"
#include "../Physics/PhysicsSystem.h"
#include "../Resources/ResourceManager.h"
#include "../Scripting/ScriptSystem.h"

#include "winproc.h"


namespace Framework {
    
    void Start(void);
    
    void Run(void);
    
    void Shutdown(void);
    
};



class ApplicationLayer {
    
public:
    
    HWND windowHandle;
    HDC deviceContext;
    
    unsigned int displayWidth;
    unsigned int displayHeight;
    
    WNDCLASSEX wClassEx;
    HINSTANCE hInstance;
    
    bool isPaused;
    bool isActive;
    
    ApplicationLayer();
    
    /// Toggle freezing the application loop.
    void Pause(void);
    
    /// Create the window handle and return its handle.
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
    
    
private:
    
    bool mIsWindowRunning;
    
};

#endif
