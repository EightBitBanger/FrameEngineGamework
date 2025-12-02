#include <GameEngineFramework/Networking/NetworkSystem.h>
#include <GameEngineFramework/Logging/Logging.h>

extern NetworkSystem Network;
extern Logger Log;

NetworkSystem::NetworkSystem() : 
    mIsConnected(false),
    mIsHosting(false) {}

bool NetworkSystem::StartHost(unsigned int port) {
    
    if (mIsConnected) 
        return false;
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR) 
        return false;
    
    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }
    
    sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    socketAddress.sin_port = htons(port);
    
    if (bind(mSocket, (SOCKADDR*)&socketAddress, sizeof(socketAddress)) == SOCKET_ERROR) {
        closesocket(mSocket);
        WSACleanup();
        return false;
    }
    
    if (listen(mSocket, 8) == SOCKET_ERROR) {
        closesocket(mSocket);
        WSACleanup();
        return false;
    }
    
    u_long sockMode = 1; // Non blocking listener
    ioctlsocket(mSocket, FIONBIO, &sockMode);
    
    mIsConnected = true;
    mIsHosting   = true;
    
    return true;
}

bool NetworkSystem::StopHost(void) {
    
    if ((!mIsConnected) || (!mIsHosting))
        return false;
    
    closesocket(mSocket);
    WSACleanup();
    
    mIsConnected = false;
    mIsHosting = false;
    
    return true;
}

unsigned int NetworkSystem::GetNumberOfSockets(void) {
    return mSockets.size();
}

SOCKET NetworkSystem::GetClientSocket(unsigned int index) {
    if (index >= mSockets.size()) 
        return INVALID_SOCKET;
    return mSockets[index];
}

std::string NetworkSystem::GetClientMessage(unsigned int index) {
    if (index >= mMessages.size()) 
        return "";
    return mMessages[index];
}

void NetworkSystem::ClearClientMessage(unsigned int index) {
    if (index < mMessages.size())
        mMessages[index] = "";
    return;
}

bool NetworkSystem::SendMessageToClient(unsigned int index, std::string& message) {
    if (!mIsHosting) 
        return false;
    if (index >= mSockets.size()) 
        return false;
    int result = send(mSockets[index], message.c_str(), message.size(), 0);
    return (result != SOCKET_ERROR);
}

// Client

bool NetworkSystem::ConnectToHost(std::string ipAddress, unsigned int port) {
    
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR) 
        return false;
    
    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == INVALID_SOCKET) {
        WSACleanup();
        return false;
    }
    
    sockaddr_in socketAddress;
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    if (socketAddress.sin_addr.s_addr == INADDR_NONE) {
        closesocket(mSocket);
        WSACleanup();
        return false;
    }
    socketAddress.sin_port = htons(port);
    
    result = connect(mSocket, (SOCKADDR*)&socketAddress, sizeof(socketAddress));
    if (result == SOCKET_ERROR) {
        closesocket(mSocket);
        WSACleanup();
        return false;
    }
    
    u_long sockMode = 1; // Non blocking client
    ioctlsocket(mSocket, FIONBIO, &sockMode);
    
    mSockets.push_back(mSocket);
    mMessages.push_back("");
    
    mIsConnected = true;
    mIsHosting   = false;
    
    return true;
}

bool NetworkSystem::SendMessageToHost(std::string& message) {
    if (mIsHosting) 
        return false;
    int result = send(mSocket, message.c_str(), message.size(), 0);
    return (result != SOCKET_ERROR);
}

bool NetworkSystem::ReceiveMessageFromHost(std::string& message) {
    if (mIsHosting) 
        return false;
    char buffer[1024];
    int messageSz = recv(mSocket, buffer, sizeof(buffer) - 1, 0);
    if (messageSz > 0) {
        buffer[messageSz] = '\0';
        message.assign(buffer);
        return true;
    }
    return false;
}

bool NetworkSystem::DisconnectFromHost(void) {
    if (!mIsConnected) 
        return false;
    closesocket(mSocket);
    WSACleanup();
    mSockets.clear();
    mMessages.clear();
    mIsConnected = false;
    return true;
}

SOCKET NetworkSystem::GetHostSocket(void) {
    return mSocket;
}

void NetworkSystem::Initiate(void) {
    return;
}

void NetworkSystem::Shutdown(void) {
    
    if (!mIsConnected) 
        return;
    
    if (mIsHosting) {
        // Tell the clients we have shutdown
    } else {
        // Tell the host we have left
        std::string message = "DISCONN";
        send(Network.mSocket, message.c_str(), message.size(), 0);
    }
    
    closesocket(mSocket);
    WSACleanup();
    mIsConnected = false;
    
    return;
}

void NetworkSystem::Update(void) {
    
    if (!mIsConnected) 
        return;
    
    char buffer[1024];
    
    if (mIsHosting) {
        // Server update
        SOCKADDR_IN addrClient;
        int length = sizeof(SOCKADDR);
        
        SOCKET newSocket = accept(mSocket, (SOCKADDR*)&addrClient, &length);
        
        if (newSocket != INVALID_SOCKET) {
            // Accept the client socket
            mSockets.push_back(newSocket);
            mMessages.push_back("");
        }
        
        // Process client messages
        for (unsigned int i = 0; i < mSockets.size(); i++) {
            int messageSz = recv(mSockets[i], buffer, sizeof(buffer) - 1, 0);
            
            if (messageSz == SOCKET_ERROR) {
                int err = WSAGetLastError();
                if (err != WSAEWOULDBLOCK && err != WSAEINTR) {
                    // Handle error
                    closesocket(mSockets[i]);
                    mSockets.erase(mSockets.begin() + i);
                    mMessages.erase(mMessages.begin() + i);
                }
                continue;
            }
            
            // Disconnected
            if (messageSz == 0) {
                closesocket(mSockets[i]);
                mSockets.erase(mSockets.begin() + i);
                mMessages.erase(mMessages.begin() + i);
                continue;
            }
            
            buffer[messageSz] = '\0';
            mMessages[i] += buffer;
        }
        
    } else {
        // Client update
        int messageSz = recv(mSocket, buffer, sizeof(buffer) - 1, 0);
        
        if (messageSz == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK && err != WSAEINTR) {
                // Handle error
                closesocket(mSocket);
                mSockets.clear();
                mMessages.clear();
                mIsConnected = false;
            }
            return;
        }
        
        buffer[messageSz] = '\0';
        mMessages[0] += buffer;
    }
    
    return;
}

bool NetworkSystem::GetConnectionState(void) {
    return mIsConnected;
}

/// Return true if we are hosting a server.
bool NetworkSystem::GetHostState(void) {
    return mIsHosting;
}
