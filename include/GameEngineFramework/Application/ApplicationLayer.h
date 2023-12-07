#ifndef APPLICATION_LAYER
#define APPLICATION_LAYER

#include <GameEngineFramework/Renderer/RenderSystem.h>
#include <GameEngineFramework/Input/InputSystem.h>
#include <GameEngineFramework/Physics/PhysicsSystem.h>
#include <GameEngineFramework/Serialization/Serialization.h>
#include <GameEngineFramework/Resources/ResourceManager.h>
#include <GameEngineFramework/Scripting/ScriptSystem.h>
#include <GameEngineFramework/Audio/AudioSystem.h>

#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <windows.h>


ENGINE_API void Start(void);

ENGINE_API void Run(void);

ENGINE_API void TickUpdate(void);

ENGINE_API void Shutdown(void);



class ENGINE_API ApplicationLayer {
    
public:
    
    HWND windowHandle;
    HDC deviceContext;
    
    int displayWidth;
    int displayHeight;
    
    int windowLeft;
    int windowTop;
    int windowRight;
    int windowBottom;
    
    WNDCLASSEX wClassEx;
    HINSTANCE hInstance;
    
    /// Game paused state.
    bool isPaused;
    /// Game loop running state. Setting this to false will exit the loop.
    bool isActive;
    
    /// Cursor icon handle.
    HICON__* hCursor;
    
    ApplicationLayer();
    
    /// Toggle freezing the application loop.
    void Pause(void);
    
    /// Create the window and return its handle.
    HWND CreateWindowHandle(std::string className, std::string windowName, HWND parentHandle, LRESULT CALLBACK(*windowProcedure)(HWND, UINT, WPARAM, LPARAM));
    
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
