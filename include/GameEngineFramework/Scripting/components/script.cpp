#include <GameEngineFramework/Scripting/components/script.h>

// Dummy landing function
void DefaultFunctionPtr(void*) {return;}


Script::Script() : 
    
    name(""),
    
    isActive(false),
    hasBeenInitiated(false),
    
    gameObject(nullptr),
    
    OnCreate( DefaultFunctionPtr ),
    OnUpdate( DefaultFunctionPtr )
{
}
