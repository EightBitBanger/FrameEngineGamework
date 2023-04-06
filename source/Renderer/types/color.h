
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
    
    
};



struct ColorPreset {
    
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
    
    Color& Make(float r, float g, float b) {
        custom.r = r;
        custom.g = g;
        custom.b = b;
        return custom;
    }
    
    Color& MakeRandom(void) {
        random.r = Random.Range(1, 100) * 0.01;
        random.g = Random.Range(1, 100) * 0.01;
        random.b = Random.Range(1, 100) * 0.01;
        return random;
    }
    
} Colors;



