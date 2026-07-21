#ifndef __COMPONENT_FRAME_BUFFER
#define __COMPONENT_FRAME_BUFFER

#include <GameEngineFramework/configuration.h>
#include <GameEngineFramework/Renderer/components/texture.h>


class ENGINE_API FrameBuffer {
    
public:
    
    Texture texture;
    
    FrameBuffer();
    
    ~FrameBuffer();
    
    void Bind(void);
    
private:
    
    unsigned int mFrameBuffer;
    
    
};


#endif
