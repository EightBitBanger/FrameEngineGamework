#ifndef USER_INTERFACE_SPRITE_ELEMENT
#define USER_INTERFACE_SPRITE_ELEMENT

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Renderer/components/texture.h>
#include <string>

class ENGINE_API Sprite {
public:
    
    std::string name;
    bool isActive;
    
    Texture* texture;
    
    int x, y;
    int w, h;
    
    float u0, v0;
    float u1, v1;
    
    Color tint;
    
    Sprite();
    
};

#endif
