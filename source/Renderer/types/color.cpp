#include "color.h"

Color Color::operator+ (const Color& color) {
    Color newColor;
    newColor.r = r + color.r;
    newColor.g = g + color.g;
    newColor.b = b + color.b;
    newColor.a = a + color.a;
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

void Color::operator= (const Color& color) {
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;
    return;
}

Color::Color() {
    r = 0;
    g = 0;
    b = 0;
    a = 1;
    return;
}

Color::Color(Color& color) {
    r = color.r;
    g = color.g;
    b = color.b;
    a = color.a;
    return;
}

Color::Color(float red, float green, float blue, float alpha) {
    r = red;
    g = green;
    b = blue;
    a = alpha;
    return;
}

Color::Color(float red, float green, float blue) {
    r = red;
    g = green;
    b = blue;
    a = 1;
    return;
}

Color& ColorPreset::Make(float r, float g, float b) {
    mCustom.r = r;
    mCustom.g = g;
    mCustom.b = b;
    return mCustom;
}

Color& ColorPreset::MakeRandom(void) {
    mRandom.r = ((float)(rand() % (int)100) + 1) * 0.01;
    mRandom.g = ((float)(rand() % (int)100) + 1) * 0.01;
    mRandom.b = ((float)(rand() % (int)100) + 1) * 0.01;
    return mRandom;
}
