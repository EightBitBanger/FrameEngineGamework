#ifndef USER_INTERFACE_PANEL_ELEMENT
#define USER_INTERFACE_PANEL_ELEMENT

#include <GameEngineFramework/configuration.h>
#include <string>

class ENGINE_API Panel {
public:
    
    std::string name;
    bool isActive;
    
    int x, y, w, h;
    float alpha;
    
    Panel();
    
};

#endif
