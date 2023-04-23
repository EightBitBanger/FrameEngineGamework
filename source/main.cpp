#include "main.h"
#define IDI_ICON  101

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    
    WNDCLASSEX wClassEx;
    MSG  wMessages;
    
    wClassEx.lpszClassName   = "frame";
    wClassEx.cbSize          = sizeof(WNDCLASSEX);
    wClassEx.style           = CS_OWNDC;
    wClassEx.lpfnWndProc     = WindowProc;
    wClassEx.cbClsExtra      = 0;
    wClassEx.cbWndExtra      = 0;
    wClassEx.hInstance       = hInstance;
    wClassEx.lpszMenuName    = NULL;
    wClassEx.hCursor         = LoadCursor(NULL, IDC_ARROW);
    wClassEx.hIcon           = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wClassEx.hIconSm         = LoadIcon(hInstance, IDI_APPLICATION);
    wClassEx.hbrBackground   = (HBRUSH)GetStockObject(BLACK_BRUSH);
    
    if (!RegisterClassEx(&wClassEx)) {
        MessageBox(NULL, "Failed to register the window class.", "Error", MB_OK);
        return 0;
    }
    
    wHnd = CreateWindowEx(0, "frame", WINDOW_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    ShowWindow(wHnd, nCmdShow);
    
    // Console window
    HWND cHnd = GetConsoleWindow();
    ShowWindow(cHnd, SW_SHOW);
    SetWindowPos(cHnd, NULL, 5, 50, 700, 800, SWP_SHOWWINDOW);
    
    // Figure a good window size
    HDC hDC = GetDC(wHnd);
    int DisplayWidth  = GetDeviceCaps(hDC, HORZRES);
    int DisplayHeight = GetDeviceCaps(hDC, VERTRES);
    
    Viewport WindowSz;
    WindowSz.w = WINDOW_WIDTH;
    WindowSz.h = WINDOW_HEIGHT;
    
    WindowSz.x = (DisplayWidth  / 2) - (WindowSz.w / 2);
    WindowSz.y = (DisplayHeight / 2) - (WindowSz.h / 2);
    
    SetWindowPos(wHnd, NULL, WindowSz.x, WindowSz.y, WindowSz.w, WindowSz.h, SWP_SHOWWINDOW);
    
    Log.Clear();
    
    if (Renderer.SetRenderTarget(wHnd) != GLEW_OK) {
        DestroyWindow(wHnd);
        MessageBox(NULL, "Failed to link to the OpenGL library.", "Error", MB_OK);
        return 0;
    }
    
    RECT WindowRect;
    GetWindowRect(wHnd, &WindowRect);
    
    Renderer.SetViewport(0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top);
    
    
    PhysicsTime.SetRefreshRate(PHYSICS_UPDATES_PER_SECOND);
    Time.SetRefreshRate(RENDER_FRAMES_PER_SECOND);
    
    Physics.Initiate();
    Renderer.Initiate();
    Resources.Initiate();
    Engine.Initiate();
    
    Start();
    
    RenderTime.Update();
    PhysicsTime.Update();
    Time.Update();
    
    while (isActive) {
        
        while (PeekMessage(&wMessages, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&wMessages);
            DispatchMessage(&wMessages);
        }
        
        
        // ESCAPE key handling
        if (Input.CheckKeyPressed(VK_ESCAPE)) {
            isPaused = !isPaused;
            if (Renderer.cameraMain == nullptr) continue;
            
            if (isPaused) {
                Renderer.cameraMain->DisableMouseLook();
                
                Input.ClearKeys();
            } else {
                Renderer.cameraMain->EnableMouseLook();
                Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
                
                Time.Update();
                PhysicsTime.Update();
                RenderTime.Update();
            }
        }
        
        if (PhysicsTime.Update()) 
            Physics.world->update( PhysicsTime.delta );
        
        if (Time.Update()) {
            
            Run();
            
            Renderer.RenderFrame( Time.delta );
            
        }
        
        continue;
    }
    
    Shutdown();
    
    Resources.DestroyAssets();
    
    Physics.common.destroyPhysicsWorld(Physics.world);
    
    Renderer.ReleaseRenderTarget();
    DestroyWindow(wHnd);
    
    return wMessages.wParam;
}




