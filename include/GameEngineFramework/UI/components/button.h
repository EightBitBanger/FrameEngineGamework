#ifndef USER_INTERFACE_BUTTON_ELEMENT
#define USER_INTERFACE_BUTTON_ELEMENT

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Renderer/components/texture.h>
#include <string>

class ENGINE_API Button {
public:
    
    bool isActive;
    
    int x, y;
    int w, h;
    
    Sprite* sprite;
    
    bool doLeftClick;
    bool doRightClick;
    bool doCenterClick;
    
    void(*callback)();
    
    Button();
};

#endif
