
struct TextureTag {
    
    std::string name;
    
    int width;
    int height;
    int channels;
    
    unsigned char* buffer;
    
    TextureTag() {
        
        width = 0;
        height = 0;
        channels = 0;
        
        buffer = nullptr;
        
    }
    
    void UnloadBuffers(void) {
        
        stbi_image_free(buffer);
        
    }
    
};

