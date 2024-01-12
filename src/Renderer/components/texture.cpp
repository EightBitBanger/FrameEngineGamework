#include <GameEngineFramework/Renderer/components/texture.h>

#define GLEW_STATIC
#include <gl/glew.h>


Texture::Texture() : 
    mWidth(0),
    mHeight(0),
    mFiltration(GL_LINEAR_MIPMAP_LINEAR)
{
    glGenTextures(1, &mTextureBuffer);
    
    return;
}

Texture::~Texture() {
    glDeleteTextures(1, &mTextureBuffer);
    return;
}

unsigned int Texture::GetImageWidth(void) {
    return mWidth;
}

unsigned int Texture::GetImageHeight(void) {
    return mHeight;
}

unsigned int Texture::GetTextureInterpolationType(void) {
    return mFiltration;
}

void Texture::Bind(void) {
    glBindTexture(GL_TEXTURE_2D, mTextureBuffer);
    return;
}

void Texture::BindTextureSlot(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    return;
}

void Texture::UploadTextureToGPU(void* textureData, unsigned int width, unsigned int height, int filtrationType) {
    
    glBindTexture(GL_TEXTURE_2D, mTextureBuffer);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtrationType);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    mWidth      = width;
    mHeight     = height;
    mFiltration = filtrationType;
    
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return;
}

