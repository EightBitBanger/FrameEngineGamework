#ifndef _NETWORKING_SUPPORT__
#define _NETWORKING_SUPPORT__

#include <GameEngineFramework/Networking/ServerSocket.h>
#include <GameEngineFramework/Networking/ClientSocket.h>

#include <thread>
#include <mutex>
#include <chrono>


class ENGINE_API NetworkSystem {
    
public:
    
    // Client
    
    /// Connect to a host at the address and port.
    bool ConnectToServer(IPAddress address, unsigned int port);
    
    /// Disconnect from the host.
    bool DisconnectFromServer(void);
    
    // Server
    
    /// Start listening for incoming connections on the port.
    bool StartServer(unsigned int port);
    
    /// Stop listening for incoming connections.
    bool StopServer(void);
    
    /// Port number from the last client to access the server.
    unsigned int GetLastPort(void);
    /// Host name from the last client to access the server.
    std::string  GetLastHost(void);
    /// IP address from the last client to access the server.
    IPAddress    GetLastAddress(void);
    /// Index position of the last client to access the server.
    unsigned int GetLastIndex(void);
    
    // Active connections
    
    /// Get the number of hosts in the connections list.
    unsigned int GetNumberOfConnections(void);
    /// Get a host name by its index location in the connections list.
    std::string GetHostByIndex(unsigned int index);
    /// Find a host index location by its name.
    int FindHost(std::string name);
    
    /// Get a port by its index location in the connections list.
    unsigned int GetPortByIndex(unsigned int index);
    
    /// Get a socket by its index location in the connections list.
    SOCKET GetSocketByIndex(unsigned int index);
    
    /// Get a buffer string from a socket index location in the connections list.
    std::string GetBufferStringByIndex(unsigned int index);
    /// Get a buffer string from a socket index location in the connections list.
    void ClearBufferStringByIndex(unsigned int index);
    
    // Internal
    
    /// Initiate the network system.
    void Initiate(void);
    
    /// Shutdown the network system.
    void Shutdown(void);
    
    /// Update the network connections and timeouts.
    void Update(void);
    
    NetworkSystem();
    
private:
    
    // Networking thread
    std::thread* networkSystemThread;
    
    std::mutex mux;
    
    // State are we hosting or just a client
    bool mIsHosting;
    
    // State are we currently connected
    bool mIsConnected;
    
    // Host socket
    SocketServer mHost;
    
    // Buffer for incoming data
    std::string mBuffer;
    
};




#endif
