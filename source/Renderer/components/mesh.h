
struct Vertex {
    float x, y, z,  r, g, b,  u, v;
    
    Vertex() {
        x=0; y=0; z=0; r=0; g=0; b=0; u=0; v=0;
    }
    
    Vertex(float xx, float yy, float zz, float rr, float gg, float bb, float uu, float vv) {
        x=xx; y=yy; z=zz; r=rr; g=gg; b=bb; u=uu; v=vv;
    }
    
    void operator= (const Vertex& vertex) {
        x = vertex.x;
        y = vertex.y;
        z = vertex.z;
        r = vertex.r;
        g = vertex.g;
        b = vertex.b;
        u = vertex.u;
        v = vertex.v;
        return;
    }
};

struct Index  {
    unsigned int index;
    
    Index(unsigned int value) {index = value;}
};


struct SubMesh {
    
    unsigned vertexBegin;
    unsigned vertexCount;
    unsigned indexBegin;
    unsigned indexCount;
    
    glm::vec3 position;
    
    std::vector<Vertex>  vertexBuffer;
    std::vector<Index>   indexBuffer;
    
};



class Mesh {
    
    unsigned int vertexArray;
    unsigned int bufferVertex;
    unsigned int bufferIndex;
    
    int primitive;
    
    std::vector<Vertex>   vertexBuffer;
    std::vector<Index>    indexBuffer;
    
public:
    
    std::vector<SubMesh> subMesh;
    std::vector<SubMesh> freeMesh;
    
    Shader* shader;
    
    
    Mesh() {
        
        primitive = GL_TRIANGLES;
        
        shader = nullptr;
        
        AllocateBuffers();
    }
    
    ~Mesh() {
        
        FreeBuffers();
    }
    
    
    void SetPrimitive(int primitiveType) {primitive = primitiveType;}
    
    void SetAttribute(int index, int attributeCount, int vertexSize, int byteOffset);
    void DisableAttribute(int index);
    void SetDefaultAttributes(void);
    
    void LoadVertexBuffer(Vertex* BufferData, int VertexCount);
    void LoadIndexBuffer(Index* BufferData, int IndexCount);
    
    
    void Bind(void);
    
    void AllocateBuffers(void);
    void FreeBuffers(void);
    
    void DrawVertexArray(void);
    void DrawIndexArray(void);
    
    unsigned int GetSubMeshCount(void) {return subMesh.size();}
    
    void AddPlane(float x, float y, float z, float width, float height, Color color, unsigned int tess_width, unsigned int tess_height);
    
    
    
    
    
    bool AddSubMesh(float x, float y, float z, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer);
    
    bool RemoveSubMesh(unsigned int index);
    
    bool CopySubMesh(unsigned int index, SubMesh& mesh);
    bool CopySubMesh(unsigned int index, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer);
    
    bool ChangeSubMeshColor(unsigned int index, Color newColor);
    bool ChangeSubMeshPosition(unsigned int index, float x, float y, float z);
    
    void UpdateMesh(void);
    
};

void Mesh::SetAttribute(int index, int attributeCount, int vertexSize, int byteOffset) {
    Bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, attributeCount, GL_FLOAT, GL_FALSE, vertexSize, (void*) byteOffset);
}

void Mesh::DisableAttribute(int index) {
    Bind();
    glDisableVertexAttribArray(index);
}



void Mesh::LoadVertexBuffer(Vertex* BufferData, int VertexCount) {
    Bind();
    glBindBuffer(GL_ARRAY_BUFFER, bufferVertex);
    glBufferData(GL_ARRAY_BUFFER, VertexCount * sizeof(Vertex), &BufferData[0], GL_DYNAMIC_DRAW);
}

void Mesh::LoadIndexBuffer(Index* BufferData, int IndexCount) {
    Bind();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount * sizeof(Index), &BufferData[0], GL_DYNAMIC_DRAW);
}


void Mesh::Bind() {
    glBindVertexArray(vertexArray);
}


void Mesh::AllocateBuffers(void) {
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    glGenBuffers(1, &bufferVertex);
    glBindBuffer(GL_ARRAY_BUFFER, bufferVertex);
    
    glGenBuffers(1, &bufferIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    
    return;
}

void Mesh::FreeBuffers(void) {
    glDeleteBuffers(1, &vertexArray);
    glDeleteBuffers(1, &bufferVertex);
    glDeleteBuffers(1, &bufferIndex);
    return;
}

void Mesh::DrawVertexArray(void) {
    
    glDrawArrays(primitive, 0, vertexBuffer.size());
    
    return;
}

void Mesh::DrawIndexArray(void) {
    
    glDrawElements(primitive, indexBuffer.size(), GL_UNSIGNED_INT, (void*)0);
    
    return;
}

void Mesh::SetDefaultAttributes(void) {
    
    Bind();
    
    SetAttribute(0, 3, sizeof(Vertex), 0);
    SetAttribute(1, 3, sizeof(Vertex), 12);
    SetAttribute(2, 2, sizeof(Vertex), 24);
    
}


void Mesh::AddPlane(float x, float y, float z, float width, float height, Color color, unsigned int tess_width=1, unsigned int tess_height=1) {
    
    Vertex vertex[4];
    vertex[0] = Vertex(x-width, y, z+height,  color.r, color.g, color.b, 0, 1);
    vertex[1] = Vertex(x+width, y, z+height,  color.r, color.g, color.b, 1, 1);
    vertex[2] = Vertex(x+width, y, z-height,  color.r, color.g, color.b, 1, 0);
    vertex[3] = Vertex(x-width, y, z-height,  color.r, color.g, color.b, 0, 0);
    
    unsigned int startVertex = vertexBuffer.size();
    unsigned int startIndex  = indexBuffer.size();
    
    vertexBuffer.push_back(vertex[0]);
    vertexBuffer.push_back(vertex[1]);
    vertexBuffer.push_back(vertex[2]);
    vertexBuffer.push_back(vertex[3]);
    
    indexBuffer.push_back(startVertex+0);
    indexBuffer.push_back(startVertex+1);
    indexBuffer.push_back(startVertex+2);
    indexBuffer.push_back(startVertex+0);
    indexBuffer.push_back(startVertex+2);
    indexBuffer.push_back(startVertex+3);
    
    SubMesh newMesh;
    newMesh.vertexBegin = startVertex;
    newMesh.vertexCount = 4;
    newMesh.indexBegin  = startIndex;
    newMesh.indexCount  = 6;
    newMesh.position    = glm::vec3(x, y, z);
    
    subMesh.push_back(newMesh);
    return;
}


bool Mesh::AddSubMesh(float x, float y, float z, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer) {
    
    if (freeMesh.size() > 0) {
        
        for (std::vector<SubMesh>::iterator it = freeMesh.begin(); it != freeMesh.end(); ++it) {
            SubMesh freeMeshPtr = *it;
            
            if (vrtxBuffer.size() > freeMeshPtr.vertexCount) continue;
            
            std::vector<Vertex> destMesh;
            std::vector<Index>  destIndex;
            
            freeMesh.erase(it);
            
            subMesh.push_back(freeMeshPtr);
            freeMeshPtr.position = glm::vec3(x, y, z);
            
            unsigned int i = 0;
            for (std::vector<Vertex>::iterator it = vertexBuffer.begin() + freeMeshPtr.vertexBegin; it != vertexBuffer.begin() + freeMeshPtr.vertexBegin + freeMeshPtr.vertexCount; ++it) {
                Vertex& vertex = *it;
                vertex.x = vrtxBuffer[i].x + x;
                vertex.y = vrtxBuffer[i].y + y;
                vertex.z = vrtxBuffer[i].z + z;
                vertex.r = vrtxBuffer[i].r;
                vertex.g = vrtxBuffer[i].g;
                vertex.b = vrtxBuffer[i].b;
                vertex.u = vrtxBuffer[i].u;
                vertex.v = vrtxBuffer[i].v;
                destMesh.push_back(vertex);
                i++;
            }
            
            i = 0;
            for (std::vector<Index>::iterator it = indexBuffer.begin() + freeMeshPtr.indexBegin; it != indexBuffer.begin() + freeMeshPtr.indexBegin + freeMeshPtr.indexCount; ++it) {
                Index& index = *it;
                index.index = indxBuffer[i].index + freeMeshPtr.vertexBegin;
                destIndex.push_back(index);
                i++;
            }
            
            Bind();
            glBufferSubData(GL_ARRAY_BUFFER, freeMeshPtr.vertexBegin * sizeof(Vertex), freeMeshPtr.vertexCount * sizeof(Vertex), &destMesh[0]);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, freeMeshPtr.indexBegin * sizeof(Index), freeMeshPtr.indexCount * sizeof(Index), &destIndex[0]);
            
            return 0;
        }
    }
    
    unsigned int startVertex = vertexBuffer.size();
    unsigned int startIndex  = indexBuffer.size();
    
    SubMesh newMesh;
    newMesh.vertexBegin = startVertex;
    newMesh.vertexCount = vrtxBuffer.size();
    newMesh.indexBegin  = startIndex;
    newMesh.indexCount  = indxBuffer.size();
    newMesh.position    = glm::vec3(x, y, z);
    
    subMesh.push_back(newMesh);
    
    for (std::vector<Vertex>::iterator it = vrtxBuffer.begin(); it != vrtxBuffer.end(); ++it) {
        Vertex vertex = *it;
        vertex.x += x;
        vertex.y += y;
        vertex.z += z;
        vertexBuffer.push_back(vertex);
    }
    
    for (std::vector<Index>::iterator it = indxBuffer.begin(); it != indxBuffer.end(); ++it) {
        Index index = *it;
        index.index += startVertex;
        
        indexBuffer.push_back(index);
    }
    
    UpdateMesh();
    return 1;
}

bool Mesh::RemoveSubMesh(unsigned int index) {
    if (subMesh.size() == 0) return false;
    if (index > subMesh.size()) return false;
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = subMesh[index];
    
    for (std::vector<Vertex>::iterator it = vertexBuffer.begin() + sourceMesh.vertexBegin; it != vertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) {
        Vertex vertex;
        *it = vertex;
        destMesh.push_back(vertex);
    }
    
    freeMesh.push_back(sourceMesh);
    
    subMesh.erase(subMesh.begin() + index);
    
    Bind();
    glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}

bool Mesh::CopySubMesh(unsigned int index, SubMesh& mesh) {
    if (index >= subMesh.size()) return false;
    
    SubMesh sourceMesh = subMesh[index];
    
    for (std::vector<Vertex>::iterator it = vertexBuffer.begin() + sourceMesh.vertexBegin; it != vertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) 
        mesh.vertexBuffer.push_back(*it);
    
    for (std::vector<Index>::iterator it = indexBuffer.begin() + sourceMesh.indexBegin; it != indexBuffer.begin() + sourceMesh.indexBegin + sourceMesh.indexCount; ++it) 
        mesh.indexBuffer.push_back(*it);
    
    return true;
}

bool Mesh::CopySubMesh(unsigned int index, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer) {
    if (index >= subMesh.size()) return false;
    
    SubMesh sourceMesh = subMesh[index];
    
    for (std::vector<Vertex>::iterator it = vertexBuffer.begin() + sourceMesh.vertexBegin; it != vertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) 
        vrtxBuffer.push_back(*it);
    
    for (std::vector<Index>::iterator it = indexBuffer.begin() + sourceMesh.indexBegin; it != indexBuffer.begin() + sourceMesh.indexBegin + sourceMesh.indexCount; ++it) 
        indxBuffer.push_back(*it);
    
    return true;
}


bool Mesh::ChangeSubMeshPosition(unsigned int index, float x, float y, float z) {
    if (index >= subMesh.size()) return false;
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = subMesh[index];
    
    for (std::vector<Vertex>::iterator it = vertexBuffer.begin() + sourceMesh.vertexBegin; it != vertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) {
        Vertex& vertex = *it;
        vertex.x -= sourceMesh.position.x;
        vertex.y -= sourceMesh.position.y;
        vertex.z -= sourceMesh.position.z;
        vertex.x += x;
        vertex.y += y;
        vertex.z += z;
        destMesh.push_back(vertex);
    }
    
    subMesh[index].position = glm::vec3(x, y, z);
    
    Bind();
    glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}


bool Mesh::ChangeSubMeshColor(unsigned int index, Color newColor) {
    if (index >= subMesh.size()) return false;
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = subMesh[index];
    
    for (std::vector<Vertex>::iterator it = vertexBuffer.begin() + sourceMesh.vertexBegin; it != vertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) {
        Vertex& vertex = *it;
        vertex.r = newColor.r;
        vertex.g = newColor.g;
        vertex.b = newColor.b;
        destMesh.push_back(vertex);
    }
    
    Bind();
    glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}


void Mesh::UpdateMesh(void) {
    
    LoadVertexBuffer(vertexBuffer.data(), vertexBuffer.size());
    LoadIndexBuffer(indexBuffer.data(), indexBuffer.size());
    
}






