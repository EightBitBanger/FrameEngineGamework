#ifndef _COLOR_TYPE__
#define _COLOR_TYPE__

#include <cstdlib>


class Color {
    
public:
    
    float r;
    float g;
    float b;
    float a;
    
    Color operator+ (const Color& color);
    void operator= (const Color& color);
    
    Color();
    Color(Color& color);
    Color(float red, float green, float blue, float alpha);
    Color(float red, float green, float blue);
    
};


class ColorPreset {
    
public:
    
    Color red      = {1, 0, 0, 1};
    Color green    = {0, 1, 0, 1};
    Color blue     = {0, 0, 1, 1};
    
    Color dkred    = {0.2, 0, 0, 1};
    
    Color yellow   = {0.87, 0.87, 0, 1};
    Color orange   = {0.8, 0.3, 0, 1};
    
    Color purple   = {0.87, 0.0, 0.87, 1};
    
    Color gray     = {0.5, 0.5, 0.5, 1};
    Color ltgray   = {0.87, 0.87, 0.87, 1};
    Color dkgray   = {0.18, 0.18, 0.18, 1};
    
    Color white    = {1, 1, 1, 1};
    Color black    = {0, 0, 0, 1};
    
    Color random;
    Color custom;
    
    Color& Make(float r, float g, float b);
    Color& MakeRandom(void);
    
};


#endif
