#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Math/Math.h>
#include <GameEngineFramework/Math/Random.h>

extern NumberGeneration Random;

Color Color::operator+ (const Color& color) {
    Color newColor;
    newColor.r = r + color.r;
    newColor.g = g + color.g;
    newColor.b = b + color.b;
    newColor.a = a + color.a;
    return newColor;
}

Color Color::operator* (const Color& color) {
    Color newColor;
    newColor.r = r * color.r;
    newColor.g = g * color.g;
    newColor.b = b * color.b;
    newColor.a = a * color.a;
    return newColor;
}

Color Color::operator- (const Color& color) {
    Color newColor;
    newColor.r = r - color.r;
    newColor.g = g - color.g;
    newColor.b = b - color.b;
    newColor.a = a - color.a;
    return newColor;
}

Color Color::operator+ (const float& grayScale) {
    Color newColor;
    newColor.r = r + grayScale;
    newColor.g = g + grayScale;
    newColor.b = b + grayScale;
    return newColor;
}

Color Color::operator* (const float& grayScale) {
    Color newColor;
    newColor.r = r * grayScale;
    newColor.g = g * grayScale;
    newColor.b = b * grayScale;
    return newColor;
}

Color Color::operator- (const float& grayScale) {
    Color newColor;
    newColor.r = r - grayScale;
    newColor.g = g - grayScale;
    newColor.b = b - grayScale;
    return newColor;
}


Color Color::operator-= (const Color& color) {
    Color newColor;
    r -= color.r;
    g -= color.g;
    b -= color.b;
    a -= color.a;
    newColor.r=r;
    newColor.g=g;
    newColor.b=b;
    newColor.a=a;
    return newColor;
}

Color Color::operator+= (const Color& color) {
    Color newColor;
    r += color.r;
    g += color.g;
    b += color.b;
    a += color.a;
    newColor.r=r;
    newColor.g=g;
    newColor.b=b;
    newColor.a=a;
    return newColor;
}

Color Color::operator= (const Color& color) {
    Color newColor;
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;
    newColor.r = r;
    newColor.g = g;
    newColor.b = b;
    newColor.a = a;
    return newColor;
}

Color Color::operator*= (const Color& color) {
    Color newColor;
    r *= color.r;
    g *= color.g;
    b *= color.b;
    a *= color.a;
    newColor.r *= r;
    newColor.g *= g;
    newColor.b *= b;
    newColor.a *= a;
    return newColor;
}

Color Color::operator+= (const float& grayScale) {
    Color newColor;
    r += grayScale;
    g += grayScale;
    b += grayScale;
    newColor.r += grayScale;
    newColor.g += grayScale;
    newColor.b += grayScale;
    return newColor;
}

Color Color::operator-= (const float& grayScale) {
    Color newColor;
    r -= grayScale;
    g -= grayScale;
    b -= grayScale;
    newColor.r -= grayScale;
    newColor.g -= grayScale;
    newColor.b -= grayScale;
    return newColor;
}

Color Color::operator*= (const float& grayScale) {
    Color newColor;
    r *= grayScale;
    g *= grayScale;
    b *= grayScale;
    newColor.r *= grayScale;
    newColor.g *= grayScale;
    newColor.b *= grayScale;
    return newColor;
}

bool Color::operator> (const Color& color) {
    if ((r > color.r) && 
        (g > color.g) && 
        (b > color.b)) 
        return true;
    return false;
}

bool Color::operator>= (const Color& color) {
    if ((r >= color.r) && 
        (g >= color.g) && 
        (b >= color.b)) 
        return true;
    return false;
}

bool Color::operator< (const Color& color) {
    if ((r < color.r) && 
        (g < color.g) && 
        (b < color.b)) 
        return true;
    return false;
}

bool Color::operator<= (const Color& color) {
    if ((r <= color.r) && 
        (g <= color.g) && 
        (b <= color.b)) 
        return true;
    return false;
}

bool Color::operator== (const Color& color) {
    if ((r == color.r) && 
        (g == color.g) && 
        (b == color.b)) 
        return true;
    return false;
}

Color::Color() : 
    r(0),
    g(0),
    b(0),
    a(1)
{
}

Color::Color(Color& color) : 
    r(color.r),
    g(color.g),
    b(color.b),
    a(color.a)
{
}

Color::Color(float grayScale) :  
    r(grayScale),
    g(grayScale),
    b(grayScale),
    a(1)
{
}


Color::Color(float red, float green, float blue, float alpha) : 
    r(red),
    g(green),
    b(blue),
    a(alpha)
{
}

Color::Color(float red, float green, float blue) : 
    r(red),
    g(green),
    b(blue),
    a(1)
{
}

Color& ColorPreset::Make(float r, float g, float b) {
    mCustom.r = r;
    mCustom.g = g;
    mCustom.b = b;
    return mCustom;
}

Color& ColorPreset::MakeRandom(void) {
    mRandom.r = Random.Range(0, 100) * 0.01f;
    mRandom.g = Random.Range(0, 100) * 0.01f;
    mRandom.b = Random.Range(0, 100) * 0.01f;
    return mRandom;
}

Color& ColorPreset::MakeGrayScale(float grayScale) {
    mCustom.r = grayScale;
    mCustom.g = grayScale;
    mCustom.b = grayScale;
    return mCustom;
}

Color& ColorPreset::MakeRandomGrayScale(void) {
    float randomScale = Random.Range(0, 100) * 0.01f;
    mRandom.r = randomScale;
    mRandom.g = randomScale;
    mRandom.b = randomScale;
    return mRandom;
}

Color& ColorPreset::Lerp(Color min, Color max, float bias) {
    mCustom = Color( glm::lerp(min.r, max.r, bias), 
                     glm::lerp(min.g, max.g, bias), 
                     glm::lerp(min.b, max.b, bias) );
    return mCustom;
}
