#ifndef _VIEWPORT_TYPE__
#define _VIEWPORT_TYPE__

class __declspec(dllexport) Viewport {
    
public:
    
    /// View left.
    int x;
    /// View top.
    int y;
    /// View width.
    int w;
    /// View height.
    int h;
    
    Viewport();
    Viewport(int x, int y, int w, int h);
    
};

#endif
