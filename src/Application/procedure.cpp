#include <GameEngineFramework/Application/procedure.h>
#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/UI/UserInterfaceSystem.h>

extern InputSystem          Input;
extern RenderSystem         Renderer;
extern PlatformLayer        Platform;
extern UserInterfaceSystem  UI;

#ifdef PLATFORM_WINDOWS

extern "C" ENGINE_API LRESULT CALLBACK WindowProc(HWND wHnd, UINT Messages, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND wHnd, UINT Messages, WPARAM wParam, LPARAM lParam) {
    
    uintptr_t wHandle     = reinterpret_cast<uintptr_t>(wHnd);
    uint32_t message      = static_cast<uint32_t>(Messages);
    uintptr_t wParameter  = static_cast<uintptr_t>(wParam);
    uintptr_t lParameter  = static_cast<uintptr_t>(lParam);
    
    UI.ProcedureHandler(wHandle, message, wParameter, lParameter);
    
    switch (Messages) {
        
        case WM_KEYDOWN: Input.SetKeyPressed(wParam); Input.lastKeyPressed = wParam; break;
        case WM_KEYUP:   Input.SetKeyReleased(wParam); Input.lastKeyReleased = wParam; break;
        
        case WM_LBUTTONDOWN:
            Input.SetMouseLeftPressed(true);
            Input.SetMouseLeftReleased(false);
            break;
        
        case WM_LBUTTONUP:
            Input.SetMouseLeftPressed(false);
            Input.SetMouseLeftReleased(true);
            break;
        
        case WM_RBUTTONDOWN:
            Input.SetMouseRightPressed(true);
            Input.SetMouseRightReleased(false);
            break;
        
        case WM_RBUTTONUP:
            Input.SetMouseRightPressed(false);
            Input.SetMouseRightReleased(true);
            break;
        
        case WM_MBUTTONUP: 
            Input.SetMouseMiddlePressed(false);
            Input.SetMouseMiddleReleased(true);
            break;
        
        case WM_MBUTTONDOWN: 
            Input.SetMouseMiddlePressed(true);
            Input.SetMouseMiddleReleased(false);
            break;
        
        case WM_MOUSEWHEEL: {
            float mouseWheelDelta = (float)GET_WHEEL_DELTA_WPARAM(wParam);
            if (mouseWheelDelta > 0) mouseWheelDelta = 1;
            if (mouseWheelDelta < 0) mouseWheelDelta = -1;
            Input.mouseWheelDelta = mouseWheelDelta;
            break;
        }
        
        case WM_SIZE: {
            Viewport viewport;
            viewport = Platform.GetWindowArea();
            int x = viewport.x;
            int y = viewport.y;
            int w = viewport.w;
            int h = viewport.h;
            
            // Set window view port
            Renderer.viewport.x = x;
            Renderer.viewport.y = y;
            Renderer.viewport.w = w;
            Renderer.viewport.h = h;
            
            Platform.windowArea.x = Renderer.viewport.x;
            Platform.windowArea.y = Renderer.viewport.y;
            Platform.windowArea.w = Renderer.viewport.w;
            Platform.windowArea.h = Renderer.viewport.h;
            
            Renderer.SetViewport(0, 0, Platform.windowArea.w, Platform.windowArea.h);
            
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
        
        case WM_GETMINMAXINFO: { // Minimum window size
            
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI -> ptMinTrackSize.x = WINDOW_WIDTH_MIN;
            lpMMI -> ptMinTrackSize.y = WINDOW_HEIGHT_MIN;
            
            break;
        }
        
        
        case WM_MOVE: {
            
            RECT windowDim;
            GetWindowRect(wHnd, &windowDim);
            
            RECT clientRect;
            GetClientRect(wHnd, &clientRect);
            
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
            Platform.windowArea.x = windowDim.left + horizontalMargin + 2.0f;
            Platform.windowArea.y = windowDim.top + verticalMargin - 2.0f;
            Platform.windowArea.w = clientRect.right - clientRect.left;
            Platform.windowArea.h = windowDim.bottom - windowDim.top;
            
            break;
        }
        
        case WM_KILLFOCUS: 
            
            Platform.EventCallbackLoseFocus();
            
            break;
        
        case WM_CLOSE: // Window (X) close button
            
            Platform.isActive = false;
            PostQuitMessage(0);
            
            break;
        
        
        default: 
            return DefWindowProc (wHnd, Messages, wParam, lParam);
        
    }
    
    return 0;
}

#endif

