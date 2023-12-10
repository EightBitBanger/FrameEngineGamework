#ifndef _SOCKET_SERVER_
#define _SOCKET_SERVER_

#include <GameEngineFramework/Timer/Timer.h>
#include <GameEngineFramework/Networking/types/ipaddress.h>

#include <WS2tcpip.h>

#include <vector>
#include <string>

// Maximum number of connections
#define MAX_CONNECTIONS  20

// Log incoming connections
#define LOG_ACTIVITY

// Seconds before timing-out a connection
#define CONNECTION_TIMEOUT  120


class ENGINE_API SocketServer {
    
public:
    
    // Client
    
    /// Connect to a server at the given address and port number.
    int ConnectToServer(unsigned int port, std::string address);
    /// Disconnect from the server.
    void DisconnectFromServer(void);
    
    // Server
    
    /// Start a server listening for incoming connections.
    int InitiateServer(unsigned int port, unsigned int maxConn=MAX_CONNECTIONS);
    /// Stop a currently running server.
    void ShutdownServer(void);
    
    /// Check for an incoming connection request.
    SOCKET CheckIncomingConnections(void);
    /// Check for incoming messages from any connected client.
    int CheckIncomingMessages(char* buffer, unsigned int bufferSize);
    /// Check client timers for a time-out.
    int CheckTimers(void);
    
    /// Remove a client from the server.
    int DisconnectFromClient(unsigned int index);
    
    // Last client who accessed the server
    
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
    /// Find a host index location by its name.
    int FindHost(std::string name);
    /// Get a host name by its index location in the connections list.
    std::string GetHostByIndex(unsigned int index);
    
    /// Get a port by its index location in the connections list.
    unsigned int GetPortByIndex(unsigned int index);
    
    /// Get a socket by its index location in the connections list.
    SOCKET GetSocketByIndex(unsigned int index);
    
    /// Get a buffer string from a socket index location in the connections list.
    std::string GetBufferStringByIndex(unsigned int index);
    /// Get a buffer string from a socket index location in the connections list.
    void ClearBufferStringByIndex(unsigned int index);
    
    // Timeout counter
    
    /// Get a timer by its index location in the connections list.
    int GetTimerByIndex(unsigned int index);
    /// Set a timer value by its index location in the connections list.
    void SetTimerValue(unsigned int index, int value);
    
    
    // Messaging
    
    /// Send a message
    void MessageSend(SOCKET socket, char* buffer, unsigned int bufferSize);
    
    /// Receive a message
    int MessageReceive(SOCKET socket, char* buffer, unsigned int bufferSize);
    
    SocketServer(void);
    ~SocketServer(void);
    
    /// Internal timer class
    Timer time;
    
    
private:
    
    std::string   mLastHost;
    unsigned int  mLastPort;
    IPAddress     mLastAddress;
    unsigned int  mLastIndex;
    
    bool    mIsConnected;
    SOCKET  mSocket;
    
    std::vector<std::string>       mHostList;
    std::vector<unsigned int>      mPortList;
    std::vector<IPAddress>         mAddressList;
    std::vector<SOCKET>            mSocketList;
    std::vector<int>               mTimeoutList;
    std::vector<std::string>       mBufferList;
    
};

#endif
