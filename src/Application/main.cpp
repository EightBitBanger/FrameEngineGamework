#include <GameEngineFramework/Application/main.h>

#ifdef RUN_UNIT_TESTS
 #include "../../tests/framework.h"
#endif

int main() {
    
#ifdef PLATFORM_LINUX
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); // Optional
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // or 4
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    // Create the main window
    SDL_Window* window = SDL_CreateWindow("Render window", 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Get the render context for openGL
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_GL_MakeCurrent(window, glContext);
    
    // Setup the window
    SDL_SetWindowResizable(window, 1);
    
    Log.Clear();
    
    Platform.SetRenderTarget();
    
    // Get display size
    Renderer.displaySize.x = 1024;
    Renderer.displaySize.y = 768;
    Renderer.displayCenter.x = Renderer.displaySize.x / 2;
    Renderer.displayCenter.y = Renderer.displaySize.y / 2;
    
    //
    // Initiate engine sub systems
    
    Resources.Initiate();
    
    Log.WriteLn(); // For event log layout
    
    AI.Initiate();
    
    Renderer.Initiate();
    
    Audio.Initiate();
    
    Physics.Initiate();
    
    Network.Initiate();
    
    Engine.Initiate();
    
    // Initiate 
    Log.WriteLn();
    Log.Write("<MainScene>");
    
    Start();
    
    
    // Initiate timers
    Time.SetRefreshRate(RENDER_FRAMES_PER_SECOND);
    Time.Update();
    
    PhysicsTime.SetRefreshRate(PHYSICS_UPDATES_PER_SECOND);
    PhysicsTime.Update();
    
    Timer fixedTimer;
    double fixedUpdateTimeout = 1000.0 / TICK_UPDATES_PER_SECOND;
    double fixedAccumulator=0;
    double fixedUpdateMax = fixedUpdateTimeout * 100;
    fixedTimer.Update();
    
    Timer tickTimer;
    double tickUpdateTimeout = 1000.0f / 1.3f;
    double tickAccumulator=0;
    double tickUpdateMax = tickUpdateTimeout * 100;
    tickTimer.Update();
    AI.UpdateSendSignal();
    
    // Main loop
    SDL_Event event;
    bool isRunning = true;
    while (isRunning) {
        
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                
                case SDL_EVENT_QUIT: 
                    isRunning = false;
                    break;
                
                case SDL_EVENT_KEY_DOWN:
                    Input.SetKeyPressed(static_cast<uint8_t>(event.key.key));
                    break;
                
                case SDL_EVENT_KEY_UP:
                    Input.SetKeyReleased(static_cast<uint8_t>(event.key.key));
                    break;
                
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    switch (event.button.button) {
                        case 1: Input.SetMouseLeftPressed(true); break;
                        case 2: Input.SetMouseMiddlePressed(true); break;
                        case 3: Input.SetMouseRightPressed(true); break;
                    }
                    break;
                
                case SDL_EVENT_MOUSE_BUTTON_UP:
                    switch (event.button.button) {
                        case 1: Input.SetMouseLeftReleased(true); break;
                        case 2: Input.SetMouseMiddleReleased(true); break;
                        case 3: Input.SetMouseRightReleased(true); break;
                    }
                    break;
                
                case SDL_EVENT_MOUSE_MOTION:
                    Input.mouseX = event.motion.x;
                    Input.mouseY = event.motion.y;
                    break;
                
            }
            
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
            
            AI.UpdateSendSignal();
            
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
                //RECT windowRect;
                //GetWindowRect(wHndl, &windowRect);
                
                //Platform.windowLeft   =  windowRect.left;
                //Platform.windowTop    =  windowRect.top;
                //Platform.windowRight  = (windowRect.right  - windowRect.left);
                //Platform.windowBottom = (windowRect.bottom - windowRect.top);
                
                Platform.windowLeft   =  0;
                Platform.windowTop    =  0;
                Platform.windowRight  = 800;
                Platform.windowBottom = 600;
                
                Renderer.SetViewport(0, 0, 800, 600);
                
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
            
            int w = 800;
            int h = 800;
            
            SDL_GetWindowSize(window, &w, &h);
            
            //glViewport(0, 0, 1280, 720);
            //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Draw the current frame state
            //Renderer.RenderFrame();
            
            // Turn over the frame buffer
            //SwapBuffers( (HDC)Platform.deviceContext );
            SDL_GL_SwapWindow(window);
            
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
        
        continue;
    }
    
    return 0;
    
#endif
    
#ifdef PLATFORM_WINDOWS
    
    HWND wHndl = (HWND)Platform.CreateWindowHandle("windowFrame", "Render window", NULL, NULL);
    
    HWND cHnd = GetConsoleWindow();
    
#ifndef  WINDOW_CONSOLE_HIDE_ON_STARTUP
    
    ShowWindow(cHnd, SW_SHOW);
    SetWindowPos(cHnd, NULL, WINDOW_CONSOLE_LEFT, WINDOW_CONSOLE_TOP, WINDOW_CONSOLE_WIDTH, WINDOW_CONSOLE_HEIGHT, SWP_SHOWWINDOW);
    
#else
    
    ShowWindow(cHnd, SW_HIDE);
    
#endif
    
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
    double tickUpdateTimeout = 1000.0f / 1.3f;
    double tickAccumulator=0;
    double tickUpdateMax = tickUpdateTimeout * 100;
    tickTimer.Update();
    AI.UpdateSendSignal();
    
    
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
            
            AI.UpdateSendSignal();
            
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
    
#endif
    
    return 0;
}


