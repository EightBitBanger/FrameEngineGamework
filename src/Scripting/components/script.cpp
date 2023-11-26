#include "script.h"

Script::Script() : 
    
    name(""),
    
    isActive(false),
    hasBeenInitiated(false),
    
    gameObject(nullptr),
    
    OnCreate(nullfunc),
    OnUpdate(nullfunc)
{
}

// Dummy landing function
void DefaultFunctionPtr(void*) {return;}
