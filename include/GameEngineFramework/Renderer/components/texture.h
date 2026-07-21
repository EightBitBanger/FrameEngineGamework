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
    
    /// Return a handle to the openGL buffer.
    void* GetHandle(void);
    
    /// Set the texture UV scale.
    void SetScaleUV(const glm::vec2& scale);
    
    /// Set the texture position offset.
    void SetPositionUV(const glm::vec2& position);
    
    /// Bind the material texture for rendering.
    void Bind(void);
    
    /// Bind the texture slot for textured rendering.
    void BindTextureSlot(unsigned int slot);
    
    /// Upload the texture buffer onto the GPU.
    void UploadTextureToGPU(void* textureData, unsigned int width, unsigned int height, unsigned int channels, int filterMin, int filterMag);
    
    friend class RenderSystem;
    
    Texture();
    ~Texture();
    
private:
    
    unsigned int mTextureBuffer;
    
    unsigned int mWidth;
    unsigned int mHeight;
    unsigned int mChannels;
    
    glm::vec2 mScale;
    glm::vec2 mPosition;
    
    int mFiltration;
};



#endif
