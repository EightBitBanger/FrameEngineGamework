#include <GameEngineFramework/Application/main.h>
#include <GameEngineFramework/Application/procedure.h>

#ifdef RUN_UNIT_TESTS
 #include "../../tests/framework.h"
#endif

int main(int argc, char* argv[]) {
    
#ifdef RUN_UNIT_TESTS
    TestFramework tests;
    tests.Initiate();
    
    tests.AddTest( &tests.TestEngineFunctionality );
    
    tests.AddTest( &tests.TestRenderSystem );
    tests.AddTest( &tests.TestScriptSystem );
    
    tests.AddTest( &tests.TestGameObject );
    tests.AddTest( &tests.TestComponentObject );
    
    tests.AddTest( &tests.TestPhysicsSystem );
    tests.AddTest( &tests.TestTransform );
    
    tests.AddTest( &tests.TestSerializerSystem );
    
    // Run all the tests
    tests.RunTestSuite();
    
    // Finalize
    if (!tests.hasTestFailed) {
        tests.Complete();
    } else {
        DestroyWindow(wHndl);
        std::string freeze;
        std::cin >> freeze;
    }
    
#endif
    
    Platform.windowHandle = Platform.CreateWindowHandle("windowFrame", "Render window");
    Log.Clear();
    
    Platform.SetRenderTarget();
    
    // Get display size
    glm::vec2 dim = Platform.GetDisplaySize();
    Renderer.displaySize.x = dim.x;
    Renderer.displaySize.y = dim.y;
    Renderer.displayCenter.x = Renderer.displaySize.x / 2;
    Renderer.displayCenter.y = Renderer.displaySize.y / 2;
    
    Log.WriteLn(); // For event log layout
    
    // Initiate engine sub systems
    Resources.Initiate();
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
    
    AI.UpdateSendSignal(); // Kick off AI updates
    
    while (Platform.isActive) {
        
#ifdef PLATFORM_LINUX
        
        SDL_Event event;
        
        while (SDL_PollEvent(&event)) {
            
            // Get event key parameter
            SDL_Keycode keycode = event.key.key;
            
            int key = static_cast<int>(keycode);
            
            int VirtualKey=-1;
            switch (keycode) {
                case SDLK_F1: VirtualKey = VK_F1; break;
                case SDLK_F2: VirtualKey = VK_F2; break;
                case SDLK_F3: VirtualKey = VK_F3; break;
                case SDLK_F4: VirtualKey = VK_F4; break;
                case SDLK_F5: VirtualKey = VK_F5; break;
                case SDLK_F6: VirtualKey = VK_F6; break;
                case SDLK_F7: VirtualKey = VK_F7; break;
                case SDLK_F8: VirtualKey = VK_F8; break;
                case SDLK_F9: VirtualKey = VK_F9; break;
                case SDLK_F10: VirtualKey = VK_F10; break;
                case SDLK_F11: VirtualKey = VK_F11; break;
                case SDLK_F12: VirtualKey = VK_F12; break;
                
                case SDLK_ESCAPE: VirtualKey = VK_ESCAPE; break;
                
                case SDLK_LCTRL: VirtualKey = VK_LCONTROL; break;
                case SDLK_RCTRL: VirtualKey = VK_RCONTROL; break;
                
                case SDLK_LSHIFT: VirtualKey = VK_LSHIFT; break;
                case SDLK_RSHIFT: VirtualKey = VK_RSHIFT; break;
                
            }
            
            switch (event.type) {
                
                case SDL_EVENT_QUIT: 
                    Platform.isActive = false; 
                    break;
                
                case SDL_EVENT_KEY_DOWN: 
                    if (VirtualKey != -1) 
                        Input.SetKeyPressed(VirtualKey); // VK virtual keys
                    else 
                        Input.lastKeyPressed = key; // Text characters
                    
                    break;
                
                case SDL_EVENT_KEY_UP: 
                    if (VirtualKey != -1) 
                        Input.SetKeyReleased(VirtualKey); // VK virtual keys
                    else
                        Input.lastKeyReleased = key; // Text characters
                    
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
                
                case SDL_EVENT_WINDOW_RESIZED: 
                    
                    int x, y, w, h;
                    SDL_GetWindowPosition((SDL_Window*)Platform.windowHandle, &x, &y);
                    SDL_GetWindowSize((SDL_Window*)Platform.windowHandle, &w, &h);
                    
                    // Set window view port
                    Renderer.viewport.x = x;
                    Renderer.viewport.y = y;
                    Renderer.viewport.w = w;
                    Renderer.viewport.h = h;
                    
                    Platform.windowLeft   = Renderer.viewport.x;
                    Platform.windowTop    = Renderer.viewport.y;
                    Platform.windowRight  = Renderer.viewport.w;
                    Platform.windowBottom = Renderer.viewport.h;
                    
                    Platform.windowArea.x = Platform.windowLeft;
                    Platform.windowArea.y = Platform.windowTop;
                    Platform.windowArea.w = Platform.windowRight;
                    Platform.windowArea.h = Platform.windowBottom;
                    
                    // Update scene cameras
                    for (unsigned int i=0; i < Renderer.GetRenderQueueSize(); i++) {
                        
                        if (Renderer[i]->camera == nullptr) 
                            continue;
                        
                        // Fixed view port
                        if (Renderer[i]->camera->isFixedAspect) 
                            continue;
                        
                        // Update view port
                        int windowWidth  = Renderer.viewport.w;
                        int windowHeight = Renderer.viewport.h;
                        
                        Renderer[i]->camera->viewport.x = 0;
                        Renderer[i]->camera->viewport.y = 0;
                        Renderer[i]->camera->viewport.w = windowWidth;
                        Renderer[i]->camera->viewport.h = windowHeight;
                        
                        // Update camera aspect
                        Renderer[i]->camera->aspect = Renderer.viewport.w / Renderer.viewport.h;
                        
                        if (Renderer[i]->camera->aspect < 1.3) 
                            Renderer[i]->camera->aspect = 1.3;
                        
                        continue;
                    }
                    
                    break;
                    
                }
            
        }
        
#endif
        
#ifdef PLATFORM_WINDOWS
        
        MSG messages;
        
        while (PeekMessage(&messages, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&messages);
            DispatchMessage(&messages);
        }
        
#endif
        
        // Reset mouse scroll wheel state
        Input.mouseWheelDelta = 0;
        
#ifdef PLATFORM_WINDOWS
        POINT cursorPos;
        GetCursorPos(&cursorPos);
        Input.mouseX = cursorPos.x;
        Input.mouseY = cursorPos.y;
#endif
        
#ifdef PLATFORM_LINUX
        float mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        
        Input.mouseX = (int)mouseX;
        Input.mouseY = (int)mouseY;
#endif
        
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
                Viewport area = Platform.GetWindowArea();
                Platform.windowLeft   = area.x;
                Platform.windowTop    = area.y;
                Platform.windowRight  = area.w - area.x;
                Platform.windowBottom = area.h - area.y;
                
                Renderer.SetViewport(0, 0, area.w, area.h);
                
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
            
            //int w, h;
            
            //SDL_GetWindowSize((SDL_Window*)Platform.windowHandle, &w, &h);
            
            //glViewport(0, 0, 1280, 720);
            glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
            //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            // Draw the current frame state
            Renderer.RenderFrame();
            
            // Turn over the frame buffer
            
#ifdef PLATFORM_LINUX
            SDL_GL_SwapWindow((SDL_Window*)Platform.windowHandle);
#endif
            
#ifdef PLATFORM_WINDOWS
            SwapBuffers( (HDC)Platform.deviceContext );
#endif
            
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


