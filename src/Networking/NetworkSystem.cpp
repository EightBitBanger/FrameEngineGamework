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
    
    // Kill current connections
    if (mIsHosting) {
        
        // Disconnect from clients
        for (unsigned int i=0; i < mHost.GetNumberOfConnections(); i++) {
            
            mHost.DisconnectFromClient(i);
            
        }
        
    } else {
        
        mHost.DisconnectFromServer();
        
    }
    
    isNetworkThreadActive = false;
    
    networkSystemThread->join();
    
    return;
}

bool NetworkSystem::ConnectToServer(std::string address, unsigned int port) {
    return mHost.ConnectToServer(port, address);
}

bool NetworkSystem::DisconnectFromServer(void) {
    mHost.DisconnectFromServer();
    return true;
}

void NetworkSystem::SendMessageToServer(char* buffer, unsigned int size) {
    send(mHost.mSocket, buffer, size, 0);
    return;
}

int NetworkSystem::GetMessageFromServer(char* buffer, unsigned int size) {
    return recv(mHost.mSocket, buffer, size, 0);
}

bool NetworkSystem::StartServer(unsigned int port) {
    if (!mIsHosting) {
        if (mHost.InitiateServer(port) == 1) {
            mIsHosting = true;
            mIsConnected = true;
            return true;
        }
    }
    return false;
}

bool NetworkSystem::StopServer(void) {
    if (!mIsHosting) 
        return false;
    mHost.ShutdownServer();
    mIsHosting = false;
    mIsConnected = false;
    return true;
}

bool NetworkSystem::Send(int index, char* message, unsigned int size) {
    SOCKET socket = GetSocketByIndex(index);
    send(mHost.mSocket, message, size, 0);
    return true;
}

unsigned int NetworkSystem::GetLastPort(void) {
    mux.lock();
    unsigned int port = mHost.GetLastPort();
    mux.unlock();
    return port;
}

IPAddress NetworkSystem::GetLastAddress(void) {
    mux.lock();
    IPAddress address = mHost.GetLastAddress();
    mux.unlock();
    return address;
}

unsigned int NetworkSystem::GetLastIndex(void) {
    mux.lock();
    unsigned int index = mHost.GetLastIndex();
    mux.unlock();
    return index;
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
        
        mHost.CheckIncomingMessages( (char*)mBuffer.c_str(), mBuffer.size() );
        
        mHost.CheckIncomingConnections();
        
    }
    
    // Check timeout on old connections
    mHost.CheckTimers();
    
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


