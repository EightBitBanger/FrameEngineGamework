#ifndef _NETWORKING_SUPPORT__
#define _NETWORKING_SUPPORT__

#include <GameEngineFramework/configuration.h>
#include <WS2tcpip.h>

#include <thread>
#include <mutex>
#include <chrono>

#include <string>
#include <vector>



// TODO: Tell the clients we have shutdown   CPP Line 172



class ENGINE_API NetworkSystem {
    
public:
    
    // Server
    
    /// Start hosting a server on a given port.
    bool StartHost(unsigned int port);
    
    /// Stop hosting the server.
    bool StopHost(void);
    
    /// Get the number of connected sockets.
    unsigned int GetNumberOfSockets(void);
    
    /// Get a socket from a connected client by the given index.
    SOCKET GetClientSocket(unsigned int index);
    
    /// Get a message from a connected client by the given index.
    std::string GetClientMessage(unsigned int index);
    
    /// Clear the message buffer for a connected client by the given index.
    void ClearClientMessage(unsigned int index);
    
    /// Send a message to a client by the given address.
    bool SendMessageToClient(unsigned int index, std::string& message);
    
    // Client
    
    /// Connect to a server at the given IP address and port.
    bool ConnectToHost(std::string ipAddress, unsigned int port);
    
    /// Disconnect from the host.
    bool DisconnectFromHost(void);
    
    /// Send a message to the connected host.
    bool SendMessageToHost(std::string& message);
    
    /// Receive a message from the connected host. A blank string indicates no messages are available.
    bool ReceiveMessageFromHost(std::string& message);
    
    // State
    
    /// Return true if our socket is connected.
    bool GetConnectionState(void);
    
    /// Return true if we are hosting a server.
    bool GetHostState(void);
    
    /// Get the number of connected sockets.
    SOCKET GetHostSocket(void);
    
    /// Initiate the network system.
    void Initiate(void);
    
    /// Shutdown the network system.
    void Shutdown(void);
    
    /// Update network sockets.
    void Update(void);
    
    NetworkSystem();
    
private:
    
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
    
};




#endif
