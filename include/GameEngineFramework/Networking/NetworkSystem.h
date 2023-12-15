#ifndef _NETWORKING_SUPPORT__
#define _NETWORKING_SUPPORT__

#include <GameEngineFramework/configuration.h>
#include <WS2tcpip.h>

#include <thread>
#include <mutex>
#include <chrono>

#include <string>
#include <vector>


class ENGINE_API NetworkSystem {
    
public:
    
    // Server
    
    /// Start hosting a server on a given port.
    bool StartHost(unsigned int port);
    
    /// Stop hosting the server.
    bool StopHost(void);
    
    // Client
    
    /// Connect to a server at the given IP address and port.
    bool ConnectToHost(std::string ipAddress, unsigned int port);
    
    /// Disconnect from the host.
    bool DisconnectFromHost(void);
    
    // State
    
    /// Return true if our socket is connected.
    bool GetConnectionState(void);
    
    /// Return true if we are hosting a server.
    bool GetHostState(void);
    
    /// Initiate the network system.
    void Initiate(void);
    
    /// Shutdown the network system.
    void Shutdown(void);
    
    /// Update network sockets.
    void Update(void);
    
    NetworkSystem();
    
    
    
    // State are we currently connected?
    bool mIsConnected;
    
    // State are we hosting a server?
    bool mIsHosting;
    
    // Host listener or client socket, depending on the connection type
    SOCKET mSocket;
    
    // Message buffer
    char buffer[1024];
    
    // Client socket list
    std::vector<SOCKET>       mSockets;
    std::vector<std::string>  mMessages;
    
private:
    
};




#endif
