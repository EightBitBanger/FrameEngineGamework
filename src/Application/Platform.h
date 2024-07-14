#ifndef APPLICATION_LAYER
#define APPLICATION_LAYER

#include <GameEngineFramework/Renderer/RenderSystem.h>
#include <GameEngineFramework/Input/InputSystem.h>
#include <GameEngineFramework/Physics/PhysicsSystem.h>
#include <GameEngineFramework/Serialization/Serialization.h>
#include <GameEngineFramework/Resources/ResourceManager.h>
#include <GameEngineFramework/Scripting/ScriptSystem.h>
#include <GameEngineFramework/Audio/AudioSystem.h>

#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Types/Types.h>

extern Logger Log;
extern IntType Int;


ENGINE_API void Start(void);

ENGINE_API void Run(void);

ENGINE_API void TickUpdate(void);

ENGINE_API void Shutdown(void);



class ENGINE_API PlatformLayer {
    
public:
    
    void* windowHandle;
    void* deviceContext;
    void* renderContext;
    
    int displayWidth;
    int displayHeight;
    
    int windowLeft;
    int windowTop;
    int windowRight;
    int windowBottom;
    
    /// Game paused state.
    bool isPaused;
    /// Game loop running state. Setting this to false will exit the loop.
    bool isActive;
    
    PlatformLayer();
    
    /// Toggle freezing the application loop.
    void Pause(void);
    
    /// Create the window and return its handle as a void pointer.
    void* CreateWindowHandle(std::string className, std::string windowName, void* parentHandle, void* hInstance);
    
    /// Destroy the window handle.
    void DestroyWindowHandle(void);
    
    /// Set the position of the window handle.
    void SetWindowPosition(Viewport windowSize);
    
    /// Set the position of the window handle to the screen center.
    void SetWindowCenter(void);
    
    /// Set and scale the position of the window handle to the screen center.
    void SetWindowCenterScale(float width, float height);
    
    /// Get the area of the window in pixels.
    Viewport GetWindowArea(void);
    
    /// Show the window on the screen.
    void ShowWindowHandle(void);
    /// Hide the window.
    void HideWindowHandle(void);
    
    /// Show the mouse cursor.
    void ShowMouseCursor(void);
    /// Hide the mouse cursor.
    void HideMouseCursor(void);
    
    
    /// Set the target render context.
    GLenum SetRenderTarget(void);
    
    
private:
    
    bool mIsWindowRunning;
    
};

#endif
