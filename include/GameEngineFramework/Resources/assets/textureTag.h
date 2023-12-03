#ifndef RESOURCE_TEXTURE_TAG
#define RESOURCE_TEXTURE_TAG

#include "../../Renderer/RenderSystem.h"

struct __declspec(dllexport) TextureTag {
    
    /// Texture resource path.
    std::string path;
    
    /// Texture resource name.
    std::string name;
    
    /// Is the resource loaded into memory.
    bool isLoaded;
    
    /// Width of the image in pixels.
    int width;
    /// Height of the image in pixels.
    int height;
    /// Color channels used by the image.
    int channels;
    
    /// Pointer to the data held by the image loading program.
    unsigned char* buffer;
    
    /// Load the data to which this asset points.
    bool Load(void);
    
    /// Frees the memory associated with this texture.
    bool Unload(void);
    
    TextureTag();
    
};

#endif
