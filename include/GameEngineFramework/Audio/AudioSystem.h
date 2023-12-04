#ifndef AUDIO_RENDER_SYSTEM
#define AUDIO_RENDER_SYSTEM

#include <thread>
#include <mutex>
#include <chrono>

#include <GameEngineFramework/configuration.h>


class ENGINE_API AudioSystem {
    
public:
    
    void Initiate(void);
    void Shutdown(void);
    
    
private:
    
    std::thread* audioThread;
    
};

#endif
