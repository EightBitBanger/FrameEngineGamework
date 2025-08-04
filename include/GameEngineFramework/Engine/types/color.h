#ifndef _COLOR_TYPE__
#define _COLOR_TYPE__

#include <GameEngineFramework/configuration.h>

#include <glm/glm.hpp>
#include <cstdlib>


class ENGINE_API Color {
    
public:
    
    /// Red color value.
    float r;
    /// Green color value.
    float g;
    /// Blue color value.
    float b;
    /// Alpha color value.
    float a;
    
    Color operator+ (const Color& color);
    Color operator* (const Color& color);
    Color operator- (const Color& color);
    Color& operator= (const Color& color);
    
    Color operator+ (const float& grayScale);
    Color operator* (const float& grayScale);
    Color operator- (const float& grayScale);
    
    Color operator+= (const Color& color);
    Color operator-= (const Color& color);
    Color operator*= (const Color& color);
    
    Color operator+= (const float& grayScale);
    Color operator-= (const float& grayScale);
    Color operator*= (const float& grayScale);
    
    bool operator>  (const Color& color);
    bool operator>= (const Color& color);
    bool operator<  (const Color& color);
    bool operator<= (const Color& color);
    bool operator== (const Color& color);
    
    Color();
    Color(const Color& color);
    
    Color(float grayScale);
    Color(float red, float green, float blue, float alpha);
    Color(float red, float green, float blue);
    
    Color(const glm::vec3& vec);
    Color& operator=(const glm::vec3& vec);
    
    glm::vec3 ToVec3(void);
    
};


class ENGINE_API ColorPreset {
    
public:
    
    Color red      = {1, 0, 0, 1};
    Color green    = {0, 1, 0, 1};
    Color blue     = {0, 0, 1, 1};
    
    Color dkred    = {0.2, 0, 0, 1};
    Color dkgreen  = {0, 0.2, 0, 1};
    Color dkblue   = {0, 0, 0.2, 1};
    
    Color yellow   = {0.87, 0.87, 0, 1};
    Color orange   = {0.8, 0.3, 0, 1};
    
    Color purple   = {0.87, 0.0, 0.87, 1};
    
    Color gray     = {0.5, 0.5, 0.5, 1};
    Color ltgray   = {0.87, 0.87, 0.87, 1};
    Color dkgray   = {0.18, 0.18, 0.18, 1};
    
    Color white    = {1, 1, 1, 1};
    Color black    = {0, 0, 0, 1};
    
    Color brown    = {0.275, 0.18, 0.07};
    
    /// Return a random color interpolated between the given color values.
    Color& Range(Color begin, Color end);
    
    /// Return a color from given red, green and blue values.
    Color& Make(float r, float g, float b);
    
    /// Return a random color.
    Color& MakeRandom(void);
    
    /// Return a gray scale colors from a given scale value.
    Color& MakeGrayScale(float grayScale);
    
    /// Return a random gray scale color.
    Color& MakeRandomGrayScale(void);
    
    /// Interpolate between two colors by a linear bias factor.
    Color& Lerp(Color min, Color max, float bias);
    
    
private:
    
    Color mRandom;
    Color mCustom;
    
};


#endif
