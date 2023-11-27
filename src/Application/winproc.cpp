#include "winproc.h"
#include <windowsx.h>
extern InputSystem    Input;
extern RenderSystem   Renderer;

extern ApplicationLayer Application;


LRESULT CALLBACK WindowProc2(HWND wHnd, UINT Messages, WPARAM wParam, LPARAM lParam) {
    return 0;
}

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
        
        case WM_SIZE: // Window resize
            
            RECT WindowRect;
            GetWindowRect(wHnd, &WindowRect);
            
            // Set window view port
            Renderer.viewport.x = WindowRect.left;
            Renderer.viewport.y = WindowRect.top;
            Renderer.viewport.w = WindowRect.right - WindowRect.left;
            Renderer.viewport.h = WindowRect.bottom - WindowRect.top;
            
            // Update scene cameras
            for (unsigned int i=0; i < Renderer.GetRenderQueueSize(); i++) {
                
                if (Renderer[i]->camera == nullptr) 
                    continue;
                
                // Fixed view port
                if (Renderer[i]->camera->isFixedAspect) 
                    continue;
                
                // Update view port
                int windowWidth  = WindowRect.right - WindowRect.left;
                int windowHeight = WindowRect.bottom - WindowRect.top;
                
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
        
        case WM_GETMINMAXINFO: { // Minimum window size
            
            LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
            lpMMI -> ptMinTrackSize.x = WINDOW_WIDTH_MIN;
            lpMMI -> ptMinTrackSize.y = WINDOW_HEIGHT_MIN;
            
            break;
        }
        
        case WM_CLOSE: // Window (X) close button
            
            Application.isActive = false;
            PostQuitMessage(0);
            
            break;
        
        
        default: 
            return DefWindowProc (wHnd, Messages, wParam, lParam);
        
    }
    
    return 0;
}
