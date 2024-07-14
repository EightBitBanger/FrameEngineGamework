#ifndef AUDIO_RENDER_SYSTEM
#define AUDIO_RENDER_SYSTEM

#include <GameEngineFramework/configuration.h>

#include <thread>
#include <mutex>
#include <chrono>


class ENGINE_API AudioSystem {
    
public:
    
    void Initiate(void);
    void Shutdown(void);
    
    
private:
    
    std::thread* audioThread;
    
};

#endif
