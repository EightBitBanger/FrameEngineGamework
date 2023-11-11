#include "color.h"

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

Color& ColorPreset::MakeGrayScale(float grayScale) {
    mCustom.r = grayScale;
    mCustom.g = grayScale;
    mCustom.b = grayScale;
    return mCustom;
}

Color& ColorPreset::MakeRandom(void) {
    mRandom.r = ((float)(rand() % (int)100) + 1) * 0.01;
    mRandom.g = ((float)(rand() % (int)100) + 1) * 0.01;
    mRandom.b = ((float)(rand() % (int)100) + 1) * 0.01;
    return mRandom;
}
