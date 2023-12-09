#include <GameEngineFramework/Networking/NetworkSystem.h>


NetworkSystem::NetworkSystem() : 
    mIsHosting(false)
{
    mBuffer.resize(4096);
    return;
}

void NetworkSystem::Initiate(void) {
    
    return;
}

void NetworkSystem::Shutdown(void) {
    
    return;
}

void NetworkSystem::Update(void) {
    
    // Update server
    if (mIsHosting) {
        
        // Check timeout on old connections
        mHost.CheckTimers();
        
        // Check new connections
        mHost.CheckIncomingConnections();
        
        // Check client messages
        mHost.CheckIncomingMessages( (char*)mBuffer.c_str(), mBuffer.size() );
        
    } else {
        
        // Update client
        
        
        
    }
    
    return;
}

bool NetworkSystem::ConnectToHost(IPAddress address, unsigned int port) {
    
    return true;
}

bool NetworkSystem::DisconnectFromHost(void) {
    
    return true;
}

bool NetworkSystem::StartServer(unsigned int port) {
    if (!mIsHosting) {
        if (mHost.InitiateServer(port) == 1) {
            mIsHosting = true;
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
    return true;
}

unsigned int NetworkSystem::GetLastPort(void) {
    return mHost.GetLastPort();
}

std::string NetworkSystem::GetLastHost(void) {
    return mHost.GetLastHost();
}

IPAddress NetworkSystem::GetLastAddress(void) {
    return mHost.GetLastAddress();
}

unsigned int NetworkSystem::GetLastIndex(void) {
    return mHost.GetLastIndex();
}

unsigned int NetworkSystem::GetNumberOfConnections(void) {
    return mHost.GetNumberOfConnections();
}

std::string NetworkSystem::GetHostByIndex(unsigned int index) {
    return mHost.GetHostByIndex(index);
}

int NetworkSystem::FindHost(std::string name) {
    return mHost.FindHost(name);
}

unsigned int NetworkSystem::GetPortByIndex(unsigned int index){
    return mHost.GetPortByIndex(index);
}

SOCKET NetworkSystem::GetSocketByIndex(unsigned int index) {
    return mHost.GetSocketByIndex(index);
}

std::string NetworkSystem::GetBufferStringByIndex(unsigned int index) {
    return mHost.GetBufferStringByIndex(index);
}

void NetworkSystem::ClearBufferStringByIndex(unsigned int index) {
    return mHost.ClearBufferStringByIndex(index);
}
