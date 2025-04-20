#ifndef USER_INTERFACE_TEXT_INPUT_ELEMENT
#define USER_INTERFACE_TEXT_INPUT_ELEMENT

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Engine/types/color.h>
#include <string>

class ENGINE_API TextField {
    
public:
    
    bool isActive;
    bool doStayInFocus;
    
    int x;
    int y;
    
    int w;
    int h;
    
    Color color;
    
    std::string text;
    
    void(*callback)(std::string&);
    
    TextField();
    
};

#endif
