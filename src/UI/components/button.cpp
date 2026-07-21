#include <GameEngineFramework/UI/components/sprite.h>
#include <GameEngineFramework/UI/components/button.h>

Button::Button() : 
    isActive(true),
    
    x(0),
    y(0),
    w(0),
    h(0),
    
    sprite(nullptr),
    callback(nullptr),
    
    doLeftClick(true),
    doRightClick(false),
    doCenterClick(false)
{}
