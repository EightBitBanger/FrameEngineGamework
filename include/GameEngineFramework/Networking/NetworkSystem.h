#ifndef _NETWORKING_SUPPORT__
#define _NETWORKING_SUPPORT__

#include <GameEngineFramework/Networking/ServerSocket.h>

#include <thread>
#include <mutex>
#include <chrono>

#include <string>
#include <vector>


class ENGINE_API NetworkSystem {
    
public:
    
    // Client
    
    /// Connect to a server at the given IP address and port.
    bool ConnectToServer(std::string address, unsigned int port);
    
    /// Disconnect from the server.
    bool DisconnectFromServer(void);
    
    /// Sends a message to the server.
    void SendMessageToServer(char*, unsigned int size);
    
    /// Get a buffer string from the server. A blank string indicates no messages are available.
    int GetMessageFromServer(char*, unsigned int size);
    
    // Server
    
    /// Start a server listening for incoming connections on the given port.
    bool StartServer(unsigned int port);
    
    /// Stop the server no longer listening for incoming connections.
    bool StopServer(void);
    
    /// Send a message to a specific client at the given index.
    bool Send(int index, char* message, unsigned int messageSz);
    
    // Active connections
    
    /// Check the connection state of the network.
    bool GetConnectionStatus(void);
    
    /// Check if we are the server.
    bool GetServerStatus(void);
    
    /// Get the number of hosts in the connections list.
    unsigned int GetNumberOfConnections(void);
    
    /// Get a port by its index location in the connections list.
    unsigned int GetPortByIndex(unsigned int index);
    
    /// Get a socket by its index location in the connections list.
    SOCKET GetSocketByIndex(unsigned int index);
    
    /// Get a buffer string from a socket index location in the connections list.
    std::string GetClientBufferByIndex(unsigned int index);
    
    /// Get a buffer string from a socket index location in the connections list.
    void ClearClientBufferByIndex(unsigned int index);
    
    // Internal
    
    /// Initiate the network system.
    void Initiate(void);
    
    /// Shutdown the network system.
    void Shutdown(void);
    
    /// Update the network connections and timeouts.
    void Update(void);
    
    NetworkSystem();
    
private:
    
    // Host socket sub system.
    SocketServer mHost;
    
    // Networking thread
    std::thread* networkSystemThread;
    
    std::mutex mux;
    
    // State are we hosting or just a client
    bool mIsHosting;
    
    // State are we currently connected
    bool mIsConnected;
    
    // Buffer for incoming data
    std::string mBuffer;
    
};




#endif
