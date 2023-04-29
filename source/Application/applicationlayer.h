#ifndef APPLICATION_LAYER_TYPE
#define APPLICATION_LAYER_TYPE

#include <windows.h>
#include <string>

#include "../Renderer/types/viewport.h"
#include "../winproc.h"

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
    
    /// Freeze the application loop.
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
    
    bool isWindowRunning;
    
};

#endif
