#include "winproc.h"

extern InputSystem    Input;
extern RenderSystem   Renderer;

extern ApplicationLayer Application;

LRESULT CALLBACK WindowProc(HWND wHnd, UINT Messages, WPARAM wParam, LPARAM lParam) {
    
    switch (Messages) {
        
        case WM_KEYDOWN: Input.SetKeyPressed(wParam); Input.LastKeyPressed = wParam + 0x20; break;
        case WM_KEYUP:   Input.SetKeyReleased(wParam); break;
        
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
        
        // Window resize
        case WM_SIZE: 
            
            RECT WindowRect;
            GetWindowRect(wHnd, &WindowRect);
            
            Renderer.SetViewport(0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top);
            
            if (Renderer.cameraMain != nullptr) {
                
                Renderer.cameraMain->aspect = Renderer.viewport.w / Renderer.viewport.h;
                
                if (Renderer.cameraMain->aspect < 1.3)
                    Renderer.cameraMain->aspect = 1.3;
            }
            
            break;
        
        // Minimum window size
        case WM_GETMINMAXINFO: {
            
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI -> ptMinTrackSize.x = WINDOW_WIDTH_MIN;
            lpMMI -> ptMinTrackSize.y = WINDOW_HEIGHT_MIN;
            
            break;
        }
        
        case WM_SETCURSOR: 
            SetCursor(Application.hCursor);
            break;
        
        // Close button handling
        case WM_CLOSE:
            
            Application.isActive = false;
            PostQuitMessage(0);
            
            break;
        
        
        default: 
            return DefWindowProc (wHnd, Messages, wParam, lParam);
        
    }
    
    return 0;
}

