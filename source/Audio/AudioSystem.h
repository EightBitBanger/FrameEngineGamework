#ifndef AUDIO_RENDER_SYSTEM
#define AUDIO_RENDER_SYSTEM

// Audio render support by openAL maybe
//#include "openAL/al/al.h"
//#include "openAL/al/alc.h"


class __declspec(dllexport) AudioSystem {
    
public:
    
    void Initiate(void);
    void Shutdown(void);
    
    
private:
    
    //ALCdevice*  mDevice  = NULL;
    //ALCcontext* mContext = alcGetCurrentContext();
    
};

#endif
