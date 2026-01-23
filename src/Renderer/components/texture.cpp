#include <GameEngineFramework/Renderer/components/texture.h>

#define GLEW_STATIC
#include <gl/glew.h>


Texture::Texture() : 
    mWidth(0),
    mHeight(0),
    mChannels(0),
    mFiltration(GL_LINEAR_MIPMAP_LINEAR)
{
    glGenTextures(1, &mTextureBuffer);
}

Texture::~Texture() {
    glDeleteTextures(1, &mTextureBuffer);
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
}

void Texture::BindTextureSlot(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
}

void* Texture::GetHandle(void) {
    return (void*)(intptr_t)mTextureBuffer;
}

void Texture::UploadTextureToGPU(void* textureData, unsigned int width, unsigned int height, unsigned int channels, int filterMin, int filterMag) {
    glBindTexture(GL_TEXTURE_2D, mTextureBuffer);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
    
    mWidth      = width;
    mHeight     = height;
    mFiltration = filterMin;
    mChannels   = channels;
    
    glGenerateMipmap(GL_TEXTURE_2D);
}

