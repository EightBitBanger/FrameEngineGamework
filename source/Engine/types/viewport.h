#ifndef _VIEWPORT_TYPE__
#define _VIEWPORT_TYPE__

struct Viewport {
    
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
