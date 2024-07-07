#ifndef __COMPONENT_TEXTURE
#define __COMPONENT_TEXTURE

#include <GameEngineFramework/Engine/types/color.h>

#include <GameEngineFramework/Renderer/enumerators.h>


class ENGINE_API Texture {
    
public:
    
    /// Get image width in pixels.
    unsigned int GetImageWidth(void);
    
    /// Get image height in pixels.
    unsigned int GetImageHeight(void);
    
    /// Get the texture interpolation filter type.
    unsigned int GetTextureInterpolationType(void);
    
    /// Bind the material texture for rendering.
    void Bind(void);
    
    /// Bind the texture slot for textured rendering.
    void BindTextureSlot(unsigned int slot);
    
    /// Upload the texture buffer onto the GPU.
    void UploadTextureToGPU(void* textureData, unsigned int width, unsigned int height, int filtrationType);
    
    
    friend class RenderSystem;
    
    Texture();
    ~Texture();
    
    
private:
    
    unsigned int mTextureBuffer;
    
    unsigned int mWidth;
    
    unsigned int mHeight;
    
    int mFiltration;
    
};



#endif
