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

#define NETWORK_DEBUG_CONSOLE


class ENGINE_API SocketServer {
    
public:
    
    friend class NetworkSystem;
    
    // Client
    
    /// Connect to a server at the given address and port number.
    bool ConnectToServer(std::string address, unsigned int port);
    
    /// Disconnect from the server.
    bool DisconnectFromServer(void);
    
    // Server
    
    /// Start a server listening for incoming connections.
    bool InitiateServer(unsigned int port, unsigned int maxConn=MAX_CONNECTIONS);
    
    /// Stop a currently running server.
    bool ShutdownServer(void);
    
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
    
    /// IP address from the last client to access the server.
    IPAddress    GetLastAddress(void);
    
    /// Index position of the last client to access the server.
    unsigned int GetLastIndex(void);
    
    // Active connections
    
    /// Get the number of clients in the connections list.
    unsigned int GetNumberOfConnections(void);
    
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
    void MessageSend(SOCKET socket, const char* buffer, unsigned int bufferSize);
    
    /// Receive a message
    int MessageReceive(SOCKET socket, char* buffer, unsigned int bufferSize);
    
    SocketServer(void);
    ~SocketServer(void);
    
    /// Internal timer class
    Timer time;
    
    /// Host socket.
    SOCKET  mSocket;
    
private:
    
    std::string   mLastHost;
    unsigned int  mLastPort;
    IPAddress     mLastAddress;
    unsigned int  mLastIndex;
    
    bool    mIsConnected;
    
    std::vector<SOCKET>            mSocketList;
    std::vector<unsigned int>      mPortList;
    std::vector<IPAddress>         mAddressList;
    std::vector<int>               mTimeoutList;
    std::vector<std::string>       mBufferList;
    
};

#endif
