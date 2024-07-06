#include <GameEngineFramework/Application/main.h>

#ifdef RUN_UNIT_TESTS
 #include "../../tests/framework.h"
#endif

extern ProfilerTimer        Profiler;
extern Timer                PhysicsTime;
extern Timer                Time;
extern Logger               Log;

extern ActorSystem          AI;
extern InputSystem          Input;
extern AudioSystem          Audio;
extern EngineSystemManager  Engine;
extern PhysicsSystem        Physics;
extern NetworkSystem        Network;
extern RenderSystem         Renderer;
extern ScriptSystem         Scripting;
extern ResourceManager      Resources;
extern PlatformLayer        Platform;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    HWND wHndl = (HWND)Platform.CreateWindowHandle("windowFrame", "Render window", NULL, (void*)hInstance);
    
    HWND cHnd = GetConsoleWindow();
    
    ShowWindow(cHnd, SW_SHOW);
    SetWindowPos(cHnd, NULL, WINDOW_CONSOLE_LEFT, WINDOW_CONSOLE_TOP, WINDOW_CONSOLE_WIDTH, WINDOW_CONSOLE_HEIGHT, SWP_SHOWWINDOW);
    
    Log.Clear();
    
    Platform.SetRenderTarget();
    
    // Set the window handle and get the device context
    HDC hDC = GetDC( wHndl );
    
    // Get display size
    Renderer.displaySize.x = GetDeviceCaps(hDC, HORZRES);
    Renderer.displaySize.y = GetDeviceCaps(hDC, VERTRES);
    Renderer.displayCenter.x = Renderer.displaySize.x / 2;
    Renderer.displayCenter.y = Renderer.displaySize.y / 2;
    
    // Get window area
    RECT windowRect;
    GetWindowRect(wHndl, &windowRect);
    
    Renderer.SetViewport(0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
    
    Platform.windowLeft   = windowRect.left;
    Platform.windowTop    = windowRect.top;
    Platform.windowRight  = windowRect.right;
    Platform.windowBottom = windowRect.bottom;
    
    // Initiate window region
    Platform.SetWindowCenterScale(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    
    //
    // Initiate engine sub systems
    //
    
    Resources.Initiate();
    
    Log.WriteLn(); // For event log layout
    
    AI.Initiate();
    
    Renderer.Initiate();
    
    Audio.Initiate();
    
    Physics.Initiate();
    
    Network.Initiate();
    
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
    Time.SetRefreshRate(RENDER_FRAMES_PER_SECOND);
    Time.Update();
    
    // Physics timer
    PhysicsTime.SetRefreshRate(PHYSICS_UPDATES_PER_SECOND);
    PhysicsTime.Update();
    
    // Fixed rate update timer
    Timer fixedTimer;
    double fixedUpdateTimeout = 1000.0 / TICK_UPDATES_PER_SECOND;
    double fixedAccumulator=0;
    double fixedUpdateMax = fixedUpdateTimeout * 100;
    fixedTimer.Update();
    
    // Tick update timer
    Timer tickTimer;
    double tickUpdateTimeout = 1000.0f / 3;
    double tickAccumulator=0;
    double tickUpdateMax = tickUpdateTimeout * 100;
    tickTimer.Update();
    AI.UpdateSendSignal();
    
    
    //
    // Game loop
    //
    
    while (Platform.isActive) {
        
        MSG messages;
        while (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }
        
        // Reset mouse scroll wheel state
        Input.mouseWheelDelta = 0;
        
        // Update mouse position
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        Input.mouseX = cursorPos.x;
        Input.mouseY = cursorPos.y;
        
        //
        // Tick update timer (background update)
        //
        
        tickAccumulator = tickTimer.GetCurrentDelta();
        
        if (tickAccumulator > tickUpdateMax) 
            tickAccumulator = tickUpdateMax;
        
        if (tickAccumulator >= tickUpdateTimeout) {
            
            tickTimer.Update();
            
            tickAccumulator -= tickUpdateTimeout;
            
            AI.UpdateSendSignal();
            
            // Call extra updates on accumulated time
            for (int i=0; i < 2; i++) {
                
                TickUpdate();
                
                tickAccumulator -= tickUpdateTimeout;
                
                // Break if no more accumulated time
                if (tickAccumulator < tickUpdateTimeout) 
                    break;
                
                continue;
            }
        }
        
        //
        // Fixed rate update timer
        //
        
        fixedAccumulator = fixedTimer.GetCurrentDelta();
        
        if (fixedAccumulator > fixedUpdateMax) 
            fixedAccumulator = fixedUpdateMax;
        
        if (fixedAccumulator >= fixedUpdateTimeout) {
            
            fixedTimer.Update();
            
            // Call extra updates on accumulated time
            for (int i=0; i < 2; i++) {
                
                // --- Profiling ---
                if (Engine.CheckIsProfilerActive()) 
                    Profiler.Begin();
                
                
                Run();
                
                Scripting.Update();
                
                Engine.Update();
                
                Network.Update();
                
                // Update window area
                RECT windowRect;
                GetWindowRect(wHndl, &windowRect);
                
                Platform.windowLeft   =  windowRect.left;
                Platform.windowTop    =  windowRect.top;
                Platform.windowRight  = (windowRect.right  - windowRect.left);
                Platform.windowBottom = (windowRect.bottom - windowRect.top);
                
                
                // --- Profiling ---
                if (Engine.CheckIsProfilerActive()) 
                    Profiler.profileGameEngineUpdate = Profiler.Query();
                
                
                fixedAccumulator -= fixedUpdateTimeout;
                
                // Break if no more accumulated time
                if (fixedAccumulator < fixedUpdateTimeout) 
                    break;
                
                continue;
            }
        }
        
        
        
        //
        // Render timer
        //
        
        if (Time.Update()) {
            
            // --- Profiling ---
            if (Engine.CheckIsProfilerActive()) 
                Profiler.Begin();
            
            
            // Draw the current frame state
            Renderer.RenderFrame();
            
            // Turn over the frame buffer
            SwapBuffers( (HDC)Platform.deviceContext );
            
            
            // --- Profiling ---
            if (Engine.CheckIsProfilerActive()) 
                Profiler.profileRenderSystem = Profiler.Query();
            
        }
        
        
        //
        // Physics timer
        //
        
        if (PhysicsTime.Update()) {
            
            // --- Profiling ---
            if (Engine.CheckIsProfilerActive()) 
                Profiler.Begin();
            
            Physics.world->update( PHYSICS_UPDATES_PER_SECOND );
            
            // Generate the physics debug meshes
            Engine.UpdatePhysicsDebugRenderer();
            
            
            // --- Profiling ---
            if (Engine.CheckIsProfilerActive()) 
                Profiler.profilePhysicsSystem = Profiler.Query();
            
        }
        
        
#ifdef APPLICATION_ESCAPE_KEY_PAUSE
        if (Input.CheckKeyPressed(VK_ESCAPE)) {
            
            Platform.Pause();
            
            if (Platform.isPaused) {
                Input.ClearKeys();
                
                Platform.ShowMouseCursor();
                
            } else {
                SetCursorPos(Renderer.displayCenter.x, Renderer.displayCenter.y);
                
                Platform.HideMouseCursor();
                
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
    
    Network.Shutdown();
    
    Physics.Shutdown();
    
    Audio.Shutdown();
    
    Renderer.Shutdown();
    
    AI.Shutdown();
    
    Resources.DestroyAssets();
    
    Platform.DestroyWindowHandle();
    
    return 0;
}


