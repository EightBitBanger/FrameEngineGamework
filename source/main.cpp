#include "main.h"

extern Timer        PhysicsTime;
extern Timer        Time;

extern PhysicsSystem        Physics;
extern RenderSystem         Renderer;
extern AudioSystem          Audio;
extern ScriptSystem         Scripting;
extern ResourceManager      Resources;
extern ApplicationLayer     Application;
extern EngineSystemManager  Engine;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    // Create the main render window
    HWND wHndl = Application.CreateWindowHandle("windowFrame", "Scene View");
    
    // Console window
    HWND cHnd = GetConsoleWindow();
    ShowWindow(cHnd, SW_SHOW);
    SetWindowPos(cHnd, NULL, WINDOW_CONSOLE_LEFT, WINDOW_CONSOLE_TOP, WINDOW_CONSOLE_WIDTH, WINDOW_CONSOLE_HEIGHT, SWP_SHOWWINDOW);
    
    Application.SetWindowCenterScale(0.85, 1.1);
    Viewport windowSz = Application.GetWindowArea();
    
    // Clear the event log output file
    Log.Clear();
    
    // Initiate and set the target context for rendering
    if (Renderer.SetRenderTarget(wHndl) != GLEW_OK) {
        DestroyWindow(wHndl);
        MessageBox(NULL, "Cannot locate the OpenGL library. Please update your graphics drivers...", "Error", MB_OK);
        return 0;
    }
    
    // Set the render window view port size
    Renderer.SetViewport(0, 0, windowSz.w, windowSz.h);
    
    // Initiate engine sub systems
    PhysicsTime.SetRefreshRate(PHYSICS_UPDATES_PER_SECOND);
    Time.SetRefreshRate(RENDER_FRAMES_PER_SECOND);
    
    Physics.Initiate();
    Renderer.Initiate();
    Audio.Initiate();
    Resources.Initiate();
    
    // Initiate the engine last
    Engine.Initiate();
    
    Framework::Start();
    
    PhysicsTime.Update();
    Time.Update();
    
#ifdef RUN_UNIT_TESTS
    ApplicationTest Test;
    Test.Initiate();
    Test.TestGameObject();
    
    if (Test.CheckFailed() > 0) {
        ShowWindow(wHndl, SW_HIDE);
        std::string freezeOnFail;
        std::cin >> freezeOnFail;
    } else {
        Test.Complete();
    }
    
#endif    
    
    MSG messages;
    
    while (Application.isActive) {
        
        while (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }
        
#ifdef ESCAPE_KEY_QUIT
        if (Input.CheckKeyPressed(VK_ESCAPE)) 
            Application.isActive = false;
#endif
        
        // Physics timer
        if (PhysicsTime.Update()) 
            Physics.world->update( 1.0 / PHYSICS_UPDATES_PER_SECOND );
        
        // Application and render timer
        if (Time.Update()) {
            
            Framework::Run();
            
            Scripting.Update();
            
            Engine.Update();
            
            Renderer.RenderFrame( Time.delta );
            
        }
        
        continue;
    }
    
    
    // Shutdown engine & sub systems
    Framework::Shutdown();
    
    Engine.Shutdown();
    
    Resources.DestroyAssets();
    
    Physics.common.destroyPhysicsWorld(Physics.world);
    
    Audio.Shutdown();
    
    Renderer.ReleaseRenderTarget();
    
    Application.DestroyWindowHandle();
    
    return messages.wParam;
}


