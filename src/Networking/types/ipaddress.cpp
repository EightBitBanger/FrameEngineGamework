#include <GameEngineFramework/Networking/types/ipaddress.h>

#include <sstream>

IPAddress::IPAddress() {
    addr[0] = 0;
    addr[1] = 0;
    addr[2] = 0;
    addr[3] = 0;
    return;
}

std::string IPAddress::ToString(void) {
    std::stringstream stream;
    stream << (unsigned int)addr[0] << ".";
    stream << (unsigned int)addr[1] << ".";
    stream << (unsigned int)addr[2] << ".";
    stream << (unsigned int)addr[3];
    return stream.str();
}
