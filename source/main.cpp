#include "main.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    // Create a main window
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
    Resources.Initiate();
    
    Framework::Start();
    
    PhysicsTime.Update();
    Time.Update();
    
    // Initial application state
    Application.isActive = true;
    Application.isPaused = false;
    
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
        
        // Physics update
        if (PhysicsTime.Update()) 
            Physics.world->update( PhysicsTime.delta );
        
        // Update application and renderer
        if (Time.Update()) {
            
            Framework::Run();
            
            Renderer.RenderFrame( Time.delta );
            
        }
        
        continue;
    }
    
    // Shutdown engine sub systems
    Framework::Shutdown();
    
    Resources.DestroyAssets();
    
    Physics.common.destroyPhysicsWorld(Physics.world);
    
    Renderer.ReleaseRenderTarget();
    
    Application.DestroyWindowHandle();
    
    return messages.wParam;
}




