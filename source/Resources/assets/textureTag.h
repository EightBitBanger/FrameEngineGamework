#ifndef RESOURCE_TEXTURE_TAG
#define RESOURCE_TEXTURE_TAG

#include "../../Renderer/RenderSystem.h"

struct TextureTag {
    
    std::string name;
    
    int width;
    int height;
    int channels;
    
    unsigned char* buffer;
    
    TextureTag();
    
};

#endif
