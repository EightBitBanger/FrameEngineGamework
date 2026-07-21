#ifndef USER_INTERFACE_TEXT_ELEMENT
#define USER_INTERFACE_TEXT_ELEMENT

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <string>

class ENGINE_API Text {
    
public:
    
    bool isActive;
    bool doFadeout;
    
    int x;
    int y;
    
    Color color;
    
    std::string text;
    
    Text();
    
};

#endif
