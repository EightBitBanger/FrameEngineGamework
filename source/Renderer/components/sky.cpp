#include "sky.h"

Sky::Sky() {
    
    background = Color(0.87, 0.87, 0.87);
    return;
}

void Sky::SetColor(Color newColor) {
    background = newColor;
    return;
}
