#ifndef RESOURCE_TEXTURE_TAG
#define RESOURCE_TEXTURE_TAG

#include "../../Renderer/RenderSystem.h"

struct __declspec(dllexport) TextureTag {
    
    /// Texture resource name.
    std::string name;
    
    /// Width of the image in pixels.
    int width;
    /// Height of the image in pixels.
    int height;
    /// Color channels used by the image.
    int channels;
    
    /// Pointer to the data held by the image loading program.
    unsigned char* buffer;
    
    TextureTag();
    
};

#endif
