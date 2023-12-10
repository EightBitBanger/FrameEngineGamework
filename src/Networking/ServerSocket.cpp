#include <GameEngineFramework/Networking/ServerSocket.h>
#include <iostream>

SocketServer::SocketServer(void) :
    
    mLastPort(0),
    mLastIndex(0),
    
    mIsConnected(false),
    mSocket(0)
{
    mLastAddress.addr[0] = 127;
    mLastAddress.addr[1] = 0;
    mLastAddress.addr[2] = 0;
    mLastAddress.addr[3] = 1;
    
    return;
}

SocketServer::~SocketServer(void) {
    return;
}


unsigned int SocketServer::GetLastPort(void) {
    return mLastPort;
}

IPAddress SocketServer::GetLastAddress(void) {
    return mLastAddress;
}

unsigned int SocketServer::GetLastIndex(void) {
    return mLastIndex;
}

unsigned int SocketServer::GetNumberOfConnections(void) {
    return mSocketList.size();
}

SOCKET SocketServer::GetSocketByIndex(unsigned int index) {
    return mSocketList[index];
}

unsigned int SocketServer::GetPortByIndex(unsigned int index) {
    return mPortList[index];
}

std::string SocketServer::GetBufferStringByIndex(unsigned int index) {
    return mBufferList[index];
}

void SocketServer::ClearBufferStringByIndex(unsigned int index) {
    mBufferList[index] = "";
}

int SocketServer::GetTimerByIndex(unsigned int index) {
    return mTimeoutList[index];
}

void SocketServer::SetTimerValue(unsigned int index, int value) {
    mTimeoutList[index] = value;
}

int SocketServer::ConnectToServer(unsigned int port, std::string ipAddress) {
    if (mIsConnected) 
        return -1;
    
    WSADATA wsData;
    
    int WSOK = WSAStartup(MAKEWORD(2, 2), &wsData);
    
    if (WSOK != 0) 
        return WSOK;
    
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET) 
        return WSACleanup();
    
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.s_addr = inet_addr( ipAddress.c_str() );
    
    if (connect(mSocket, (SOCKADDR*)&hint, sizeof(SOCKADDR)) == SOCKET_ERROR) {
        
        WSACleanup();
        return 0;
    }
    
    mSocketList.push_back(mSocket);
    mPortList.push_back(mLastPort);
    mAddressList.push_back(mLastAddress);
    mBufferList.push_back("");
    mTimeoutList.push_back(CONNECTION_TIMEOUT);
    
    mIsConnected = true;
    return 1;
}

void SocketServer::DisconnectFromServer(void) {
    MessageSend(mSocket, "DISCONN", 7);
    if (mSocket != INVALID_SOCKET) 
        closesocket(mSocket);
    WSACleanup();
    mIsConnected = false;
    return;
}

int SocketServer::InitiateServer(unsigned int port, unsigned int maxConn) {
    if (mIsConnected) 
        return -1;
    
    WSADATA wsData;
    
    int WSOK = WSAStartup(MAKEWORD(2, 2), &wsData);
    
    if (WSOK != 0) 
        return WSOK;
    
    mSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket == INVALID_SOCKET) 
        return WSACleanup();
    
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(port);
    hint.sin_addr.S_un.S_addr = INADDR_ANY;
    
    bind(mSocket, (sockaddr*)&hint, sizeof(hint));
    listen(mSocket, maxConn);
    
    u_long sockMode = 1; // Non blocking listener
    ioctlsocket(mSocket, FIONBIO, &sockMode);
    
    if (mSocket == INVALID_SOCKET) 
        return -1;
    
    time.SetRefreshRate(1);
    
    mIsConnected = true;
    return 1;
}

void SocketServer::ShutdownServer(void) {
    if (mSocket != INVALID_SOCKET) 
        closesocket(mSocket);
    WSACleanup();
    mIsConnected = false;
    return;
}

SOCKET SocketServer::CheckIncomingConnections(void) {
    
    sockaddr_in client;
    int clientSz = sizeof(client);
    
    SOCKET clientSocket = accept(mSocket, (sockaddr*)&client, &clientSz);
    
    if ((clientSocket == WSAEWOULDBLOCK) | (clientSocket == INVALID_SOCKET)) 
        return clientSocket;
    
    char newHost[NI_MAXHOST];
    char newPort[NI_MAXSERV];
    
    ZeroMemory(newHost, NI_MAXHOST);
    ZeroMemory(newPort, NI_MAXSERV);
    
    getnameinfo((sockaddr*)&client, clientSz, newHost, NI_MAXHOST, newPort, NI_MAXSERV, 0);
    
    IPAddress address;
    address.addr[0] = client.sin_addr.S_un.S_un_b.s_b1;
    address.addr[1] = client.sin_addr.S_un.S_un_b.s_b2;
    address.addr[2] = client.sin_addr.S_un.S_un_b.s_b3;
    address.addr[3] = client.sin_addr.S_un.S_un_b.s_b4;
    
    // Accept the client into the connection list
    mLastIndex   = mSocketList.size();
    mLastHost    = newHost;
    mLastPort    = client.sin_port;
    mLastAddress = address;
    
    mSocketList.push_back(clientSocket);
    mPortList.push_back(mLastPort);
    mAddressList.push_back(mLastAddress);
    mBufferList.push_back("");
    mTimeoutList.push_back(CONNECTION_TIMEOUT);
    
#ifdef NETWORK_DEBUG_CONSOLE
    std::cout << "+ conn\n";
#endif
    
    return clientSocket;
}

int SocketServer::CheckIncomingMessages(char* buffer, unsigned int bufferSize) {
    int numberOfBytes = SOCKET_ERROR;
    
    for (unsigned int i=0; i < mSocketList.size(); i++) {
        SOCKET socket = mSocketList[i];
        
        numberOfBytes = MessageReceive(socket, buffer, bufferSize);
        
        if (numberOfBytes < 0) 
            continue;
        
        // Remember the last client to access this connection
        mLastIndex   = i;
        mLastPort    = mPortList[i];
        mLastAddress = mAddressList[i];
        
        // Check explicit disconnection
        if (numberOfBytes == 7) {
            if ((buffer[0] == 'D') & 
                (buffer[1] == 'I') & 
                (buffer[2] == 'S') & 
                (buffer[3] == 'C') & 
                (buffer[4] == 'O') & 
                (buffer[5] == 'N') & 
                (buffer[6] == 'N')) {
                
#ifdef NETWORK_DEBUG_CONSOLE
                std::cout << "- disconn\n";
#endif
                
                DisconnectFromClient(i);
                continue;
            }
        }
        
        // Reset connection time out
        mTimeoutList[i] = CONNECTION_TIMEOUT;
        
        // Assemble the message string
        std::string bufferBuf;
        bufferBuf.reserve(numberOfBytes+1);
        bufferBuf.resize(numberOfBytes);
        
        for (int a=0; a < numberOfBytes; a++) 
            bufferBuf[a] += buffer[a];
        
        mBufferList[i] += bufferBuf;
        
        continue;
    }
    
    return numberOfBytes;
}

int SocketServer::CheckTimers(void) {
    
    // Increment timers
    if (!time.Update()) 
        return 0;
    
    // Check timers
    for (int i=0; i < mSocketList.size(); i++) {
        
        mTimeoutList[i]--;
        
        if (mTimeoutList[i] <= 0) 
            DisconnectFromClient(i);
        
    }
    
    return 0;
}

int SocketServer::DisconnectFromClient(unsigned int index) {
    
    if (index > mSocketList.size()) 
        return -1;
    
    MessageSend(index, "DISCONN", 7);
    
    SOCKET socket = mSocketList[index];
    closesocket(socket);
    
    mSocketList.erase(mSocketList.begin() + index);
    mPortList.erase(mPortList.begin() + index);
    mAddressList.erase(mAddressList.begin() + index);
    mBufferList.erase(mBufferList.begin() + index);
    mTimeoutList.erase(mTimeoutList.begin() + index);
    
    return 1;
}

void SocketServer::MessageSend(SOCKET socket, const char* buffer, unsigned int bufferSize) {
    send(socket, buffer, bufferSize, 0);
}

int SocketServer::MessageReceive(SOCKET socket, char* buffer, unsigned int bufferSize) {
    return recv(socket, buffer, bufferSize, 0);
}

