#include "color.h"

Color Color::operator+ (const Color& color) {
    Color newColor;
    newColor.r = r + color.r;
    newColor.g = g + color.g;
    newColor.b = b + color.b;
    newColor.a = a + color.a;
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
    custom.r = r;
    custom.g = g;
    custom.b = b;
    return custom;
}

Color& ColorPreset::MakeRandom(void) {
    random.r = ((float)(rand() % (int)100) + 1) * 0.01;
    random.g = ((float)(rand() % (int)100) + 1) * 0.01;
    random.b = ((float)(rand() % (int)100) + 1) * 0.01;
    return random;
}

