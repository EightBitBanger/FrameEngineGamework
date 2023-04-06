
struct MeshTag {
    
    std::string name;
    
    std::vector<Vertex> vertexBuffer;
    std::vector<Index>  indexBuffer;
    
    
    
    void UnloadBuffers(void) {
        vertexBuffer.clear();
        indexBuffer.clear();
    }
    
};

