#include "script.h"

Script::Script() {
    OnCreate = DefaultFunctionPtr;
    OnUpdate = DefaultFunctionPtr;
}

void DefaultFunctionPtr(void) {return;}

