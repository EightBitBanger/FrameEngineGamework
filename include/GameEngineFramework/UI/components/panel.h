#ifndef USER_INTERFACE_PANEL_ELEMENT
#define USER_INTERFACE_PANEL_ELEMENT

#include <GameEngineFramework/configuration.h>
#include <string>

class ENGINE_API Panel {
    
public:
    
    bool isActive;
    
    int x;
    int y;
    
    int w;
    int h;
    
    Panel();
    
};

#endif
