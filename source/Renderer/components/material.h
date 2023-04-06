

struct Material {
    
    unsigned int textureBuffer;
    
    unsigned int width;
    unsigned int height;
    
    Color color;
    
    Material() {
        
        width  = 0;
        height = 0;
        
        color = Color(1, 1, 1, 1);
        
        glGenTextures(1, &textureBuffer);
        glBindTexture(GL_TEXTURE_2D, textureBuffer);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
    }
    
    ~Material() {
        
        glDeleteTextures(1, &textureBuffer);
        
    }
    
    
    void Bind(void) {glBindTexture(GL_TEXTURE_2D, textureBuffer);}
    
    void BindTextureSlot(unsigned int slot) {glActiveTexture(GL_TEXTURE0 + slot);}
    
    
    void UpdateTexture(void* textureData) {
        
        glBindTexture(GL_TEXTURE_2D, textureBuffer);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
        
    }
};



