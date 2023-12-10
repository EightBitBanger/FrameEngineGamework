#ifndef _SOCKET_IP_ADDRESS_TYPE__
#define _SOCKET_IP_ADDRESS_TYPE__

#include <GameEngineFramework/configuration.h>

#include <string>

class ENGINE_API IPAddress {
    
public:
    
    unsigned char addr[4] = {0, 0, 0, 0};
    
    IPAddress();
    
    std::string ToString(void);
    
};

#endif
