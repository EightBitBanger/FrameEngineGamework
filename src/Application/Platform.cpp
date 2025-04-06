#include <GameEngineFramework/Application/Platform.h>
#include <GameEngineFramework/Application/procedure.h>
#include <GameEngineFramework/Engine/types/nulltype.h>

#include <GameEngineFramework/configuration.h>

#include <SDL3/SDL.h>

#ifdef PLATFORM_WINDOWS
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x500
#endif

#define WIN32_LEAN_AND_MEAN

#include <sdkddkver.h>
#include <windows.h>

// Icon reference
#define IDI_ICON  101
#endif



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
    
    EventCallbackLoseFocus(nullfunc),
    
    mIsWindowRunning(false)
{
}

void PlatformLayer::Pause(void) {
    isPaused = !isPaused;
    return;
}

void* PlatformLayer::CreateWindowHandle(std::string className, std::string windowName) {
#ifdef PLATFORM_WINDOWS
    isPaused = false;
    isActive = true;
    
    HICON__* hCursor = LoadCursor(NULL, IDC_ARROW);
    
    WNDCLASSEX wClassEx;
    wClassEx.lpszClassName   = className.c_str();
    wClassEx.cbSize          = sizeof(WNDCLASSEX);
    wClassEx.style           = CS_OWNDC;
    wClassEx.lpfnWndProc     = (WNDPROC)WindowProc;
    wClassEx.cbClsExtra      = 0;
    wClassEx.cbWndExtra      = 0;
    wClassEx.hInstance       = NULL;
    wClassEx.lpszMenuName    = NULL;
    wClassEx.hCursor         = hCursor;
    wClassEx.hIcon           = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON));
    wClassEx.hIconSm         = LoadIcon(NULL, IDI_APPLICATION);
    wClassEx.hbrBackground   = (HBRUSH)GetStockObject(BLACK_BRUSH);
    
    assert( RegisterClassEx(&wClassEx) );
    
    windowHandle = CreateWindowEx(0, className.c_str(), windowName.c_str(), 
                                  WS_OVERLAPPEDWINDOW, 
                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                                  NULL, NULL, 
                                  NULL, NULL);
    
    assert(windowHandle != NULL);
    
    ShowWindow( (HWND)windowHandle, true );
    SetCursor(hCursor);
    
#ifdef APPLICATION_CURSOR_HIDE_ON_STARTUP
    while (ShowCursor(false) >= 0);
#endif
    
    deviceContext = GetDC( (HWND)windowHandle );
    
    glm::vec2 dim = GetDisplaySize();
    displayWidth  = dim.x;
    displayHeight = dim.y;
    
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
    windowArea.x = windowDim.left + horizontalMargin + 2.0f;
    windowArea.y = windowDim.top + verticalMargin - 2.0f;
    windowArea.w = clientRect.right - clientRect.left;
    windowArea.h = windowDim.bottom - windowDim.top;
    
    mIsWindowRunning = true;
    return (void*)windowHandle;
#endif
    
#ifdef PLATFORM_LINUX
    windowArea.x = 0;
    windowArea.y = 0;
    windowArea.w = 800;
    windowArea.h = 600;
    
    SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1");
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); // Optional
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); // or 4
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    // Create the main window
    windowHandle = SDL_CreateWindow("Render window", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
    assert(windowHandle != nullptr);
    
    SDL_SetWindowResizable((SDL_Window*)windowHandle, 1);
    SetWindowCenterScale(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    // Get windows area
    int x, y, w, h;
    SDL_GetWindowPosition((SDL_Window*)windowHandle, &x, &y);
    SDL_GetWindowSize((SDL_Window*)windowHandle, &w, &h);
    
    // Set window view port
    this->windowArea.x = this->windowLeft;
    this->windowArea.y = this->windowTop;
    this->windowArea.w = this->windowRight;
    this->windowArea.h = this->windowBottom;
    
    mIsWindowRunning = true;
    return (void*)windowHandle;
#endif
    
}

void PlatformLayer::DestroyWindowHandle(void) {
#ifdef PLATFORM_WINDOWS
    DestroyWindow((HWND)windowHandle);
    windowHandle = nullptr;
    
    mIsWindowRunning = false;
#endif
    
#ifdef PLATFORM_LINUX
    SDL_DestroyWindow((SDL_Window*)windowHandle);
    windowHandle = nullptr;
    mIsWindowRunning = false;
#endif
    return;
}

void PlatformLayer::SetWindowCenter(void) {
#ifdef PLATFORM_WINDOWS
    glm::vec2 dim = GetDisplaySize();
    displayWidth  = dim.x;
    displayHeight = dim.y;
    
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
    windowArea.x = windowDim.left + horizontalMargin + 2.0f;
    windowArea.y = windowDim.top + verticalMargin - 2.0f;
    windowArea.w = clientRect.right - clientRect.left;
    windowArea.h = windowDim.bottom - windowDim.top;
#endif
#ifdef PLATFORM_LINUX
    int screenW, screenH;
    SDL_DisplayID display = SDL_GetDisplayForWindow((SDL_Window*)windowHandle);
    SDL_Rect usableBounds;
    SDL_GetDisplayUsableBounds(display, &usableBounds);
    
    int winW, winH;
    SDL_GetWindowSize((SDL_Window*)windowHandle, &winW, &winH);
    
    int x = usableBounds.x + (usableBounds.w - winW) / 2;
    int y = usableBounds.y + (usableBounds.h - winH) / 2;
    
    SDL_SetWindowPosition((SDL_Window*)windowHandle, x, y);
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
    windowArea.x = windowDim.left + horizontalMargin + 2.0f;
    windowArea.y = windowDim.top + verticalMargin - 2.0f;
    windowArea.w = clientRect.right - clientRect.left;
    windowArea.h = windowDim.bottom - windowDim.top;
#endif
#ifdef PLATFORM_LINUX
    
    int windowWidth = displayWidth * width;
    int windowHeight = displayHeight * height;
    
    SDL_SetWindowSize((SDL_Window*)windowHandle, windowWidth, windowHeight);
    
    int windowX = (this->displayWidth / 2) - (windowWidth / 2);
    int windowY = (this->displayHeight / 2) - (windowHeight / 2);
    
    SDL_SetWindowPosition((SDL_Window*)windowHandle, windowX, windowY);
#endif
    return;
}

void PlatformLayer::SetWindowPosition(Viewport windowSize) {
#ifdef PLATFORM_WINDOWS
    SetWindowPos((HWND)windowHandle, NULL, windowSize.x, windowSize.y, windowSize.w, windowSize.h, SWP_SHOWWINDOW);
    
    windowArea.w = windowSize.w;
    windowArea.h = windowSize.h;
#endif
    
#ifdef PLATFORM_LINUX
    SDL_SetWindowPosition((SDL_Window*)windowHandle, windowSize.x, windowSize.y);
#endif
    
    return;
}

Viewport PlatformLayer::GetWindowArea(void) {
    Viewport area;
    
#ifdef PLATFORM_WINDOWS
    RECT windowSz;
    GetWindowRect((HWND)windowHandle, &windowSz);
    
    area.x = windowSz.left;
    area.y = windowSz.top;
    area.w = (windowSz.right  - windowSz.left);
    area.h = (windowSz.bottom - windowSz.top);
#endif
    
#ifdef PLATFORM_LINUX
    SDL_GetWindowPosition((SDL_Window*)windowHandle, &area.x, &area.y);
    SDL_GetWindowSize((SDL_Window*)windowHandle, &area.w, &area.h);
#endif
    
    return area;
}

glm::vec2 PlatformLayer::GetDisplaySize(void) {
    glm::vec2 dim;
#ifdef PLATFORM_WINDOWS
    dim.x = GetDeviceCaps( (HDC)deviceContext, HORZRES );
    dim.y = GetDeviceCaps( (HDC)deviceContext, VERTRES );
#endif
#ifdef PLATFORM_LINUX
    SDL_Rect bounds;
    SDL_DisplayID display = SDL_GetDisplayForWindow((SDL_Window*)windowHandle);
    
    SDL_GetDisplayBounds(display, &bounds);
    dim.x = bounds.w;
    dim.y = bounds.h;
#endif
    return dim;
}

void PlatformLayer::WindowEnableFullscreen(void) {
#ifdef PLATFORM_WINDOWS
    SetWindowLongPtr((HWND)windowHandle, GWL_STYLE, WS_POPUP);
    SetWindowPos((HWND)windowHandle, HWND_TOPMOST, 0, 0, displayWidth, displayHeight, SWP_FRAMECHANGED);
    ShowWindow((HWND)windowHandle, SW_MAXIMIZE);
#endif
#ifdef PLATFORM_LINUX
    SDL_SetWindowFullscreen((SDL_Window*)windowHandle, 1);
#endif
    return;
}

void PlatformLayer::WindowDisableFullscreen(void) {
#ifdef PLATFORM_WINDOWS
    SetWindowLongPtr((HWND)windowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
    SetWindowPos((HWND)windowHandle, HWND_NOTOPMOST, windowArea.x, windowArea.y, windowArea.w, windowArea.h, SWP_FRAMECHANGED);
    ShowWindow((HWND)windowHandle, SW_RESTORE);
#endif
#ifdef PLATFORM_LINUX
    SDL_SetWindowFullscreen((SDL_Window*)windowHandle, 0);
#endif
    return;
}

void PlatformLayer::HideWindowHandle(void) {
#ifdef PLATFORM_WINDOWS
    ShowWindow((HWND)windowHandle, false);
#endif
#ifdef PLATFORM_LINUX
    SDL_HideWindow((SDL_Window*)windowHandle);
#endif
    return;
}

void PlatformLayer::ShowWindowHandle(void) {
#ifdef PLATFORM_WINDOWS
    ShowWindow((HWND)windowHandle, true);
#endif
#ifdef PLATFORM_LINUX
    SDL_ShowWindow((SDL_Window*)windowHandle);
#endif
    return;
}

void PlatformLayer::ShowMouseCursor(void) {
#ifdef PLATFORM_WINDOWS
    while (ShowCursor(true) < 0);
#endif
#ifdef PLATFORM_LINUX
    SDL_ShowCursor();
#endif
}

void PlatformLayer::HideMouseCursor(void) {
#ifdef PLATFORM_WINDOWS
    while (ShowCursor(false) >= 0);
#endif
#ifdef PLATFORM_LINUX
    SDL_HideCursor();
#endif
}

int PlatformLayer::GetTaskbarHeight(void) {

    return 0;
}

int PlatformLayer::GetTitlebarHeight(void) {
    
    return 0;
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
    std::string text;
    
#ifdef PLATFORM_WINDOWS
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
    int colorBits = 32;
    int depthBits = 32;
    
#ifdef PLATFORM_WINDOWS
    
    int iFormat;
    
    // Set the window handle and get the device context
    //windowHandle  = (HWND)windowHandle;
    deviceContext = GetDC((HWND)windowHandle);
    
    // Pixel format descriptor
    PIXELFORMATDESCRIPTOR pfd;
    
    ZeroMemory (&pfd, sizeof (pfd));
    
    pfd.nSize       = sizeof (pfd);
    pfd.nVersion    = 1;
    pfd.dwFlags     = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType  = PFD_TYPE_RGBA;
    pfd.cColorBits  = colorBits;
    pfd.cDepthBits  = depthBits;
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
    
#ifdef PLATFORM_LINUX
    
    // Get the render context for openGL
    deviceContext = (void*)SDL_GL_CreateContext( (SDL_Window*)windowHandle );
    
    if (!deviceContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_GL_MakeCurrent((SDL_Window*)windowHandle, (SDL_GLContext)deviceContext);
    
#endif
    
    //
    // Log hardware details
    
    // Initiate glew after setting the render target
    GLenum passed = glewInit();
    
#ifdef  LOG_RENDER_DETAILS
    std::string gcVendor, gcRenderer, gcExtensions, gcVersion, Line;
    
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
    Line = DetailStringHead + "Color" + DetailStringEqu + Int.ToString(colorBits) + " bit"; Log.Write(Line);
    Line = DetailStringHead + "Depth" + DetailStringEqu + Int.ToString(depthBits) + " bit"; Log.Write(Line);
    Log.WriteLn();
    Log.WriteLn();
    
#endif
    
    return passed;
}
