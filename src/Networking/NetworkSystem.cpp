#include <GameEngineFramework/Networking/NetworkSystem.h>
#include <GameEngineFramework/Logging/Logging.h>

extern NetworkSystem Network;
extern Logger Log;

// Actor system thread
bool isNetworkThreadActive = true;
void networkThreadMain(void);


NetworkSystem::NetworkSystem() : 
    mIsHosting(false),
    mIsConnected(false)
{
    mBuffer.resize(1024);
    return;
}

void NetworkSystem::Initiate(void) {
    
    networkSystemThread = new std::thread( networkThreadMain );
    
    Log.Write( " >> Starting thread networking" );
    
    return;
}

void NetworkSystem::Shutdown(void) {
    
    if (mIsHosting) {
        
        // Disconnect from clients
        for (unsigned int i=0; i < mHost.GetNumberOfConnections(); i++) 
            mHost.DisconnectFromClient(i);
        
    } else {
        
        mHost.DisconnectFromServer();
        
    }
    
    // Shutdown the thread
    isNetworkThreadActive = false;
    
    networkSystemThread->join();
    
    return;
}

bool NetworkSystem::ConnectToServer(std::string address, unsigned int port) {
    if (mHost.ConnectToServer(address, port)) {
        mIsHosting = false;
        mIsConnected = true;
        return true;
    }
    
    return false;
}

bool NetworkSystem::DisconnectFromServer(void) {
    if (mHost.DisconnectFromServer()) {
        mIsHosting = false;
        mIsConnected = false;
        return true;
    }
    return false;
}

void NetworkSystem::SendMessageToServer(char* buffer, unsigned int size) {
    send(mHost.mSocket, buffer, size, 0);
    return;
}

int NetworkSystem::GetMessageFromServer(char* buffer, unsigned int size) {
    return recv(mHost.mSocket, buffer, size, EWOULDBLOCK);
}

bool NetworkSystem::StartServer(unsigned int port) {
    if (mHost.InitiateServer(port)) {
        mIsHosting = true;
        mIsConnected = true;
        return true;
    }
    return false;
}

bool NetworkSystem::StopServer(void) {
    if (mHost.ShutdownServer()) {
        mIsHosting = false;
        mIsConnected = false;
        return true;
    }
    return false;
}

bool NetworkSystem::Send(int index, char* message, unsigned int messageSz) {
    SOCKET socket = GetSocketByIndex(index);
    send(socket, message, messageSz, 0);
    return true;
}

bool NetworkSystem::GetConnectionStatus(void) {
    return mIsConnected;
}

bool NetworkSystem::GetServerStatus(void) {
    return mIsHosting;
}

unsigned int NetworkSystem::GetNumberOfConnections(void) {
    mux.lock();
    unsigned int number = mHost.GetNumberOfConnections();
    mux.unlock();
    return number;
}

unsigned int NetworkSystem::GetPortByIndex(unsigned int index) {
    mux.lock();
    unsigned int port = mHost.GetPortByIndex(index);
    mux.unlock();
    return port;
}

SOCKET NetworkSystem::GetSocketByIndex(unsigned int index) {
    mux.lock();
    SOCKET socket = mHost.GetSocketByIndex(index);
    mux.unlock();
    return socket;
}

std::string NetworkSystem::GetClientBufferByIndex(unsigned int index) {
    mux.lock();
    std::string bufferString = mHost.GetBufferStringByIndex(index);
    mux.unlock();
    return bufferString;
}

void NetworkSystem::ClearClientBufferByIndex(unsigned int index) {
    mux.lock();
    mHost.ClearBufferStringByIndex(index);
    mux.unlock();
    return;
}

void NetworkSystem::Update(void) {
    
    if (!mIsConnected) 
        return;
    
    mux.lock();
    
    if (mIsHosting) {
        
        mHost.CheckIncomingConnections();
        
        mHost.CheckIncomingMessages( (char*)mBuffer.c_str(), mBuffer.size() );
        
        // Timeout old connections
        mHost.CheckTimers();
        
    }
    
    mux.unlock();
    
    return;
}



//
// Networking thread
//

void networkThreadMain() {
    
    while (isNetworkThreadActive) {
        std::this_thread::sleep_for( std::chrono::duration<float, std::micro>(1) );
        
        //Network.Update();
        
        std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(4) );
        
        continue;
    }
    
    std::this_thread::sleep_for( std::chrono::duration<float, std::milli>(4) );
    Log.Write( " >> Shutting down on thread networking" );
    
    return;
}


