#include <GameEngineFramework/Application/ApplicationLayer.h>

#define IDI_ICON  101

ApplicationLayer::ApplicationLayer() : 
    
    windowHandle(NULL),
    deviceContext(NULL),
    
    displayWidth(1024),
    displayHeight(800),
    
    windowLeft(0),
    windowTop(0),
    windowRight(0),
    windowBottom(0),
    
    isPaused(false),
    isActive(true),
    
    hCursor(nullptr),
    
    mIsWindowRunning(false)
{
}

void ApplicationLayer::Pause(void) {
    isPaused = !isPaused;
    return;
}

HWND ApplicationLayer::CreateWindowHandle(std::string className, std::string windowName, HWND parentHandle, LRESULT CALLBACK(*windowProcedure)(HWND, UINT, WPARAM, LPARAM)) {
    
    isPaused = false;
    isActive = true;
    
    hCursor = LoadCursor(NULL, IDC_ARROW);
    
    wClassEx.lpszClassName   = className.c_str();
    wClassEx.cbSize          = sizeof(WNDCLASSEX);
    wClassEx.style           = CS_OWNDC;
    wClassEx.lpfnWndProc     = (WNDPROC)windowProcedure;
    wClassEx.cbClsExtra      = 0;
    wClassEx.cbWndExtra      = 0;
    wClassEx.hInstance       = hInstance;
    wClassEx.lpszMenuName    = NULL;
    wClassEx.hCursor         = hCursor;
    wClassEx.hIcon           = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wClassEx.hIconSm         = LoadIcon(hInstance, IDI_APPLICATION);
    wClassEx.hbrBackground   = (HBRUSH)GetStockObject(BLACK_BRUSH);
    
    assert( RegisterClassEx(&wClassEx) );
    
    windowHandle = CreateWindowEx(0, className.c_str(), windowName.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, parentHandle, NULL, hInstance, NULL);
    assert(windowHandle != NULL);
    
    ShowWindow(windowHandle, true);
    SetCursor(hCursor);
    
#ifdef APPLICATION_CURSOR_HIDDEN_ON_START
    while (ShowCursor(false) >= 0);
#endif
    
    deviceContext = GetDC(windowHandle);
    
    displayWidth  = GetDeviceCaps(deviceContext, HORZRES);
    displayHeight = GetDeviceCaps(deviceContext, VERTRES);
    
    mIsWindowRunning = true;
    return windowHandle;
}

void ApplicationLayer::DestroyWindowHandle(void) {
    assert(windowHandle != NULL);
    
    DestroyWindow(windowHandle);
    windowHandle = NULL;
    
    mIsWindowRunning = false;
    return;
}

void ApplicationLayer::SetWindowCenter(void) {
    assert(windowHandle != NULL);
    
    int DisplayWidth  = GetDeviceCaps(deviceContext, HORZRES);
    int DisplayHeight = GetDeviceCaps(deviceContext, VERTRES);
    
    RECT windowSz;
    GetWindowRect(windowHandle, &windowSz);
    
    Viewport WindowSz;
    WindowSz.w = windowSz.right  - windowSz.left;
    WindowSz.h = windowSz.bottom - windowSz.top;
    
    WindowSz.x = (DisplayWidth  / 2) - (WindowSz.w / 2);
    WindowSz.y = (DisplayHeight / 2) - (WindowSz.h / 2);
    
    SetWindowPos(windowHandle, NULL, WindowSz.x, WindowSz.y, WindowSz.w, WindowSz.h, SWP_SHOWWINDOW);
    return;
}

void ApplicationLayer::SetWindowCenterScale(float width, float height) {
    assert(windowHandle != NULL);
    
    Viewport newWindowSz;
    newWindowSz.y = 0;
    newWindowSz.x = 0;
    newWindowSz.w = displayWidth  * width;
    newWindowSz.h = displayHeight * height;
    
    SetWindowPosition(newWindowSz);
    SetWindowCenter();
    return;
}

void ApplicationLayer::SetWindowPosition(Viewport windowSize) {
    assert(windowHandle != NULL);
    SetWindowPos(windowHandle, NULL, windowSize.x, windowSize.y, windowSize.w, windowSize.h, SWP_SHOWWINDOW);
    return;
}

Viewport ApplicationLayer::GetWindowArea(void) {
    assert(windowHandle != NULL);
    
    RECT windowSz;
    GetWindowRect(windowHandle, &windowSz);
    
    Viewport area;
    area.x = windowSz.left;
    area.y = windowSz.top;
    area.w = (windowSz.right  - windowSz.left);
    area.h = (windowSz.bottom - windowSz.top);
    
    return area;
}

void ApplicationLayer::HideWindowHandle(void) {
    assert(windowHandle != NULL);
    ShowWindow(windowHandle, false);
    return;
}

void ApplicationLayer::ShowWindowHandle(void) {
    assert(windowHandle != NULL);
    ShowWindow(windowHandle, true);
    return;
}

void ApplicationLayer::ShowMouseCursor(void) {
    while (ShowCursor(true) < 0);
}

void ApplicationLayer::HideMouseCursor(void) {
    while (ShowCursor(false) >= 0);
}
