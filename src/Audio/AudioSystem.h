#ifndef AUDIO_RENDER_SYSTEM
#define AUDIO_RENDER_SYSTEM

#include <thread>
#include <mutex>
#include <chrono>

class __declspec(dllexport) AudioSystem {
    
public:
    
    void Initiate(void);
    void Shutdown(void);
    
    
private:
    
    std::thread* audioThread;
    
};

#endif
