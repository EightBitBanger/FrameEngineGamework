#include <GameEngineFramework/Application/Platform.h>
#include <GameEngineFramework/Application/winproc.h>
#include <GameEngineFramework/Engine/types/nulltype.h>

#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <windows.h>
#include <shellapi.h>

#define IDI_ICON  101


PlatformLayer::PlatformLayer() : 
    
    windowHandle(NULL),
    deviceContext(NULL),
    renderContext(NULL),
    
    displayWidth(1024),
    displayHeight(800),
    
    windowLeft(0),
    windowTop(0),
    windowRight(0),
    windowBottom(0),
    
    isPaused(false),
    isActive(true),
    
    mIsWindowRunning(false),
    EventCallbackLoseFocus(nullfunc)
{
}

void PlatformLayer::Pause(void) {
    isPaused = !isPaused;
    return;
}

void* PlatformLayer::CreateWindowHandle(std::string className, std::string windowName, void* parentHandle, void* hInstance) {
#ifdef PLATFORM_WINDOWS
    isPaused = false;
    isActive = true;
    
    HICON__* hCursor = LoadCursor(NULL, IDC_ARROW);
    
    HINSTANCE instanceHandle = (HINSTANCE)hInstance;
    
    WNDCLASSEX wClassEx;
    wClassEx.lpszClassName   = className.c_str();
    wClassEx.cbSize          = sizeof(WNDCLASSEX);
    wClassEx.style           = CS_OWNDC;
    wClassEx.lpfnWndProc     = (WNDPROC)WindowProc;
    wClassEx.cbClsExtra      = 0;
    wClassEx.cbWndExtra      = 0;
    wClassEx.hInstance       = instanceHandle;
    wClassEx.lpszMenuName    = NULL;
    wClassEx.hCursor         = hCursor;
    wClassEx.hIcon           = LoadIcon(instanceHandle, MAKEINTRESOURCE(IDI_ICON));
    wClassEx.hIconSm         = LoadIcon(instanceHandle, IDI_APPLICATION);
    wClassEx.hbrBackground   = (HBRUSH)GetStockObject(BLACK_BRUSH);
    
    assert( RegisterClassEx(&wClassEx) );
    
    windowHandle = CreateWindowEx(0, className.c_str(), windowName.c_str(), 
                                  WS_OVERLAPPEDWINDOW, 
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                                  (HWND)parentHandle, NULL, 
                                  instanceHandle, NULL);
    
    assert(windowHandle != NULL);
    
    ShowWindow( (HWND)windowHandle, true );
    SetCursor(hCursor);
    
#ifdef APPLICATION_CURSOR_HIDE_ON_STARTUP
    while (ShowCursor(false) >= 0);
#endif
    
    deviceContext = GetDC( (HWND)windowHandle );
    
    displayWidth  = GetDeviceCaps( (HDC)deviceContext, HORZRES );
    displayHeight = GetDeviceCaps( (HDC)deviceContext, VERTRES );
    
    
    RECT windowDim;
    GetWindowRect((HWND)windowHandle, &windowDim);
    
    RECT clientRect;
    GetClientRect((HWND)windowHandle, &clientRect);
    
    // Calculate the window's width and height
    float windowWidth = windowDim.right - windowDim.left;
    float windowHeight = windowDim.bottom - windowDim.top;
    
    // Calculate the client area width and height
    float clientWidth = clientRect.right - clientRect.left;
    float clientHeight = clientRect.bottom - clientRect.top;
    
    // Calculate the margins (borders)
    float horizontalMargin = (windowWidth - clientWidth) / 2.0f;
    float verticalMargin = (windowHeight - clientHeight) / 2.0f;
    
    // The starting position of the client area relative to the window
    clientArea.x = windowDim.left + horizontalMargin + 2.0f;
    clientArea.y = windowDim.top + verticalMargin - 2.0f;
    clientArea.w = clientRect.right - clientRect.left;
    clientArea.h = windowDim.bottom - windowDim.top;
    
    mIsWindowRunning = true;
    return (void*)windowHandle;
#endif
}

void PlatformLayer::DestroyWindowHandle(void) {
#ifdef PLATFORM_WINDOWS
    DestroyWindow((HWND)windowHandle);
    windowHandle = NULL;
    
    mIsWindowRunning = false;
#endif
    return;
}

void PlatformLayer::SetWindowCenter(void) {
#ifdef PLATFORM_WINDOWS
    displayWidth  = GetDeviceCaps( (HDC)deviceContext, HORZRES );
    displayHeight = GetDeviceCaps( (HDC)deviceContext, VERTRES );
    
    RECT windowDim;
    GetWindowRect((HWND)windowHandle, &windowDim);
    
    Viewport WindowSz;
    WindowSz.w = windowDim.right  - windowDim.left;
    WindowSz.h = windowDim.bottom - windowDim.top;
    
    WindowSz.x = (displayWidth  / 2) - (WindowSz.w / 2);
    WindowSz.y = (displayHeight / 2) - (WindowSz.h / 2);
    
    SetWindowPos((HWND)windowHandle, NULL, WindowSz.x, WindowSz.y, WindowSz.w, WindowSz.h, SWP_SHOWWINDOW);
    
    
    RECT clientRect;
    GetClientRect((HWND)windowHandle, &clientRect);
    
    // Calculate the window's width and height
    float windowWidth = windowDim.right - windowDim.left;
    float windowHeight = windowDim.bottom - windowDim.top;
    
    // Calculate the client area width and height
    float clientWidth = clientRect.right - clientRect.left;
    float clientHeight = clientRect.bottom - clientRect.top;
    
    // Calculate the margins (borders)
    float horizontalMargin = (windowWidth - clientWidth) / 2.0f;
    float verticalMargin = (windowHeight - clientHeight) / 2.0f;
    
    // The starting position of the client area relative to the window
    clientArea.x = windowDim.left + horizontalMargin + 2.0f;
    clientArea.y = windowDim.top + verticalMargin - 2.0f;
    clientArea.w = clientRect.right - clientRect.left;
    clientArea.h = windowDim.bottom - windowDim.top;
#endif
    return;
}

void PlatformLayer::SetWindowCenterScale(float width, float height) {
#ifdef PLATFORM_WINDOWS
    Viewport newWindowSz;
    newWindowSz.y = 0;
    newWindowSz.x = 0;
    newWindowSz.w = displayWidth  * width;
    newWindowSz.h = displayHeight * height;
    
    windowArea.w = newWindowSz.w;
    windowArea.h = newWindowSz.h;
    
    SetWindowPosition(newWindowSz);
    SetWindowCenter();
    
    RECT windowDim;
    GetWindowRect((HWND)windowHandle, &windowDim);
    
    RECT clientRect;
    GetClientRect((HWND)windowHandle, &clientRect);
    
    // Calculate the window's width and height
    float windowWidth = windowDim.right - windowDim.left;
    float windowHeight = windowDim.bottom - windowDim.top;
    
    // Calculate the client area width and height
    float clientWidth = clientRect.right - clientRect.left;
    float clientHeight = clientRect.bottom - clientRect.top;
    
    // Calculate the margins (borders)
    float horizontalMargin = (windowWidth - clientWidth) / 2.0f;
    float verticalMargin = (windowHeight - clientHeight) / 2.0f;
    
    // The starting position of the client area relative to the window
    clientArea.x = windowDim.left + horizontalMargin + 2.0f;
    clientArea.y = windowDim.top + verticalMargin - 2.0f;
    clientArea.w = clientRect.right - clientRect.left;
    clientArea.h = windowDim.bottom - windowDim.top;
#endif
    return;
}

void PlatformLayer::SetWindowPosition(Viewport windowSize) {
#ifdef PLATFORM_WINDOWS
    SetWindowPos((HWND)windowHandle, NULL, windowSize.x, windowSize.y, windowSize.w, windowSize.h, SWP_SHOWWINDOW);
    
    windowArea.w = windowSize.w;
    windowArea.h = windowSize.h;
#endif
    return;
}

Viewport PlatformLayer::GetWindowArea(void) {
#ifdef PLATFORM_WINDOWS
    RECT windowSz;
    GetWindowRect((HWND)windowHandle, &windowSz);
    
    Viewport area;
    area.x = windowSz.left;
    area.y = windowSz.top;
    area.w = (windowSz.right  - windowSz.left);
    area.h = (windowSz.bottom - windowSz.top);
    return area;
#endif
}

void PlatformLayer::WindowEnableFullscreen(void) {
#ifdef PLATFORM_WINDOWS
    SetWindowLongPtr((HWND)windowHandle, GWL_STYLE, WS_POPUP);
    SetWindowPos((HWND)windowHandle, HWND_TOPMOST, 0, 0, displayWidth, displayHeight, SWP_FRAMECHANGED);
    ShowWindow((HWND)windowHandle, SW_MAXIMIZE);
#endif
    return;
}

void PlatformLayer::WindowDisableFullscreen(void) {
#ifdef PLATFORM_WINDOWS
    SetWindowLongPtr((HWND)windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
    SetWindowPos((HWND)windowHandle, HWND_NOTOPMOST, windowArea.x, windowArea.y, windowArea.w, windowArea.h, SWP_FRAMECHANGED);
    ShowWindow((HWND)windowHandle, SW_RESTORE);
#endif
    return;
}

void PlatformLayer::HideWindowHandle(void) {
#ifdef PLATFORM_WINDOWS
    ShowWindow((HWND)windowHandle, false);
#endif
    return;
}

void PlatformLayer::ShowWindowHandle(void) {
#ifdef PLATFORM_WINDOWS
    ShowWindow((HWND)windowHandle, true);
#endif
    return;
}

void PlatformLayer::ShowMouseCursor(void) {
#ifdef PLATFORM_WINDOWS
    while (ShowCursor(true) < 0);
#endif
}

void PlatformLayer::HideMouseCursor(void) {
#ifdef PLATFORM_WINDOWS
    while (ShowCursor(false) >= 0);
#endif
}

int PlatformLayer::GetTaskbarHeight(void) {
#ifdef PLATFORM_WINDOWS
    APPBARDATA abd;
    abd.cbSize = sizeof(APPBARDATA);
    
    SHAppBarMessage(ABM_GETTASKBARPOS, &abd);
    
    return abd.rc.bottom - abd.rc.top;
#endif
}

int PlatformLayer::GetTitlebarHeight(void) {
#ifdef PLATFORM_WINDOWS
    int titleBarHeight = GetSystemMetrics(SM_CYCAPTION);
#endif
    return titleBarHeight;
}

void PlatformLayer::SetClipboardText(std::string text) {
    
#ifdef PLATFORM_WINDOWS
    
    if (OpenClipboard(nullptr)) {
        
        EmptyClipboard();
        
        HGLOBAL block = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
        if (block) {
            char* globalLock = static_cast<char*>(GlobalLock(block));
            memcpy(globalLock, text.c_str(), text.size() + 1); // +1 for null terminator
            GlobalUnlock(block);
            
            // Set the clipboard data
            SetClipboardData(CF_TEXT, block);
        }
        
        // Close the clipboard
        CloseClipboard();
    }
    
#endif
    
    return;
}

std::string PlatformLayer::GetClipboardText(void) {
    
#ifdef PLATFORM_WINDOWS
    
    std::string text;
    
    if (OpenClipboard(nullptr)) {
        
        HGLOBAL block = GetClipboardData(CF_TEXT);
        
        if (block) {
            
            char* globalLock = static_cast<char*>(GlobalLock(block));
            
            if (globalLock) {
                text = std::string(globalLock);
                GlobalUnlock(block);
            }
        }
        
        CloseClipboard();
    }
    
#endif
    
    return text;
}

GLenum PlatformLayer::SetRenderTarget(void) {
    
#ifdef PLATFORM_WINDOWS
    
    int iFormat;
    std::string gcVendor, gcRenderer, gcExtensions, gcVersion, Line;
    
    // Set the window handle and get the device context
    windowHandle  = (HWND)windowHandle;
    deviceContext = GetDC((HWND)windowHandle);
    
    // Pixel format descriptor
    PIXELFORMATDESCRIPTOR pfd;
    
    ZeroMemory (&pfd, sizeof (pfd));
    
    pfd.nSize       = sizeof (pfd);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = 32;
    pfd.cDepthBits  = 32;
    pfd.iLayerType  = PFD_MAIN_PLANE;
    
    // Setup pixel format
    iFormat = ChoosePixelFormat((HDC)deviceContext, &pfd);
    SetPixelFormat((HDC)deviceContext, iFormat, &pfd);
    
    renderContext = wglCreateContext((HDC)deviceContext);
    
    wglMakeCurrent((HDC)deviceContext, (HGLRC)renderContext);
    
    // Enable VSYNC
    //typedef BOOL (APIENTRY * PFNWGLSWAPINTERVALEXTPROC)(int interval);
    //PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
    //wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    //wglSwapIntervalEXT(1);
    
#endif
    
    // Initiate glew after setting the render target
    GLenum passed = glewInit();
    
    //
    // Log hardware details
    
#ifdef  LOG_RENDER_DETAILS
    
    const char* gcVendorConst     = (const char*)glGetString(GL_VENDOR);
    const char* gcRendererConst   = (const char*)glGetString(GL_RENDERER);
    const char* gcExtensionsConst = (const char*)glGetString(GL_EXTENSIONS);
    const char* gcVersionConst    = (const char*)glGetString(GL_VERSION);
    
    gcVendor     = std::string(gcVendorConst);
    gcRenderer   = std::string(gcRendererConst);
    gcExtensions = std::string(gcExtensionsConst);
    gcVersion    = std::string(gcVersionConst);
    
    // Log details
    Log.Write("== Hardware details =="); Line = "" + gcRenderer;
    Log.Write(Line);
    Log.WriteLn();
    
    std::string DetailStringHead = "  - ";
    std::string DetailStringEqu  = " = ";
    
    Line = " Device"; Log.Write(Line);
    Line = DetailStringHead + "Name   " + DetailStringEqu + gcVendor;  Log.Write(Line);
    Line = DetailStringHead + "Version" + DetailStringEqu + gcVersion; Log.Write(Line);
    Log.WriteLn();
    
    Line = " Colors"; Log.Write(Line);
    Line = DetailStringHead + "Color" + DetailStringEqu + Int.ToString(pfd.cColorBits) + " bit"; Log.Write(Line);
    Line = DetailStringHead + "Depth" + DetailStringEqu + Int.ToString(pfd.cDepthBits) + " bit"; Log.Write(Line);
    Log.WriteLn();
    Log.WriteLn();
    
#endif
    
    return passed;
}
