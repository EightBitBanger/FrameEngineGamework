#include "main.h"

extern Timer                PhysicsTime;
extern Timer                Time;
extern Logger               Log;
extern ActorSystem          AI;
extern InputSystem          Input;
extern AudioSystem          Audio;
extern EngineSystemManager  Engine;
extern PhysicsSystem        Physics;
extern RenderSystem         Renderer;
extern ScriptSystem         Scripting;
extern ResourceManager      Resources;
extern ApplicationLayer     Application;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    HWND wHndl = Application.CreateWindowHandle("windowFrame", "Render window", NULL, WindowProc);
    
    HWND cHnd = GetConsoleWindow();
    
    ShowWindow(cHnd, SW_SHOW);
    SetWindowPos(cHnd, NULL, WINDOW_CONSOLE_LEFT, WINDOW_CONSOLE_TOP, WINDOW_CONSOLE_WIDTH, WINDOW_CONSOLE_HEIGHT, SWP_SHOWWINDOW);
    
    Log.Clear();
    
    if (Renderer.SetRenderTarget(wHndl) != GLEW_OK) {
        Application.ShowMouseCursor();
        
        DestroyWindow(wHndl);
        ShowWindow(cHnd, SW_HIDE);
        
        MessageBox(NULL, "Cannot locate the OpenGL library. Please update your graphics drivers...", "Error", MB_OK);
        return 0;
    }
    
    // Initiate window size
    Application.SetWindowCenterScale(0.85, 1.1);
    Viewport windowSz = Application.GetWindowArea();
    Renderer.SetViewport(0, 0, windowSz.w, windowSz.h);
    
    // Initiate engine sub systems
    Resources.Initiate();
    
    Log.WriteLn(); // For event log layout
    
    AI.Initiate();
    
    Renderer.Initiate();
    
    Audio.Initiate();
    
    Physics.Initiate();
    
    Engine.Initiate();
    
#ifdef RUN_UNIT_TESTS
    TestFramework testFrameWork;
    testFrameWork.Initiate();
    
    testFrameWork.AddTest( &testFrameWork.TestEngineFunctionality );
    
    testFrameWork.AddTest( &testFrameWork.TestRenderSystem );
    testFrameWork.AddTest( &testFrameWork.TestScriptSystem );
    
    testFrameWork.AddTest( &testFrameWork.TestGameObject );
    testFrameWork.AddTest( &testFrameWork.TestComponentObject );
    
    testFrameWork.AddTest( &testFrameWork.TestPhysicsSystem );
    testFrameWork.AddTest( &testFrameWork.TestTransform );
    
    testFrameWork.AddTest( &testFrameWork.TestSerializerSystem );
    
    // Run all the tests
    testFrameWork.RunTestSuite();
    
    // Finalize
    if (!testFrameWork.hasTestFailed) {
        testFrameWork.Complete();
    } else {
        DestroyWindow(wHndl);
        std::string freeze;
        std::cin >> freeze;
    }
#endif
    
    Log.WriteLn();
    Log.Write("<MainScene>");
    
    Start();
    
    
    // Render timer
    float renderUpdateTimeout = 1000.0f / RENDER_FRAMES_PER_SECOND;
    float renderAccumulator=0;
    Time.SetRefreshRate(RENDER_FRAMES_PER_SECOND);
    Time.Update();
    
    // Physics timer
    float physicsUpdateTimeout = 1000.0f / PHYSICS_UPDATES_PER_SECOND;
    float physicsAccumulator=0;
    float physicsAlpha=0;
    PhysicsTime.SetRefreshRate(PHYSICS_UPDATES_PER_SECOND);
    PhysicsTime.Update();
    
    // Fixed rate update timer
    Timer fixedTimer;
    float fixedUpdateTimeout = 1000.0f / TICK_UPDATES_PER_SECOND;
    float fixedAccumulator=0;
    float fixedUpdateMax = fixedUpdateTimeout * 100;
    fixedTimer.Update();
    
    // Tick update timer
    Timer tickTimer;
    float tickUpdateTimeout = 1000.0f / 4;
    float tickAccumulator=0;
    float tickUpdateMax = tickUpdateTimeout * 100;
    tickTimer.Update();
    AI.UpdateSendSignal();
    
    
    //
    // Game loop
    //
    
    while (Application.isActive) {
        
        MSG messages;
        while (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }
        
        //
        // Fixed rate update timer
        //
        
        fixedAccumulator += fixedTimer.GetCurrentDelta();
        
        if (fixedAccumulator > fixedUpdateMax) 
            fixedAccumulator = fixedUpdateMax;
        
        fixedTimer.Update();
        
        if (fixedAccumulator >= fixedUpdateTimeout) {
            
            // Call extra updates on accumulated time
            for (int i=0; i < 2; i++) {
                
                Run();
                
                Scripting.Update();
                
                Engine.Update();
                
                fixedAccumulator -= fixedUpdateTimeout;
                
                // Break if no more accumulated time
                if (fixedAccumulator < fixedUpdateTimeout) 
                    break;
                
                continue;
            }
        }
        
        //
        // Tick update timer
        //
        
        tickAccumulator += tickTimer.GetCurrentDelta();
        if (tickAccumulator > tickUpdateMax) 
            tickAccumulator = tickUpdateMax;
            
        tickTimer.Update();
        
        if (tickAccumulator >= tickUpdateTimeout) {
            
            tickAccumulator -= tickUpdateTimeout;
            
            // Call extra updates on accumulated time
            for (int i=0; i < 2; i++) {
                TickUpdate();
                
                AI.UpdateSendSignal();
                
                tickAccumulator -= tickUpdateTimeout;
                
                // Break if no more accumulated time
                if (tickAccumulator < tickUpdateTimeout) 
                    break;
                
                continue;
            }
        }
        
        //
        // Physics timer
        //
        
        physicsAccumulator += PhysicsTime.GetCurrentDelta();
        PhysicsTime.Update();
        
        if (physicsAccumulator >= physicsUpdateTimeout) {
            
            physicsAccumulator -= physicsUpdateTimeout;
            
            Physics.world->update( physicsUpdateTimeout );
            
            // Interpolation factor
            //physicsAlpha = physicsAccumulator / physicsUpdateTimeout;
            
        }
        
        //
        // Render timer
        //
        
        renderAccumulator += Time.GetCurrentDelta();
        Time.Update();
        
        if (renderAccumulator >= renderUpdateTimeout) {
            
            renderAccumulator -= renderUpdateTimeout;
            
            // Reset mouse scroll wheel state
            Input.mouseWheelDelta = 0;
            
            // Update mouse position
            POINT cursorPos;
            GetCursorPos(&cursorPos);
            Input.mouseX = cursorPos.x;
            Input.mouseY = cursorPos.y;
            
            // Update window area
            RECT WindowRect;
            GetWindowRect(Application.windowHandle, &WindowRect);
            
            // Set window view port
            Renderer.viewport.x = WindowRect.left;
            Renderer.viewport.y = WindowRect.top;
            Renderer.viewport.w = WindowRect.right - WindowRect.left;
            Renderer.viewport.h = WindowRect.bottom - WindowRect.top;
            
            // Draw the current frame state
            Renderer.RenderFrame();
        }
        
#ifdef APPLICATION_ESCAPE_KEY_PAUSE
        if (Input.CheckKeyPressed(VK_ESCAPE)) {
            
            Application.Pause();
            
            if (Application.isPaused) {
                Input.ClearKeys();
                
                Application.ShowMouseCursor();
                
            } else {
                SetCursorPos(Renderer.displayCenter.x, Renderer.displayCenter.y);
                
                Application.HideMouseCursor();
                
                Time.Update();
                PhysicsTime.Update();
            }
            
        }
#endif
        
        continue;
    }
    
    
    Log.WriteLn();
    
    // Shutdown engine & sub systems
    Shutdown();
    
    Engine.Shutdown();
    
    Physics.Shutdown();
    
    Audio.Shutdown();
    
    Renderer.Shutdown();
    Renderer.ReleaseRenderTarget();
    
    AI.Shutdown();
    
    Resources.DestroyAssets();
    
    Application.DestroyWindowHandle();
    
    return 0;
}


