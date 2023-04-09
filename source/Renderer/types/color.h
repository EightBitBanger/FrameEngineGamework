#ifndef _COLOR_TYPE__
#define _COLOR_TYPE__

struct Color {
    
    float r;
    float g;
    float b;
    float a;
    
    Color() {
        r = 0;
        g = 0;
        b = 0;
        a = 1;
    }
    
    Color(Color& color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
    }
    
    Color operator+ (const Color& color) {
        Color newColor;
        newColor.r = r + color.r;
        newColor.g = g + color.g;
        newColor.b = b + color.b;
        newColor.a = a + color.a;
        return newColor;
    }
    
    void operator= (const Color& color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
        return;
    }
    
    
    Color(float red, float green, float blue, float alpha) {
        r = red;
        g = green;
        b = blue;
        a = alpha;
    }
    
    Color(float red, float green, float blue) {
        r = red;
        g = green;
        b = blue;
        a = 1;
    }
    
    
};






#endif
