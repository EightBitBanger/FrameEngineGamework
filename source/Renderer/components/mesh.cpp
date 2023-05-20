#include "mesh.h"


void Mesh::Reallocate(unsigned int newBufferSize) {
    FreeBuffers();
    AllocateBuffers(newBufferSize);
    
    SetDefaultAttributes();
    return;
}

Mesh::Mesh() {
    
    mPrimitive = GL_TRIANGLES;
    mVertexBufferSz = 0;
    mIndexBufferSz = 0;
    
    AllocateBuffers(32);
    
    SetDefaultAttributes();
    return;
}

Mesh::~Mesh() {
    
    FreeBuffers();
    return;
}

void Mesh::SetPrimitive(int primitiveType) {
    mPrimitive = primitiveType;
    return;
}

void Mesh::SetAttribute(int index, int attributeCount, int vertexSize, int byteOffset) {
    Bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, attributeCount, GL_FLOAT, GL_FALSE, vertexSize, (void*) byteOffset);
    return;
}

void Mesh::DisableAttribute(int index) {
    Bind();
    glDisableVertexAttribArray(index);
    return;
}

void Mesh::LoadVertexBuffer(Vertex* bufferData, int vertexCount) {
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferVertex);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), &bufferData[0], GL_DYNAMIC_DRAW);
    return;
}

void Mesh::LoadIndexBuffer(Index* bufferData, int indexCount) {
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), &bufferData[0], GL_DYNAMIC_DRAW);
    return;
}

void Mesh::Bind() {
    glBindVertexArray(mVertexArray);
    return;
}

void Mesh::AllocateBuffers(unsigned int maxBufferSize) {
    mMaxSize = maxBufferSize;
    
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);
    
    glGenBuffers(1, &mBufferVertex);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferVertex);
    glBufferData(GL_ARRAY_BUFFER, mMaxSize * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
    
    glGenBuffers(1, &mBufferIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mMaxSize * sizeof(Index), NULL, GL_DYNAMIC_DRAW);
    
    return;
}

void Mesh::FreeBuffers(void) {
    glDeleteBuffers(1, &mVertexArray);
    glDeleteBuffers(1, &mBufferVertex);
    glDeleteBuffers(1, &mBufferIndex);
    return;
}

void Mesh::DrawVertexArray(void) {
    
    glDrawArrays(mPrimitive, 0, mVertexBufferSz);
    
    return;
}

void Mesh::DrawIndexArray(void) {
    
    glDrawElements(mPrimitive, mIndexBufferSz, GL_UNSIGNED_INT, (void*)0);
    
    return;
}

unsigned int Mesh::GetSubMeshCount(void) {
    return mSubMesh.size();
}

void Mesh::SetDefaultAttributes(void) {
    
    Bind();
    
    SetAttribute(0, 3, sizeof(Vertex), 0);
    SetAttribute(1, 3, sizeof(Vertex), 12);
    SetAttribute(2, 3, sizeof(Vertex), 24);
    SetAttribute(3, 2, sizeof(Vertex), 36);
    SetAttribute(4, 3, sizeof(Vertex), 44);
    
    return;
}

void Mesh::AddPlain(float x, float y, float z, float width, float height, Color color) {
    
    // Align the mesh
    z = (z * 2) - x;
    
    Vertex vertex[4];
    vertex[0] = Vertex( x-width, y, z+height,  color.r, color.g, color.b, 0, 1 );
    vertex[1] = Vertex( x+width, y, z+height,  color.r, color.g, color.b, 1, 1 );
    vertex[2] = Vertex( x+width, y, z-height,  color.r, color.g, color.b, 1, 0 );
    vertex[3] = Vertex( x-width, y, z-height,  color.r, color.g, color.b, 0, 0 );
    
    SubMesh subBuffer;
    subBuffer.vertexBuffer.push_back(vertex[0]);
    subBuffer.vertexBuffer.push_back(vertex[1]);
    subBuffer.vertexBuffer.push_back(vertex[2]);
    subBuffer.vertexBuffer.push_back(vertex[3]);
    
    subBuffer.indexBuffer.push_back(0);
    subBuffer.indexBuffer.push_back(1);
    subBuffer.indexBuffer.push_back(2);
    subBuffer.indexBuffer.push_back(0);
    subBuffer.indexBuffer.push_back(2);
    subBuffer.indexBuffer.push_back(3);
    
    AddSubMesh(x, y, x, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
    return;
}

void Mesh::AddPlainSubDivided(float x, float y, float z, float width, float height, Color color, unsigned int widthSub, unsigned int heightSub) {
    for (unsigned int h=0; h < heightSub; h++) {
        for (unsigned int w=0; w < widthSub; w++) {
            
            AddPlain( x + (w * width), y, z + (h * height), width, height, color);
        }
    }
    UpdateMesh();
    return;
}

void Mesh::AddWall(float x, float y, float z, float width, float height, Color color) {
    
    // Align the mesh
    z -= x;
    
    Vertex vertex[4];
    vertex[0] = Vertex( x-width, y+height, z,  color.r, color.g, color.b, 0, 1 );
    vertex[1] = Vertex( x+width, y+height, z,  color.r, color.g, color.b, 1, 1 );
    vertex[2] = Vertex( x+width, y-height, z,  color.r, color.g, color.b, 1, 0 );
    vertex[3] = Vertex( x-width, y-height, z,  color.r, color.g, color.b, 0, 0 );
    
    SubMesh subBuffer;
    subBuffer.vertexBuffer.push_back(vertex[0]);
    subBuffer.vertexBuffer.push_back(vertex[1]);
    subBuffer.vertexBuffer.push_back(vertex[2]);
    subBuffer.vertexBuffer.push_back(vertex[3]);
    
    subBuffer.indexBuffer.push_back(0);
    subBuffer.indexBuffer.push_back(1);
    subBuffer.indexBuffer.push_back(2);
    subBuffer.indexBuffer.push_back(0);
    subBuffer.indexBuffer.push_back(2);
    subBuffer.indexBuffer.push_back(3);
    
    AddSubMesh(x, y, x, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
    return;
}

void Mesh::AddWallSubDivided(float x, float y, float z, float width, float height, Color color, unsigned int widthSub, unsigned int heightSub) {
    for (unsigned int h=0; h < heightSub; h++) {
        for (unsigned int w=0; w < widthSub; w++) {
            
            AddWall( x + (w * width), y + (h * height), z, width, height, color);
        }
    }
    UpdateMesh();
    return;
}

bool Mesh::AddSubMesh(float x, float y, float z, SubMesh& mesh, bool doUploadToGpu) {
    return AddSubMesh(x, y, z, mesh.vertexBuffer, mesh.indexBuffer, doUploadToGpu);
}

bool Mesh::AddSubMesh(float x, float y, float z, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer, bool doUploadToGpu) {
    
    if (mFreeMesh.size() > 0) {
        
        for (std::vector<SubMesh>::iterator it = mFreeMesh.begin(); it != mFreeMesh.end(); ++it) {
            SubMesh freeMeshPtr = *it;
            
            if (vrtxBuffer.size() != freeMeshPtr.vertexCount) continue;
            
            mFreeMesh.erase(it);
            
            freeMeshPtr.position = glm::vec3(x, y, z);
            
            mSubMesh.push_back(freeMeshPtr);
            
            unsigned int i = 0;
            for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + freeMeshPtr.vertexBegin; it != mVertexBuffer.begin() + freeMeshPtr.vertexBegin + freeMeshPtr.vertexCount; ++it) {
                Vertex& vertex = *it;
                vertex.x = vrtxBuffer[i].x + x;
                vertex.y = vrtxBuffer[i].y + y;
                vertex.z = vrtxBuffer[i].z + z;
                vertex.r = vrtxBuffer[i].r;
                vertex.g = vrtxBuffer[i].g;
                vertex.b = vrtxBuffer[i].b;
                vertex.u = vrtxBuffer[i].u;
                vertex.v = vrtxBuffer[i].v;
                i++;
            }
            
            i = 0;
            for (std::vector<Index>::iterator it = mIndexBuffer.begin() + freeMeshPtr.indexBegin; it != mIndexBuffer.begin() + freeMeshPtr.indexBegin + freeMeshPtr.indexCount; ++it) {
                Index& index = *it;
                index.index = indxBuffer[i].index + freeMeshPtr.vertexBegin;
                i++;
            }
            
            if (doUploadToGpu) {
                glBindBuffer(GL_ARRAY_BUFFER, mBufferVertex);
                glBufferSubData(GL_ARRAY_BUFFER, freeMeshPtr.vertexBegin * sizeof(Vertex), freeMeshPtr.vertexCount * sizeof(Vertex), &mVertexBuffer[freeMeshPtr.vertexBegin]);
                
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferIndex);
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, freeMeshPtr.indexBegin * sizeof(Index),  freeMeshPtr.indexCount * sizeof(Index), &mIndexBuffer[freeMeshPtr.indexBegin]);
            }
            
            return false;
        }
    }
    
    unsigned int startVertex = mVertexBuffer.size();
    unsigned int startIndex  = mIndexBuffer.size();
    
    SubMesh newMesh;
    newMesh.vertexBegin = startVertex;
    newMesh.vertexCount = vrtxBuffer.size();
    newMesh.indexBegin  = startIndex;
    newMesh.indexCount  = indxBuffer.size();
    newMesh.position    = glm::vec3(x, y, z);
    
    mSubMesh.push_back(newMesh);
    
    for (std::vector<Vertex>::iterator it = vrtxBuffer.begin(); it != vrtxBuffer.end(); ++it) {
        Vertex vertex = *it;
        vertex.x += x;
        vertex.y += y;
        vertex.z += z;
        mVertexBuffer.push_back(vertex);
    }
    
    for (std::vector<Index>::iterator it = indxBuffer.begin(); it != indxBuffer.end(); ++it) {
        Index index = *it;
        index.index += startVertex;
        
        mIndexBuffer.push_back(index);
    }
    
    
    mVertexBufferSz = mVertexBuffer.size();
    mIndexBufferSz  = mIndexBuffer.size();
    
    if (!doUploadToGpu) 
        return true;
    
    // Check to allocate new GPU memory
    if ((mVertexBufferSz > mMaxSize) | (mIndexBufferSz > mMaxSize)) {
        if (mVertexBufferSz > mIndexBufferSz) 
        {mMaxSize = mVertexBufferSz;} else {mMaxSize = mIndexBufferSz;}
        
        glBufferData(GL_ARRAY_BUFFER, mVertexBufferSz * sizeof(Vertex) + (vrtxBuffer.size() * sizeof(Vertex)), NULL, GL_DYNAMIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferSz * sizeof(Index) + (indxBuffer.size() * sizeof(Vertex)), NULL, GL_DYNAMIC_DRAW);
        
        glBufferSubData(GL_ARRAY_BUFFER, 0, mVertexBufferSz * sizeof(Vertex), &mVertexBuffer[0]);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, mIndexBufferSz * sizeof(Index), &mIndexBuffer[0]);
        
        return true;
    }
    
    glBindVertexArray(mVertexArray);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, mVertexBufferSz * sizeof(Vertex), &mVertexBuffer[0]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, mIndexBufferSz * sizeof(Index), &mIndexBuffer[0]);
    
    return true;
}

bool Mesh::RemoveSubMesh(unsigned int index) {
    if (mSubMesh.size() == 0) return false;
    if (index > mSubMesh.size()) return false;
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) {
        Vertex vertex;
        *it = vertex;
        destMesh.push_back(vertex);
    }
    
    mFreeMesh.push_back(sourceMesh);
    
    mSubMesh.erase(mSubMesh.begin() + index);
    
    glBindVertexArray(mVertexArray);
    glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}

bool Mesh::CopySubMesh(unsigned int index, SubMesh& mesh) {
    if (index >= mSubMesh.size()) return false;
    
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) 
        mesh.vertexBuffer.push_back(*it);
    
    for (std::vector<Index>::iterator it = mIndexBuffer.begin() + sourceMesh.indexBegin; it != mIndexBuffer.begin() + sourceMesh.indexBegin + sourceMesh.indexCount; ++it) 
        mesh.indexBuffer.push_back(*it);
    
    return true;
}

bool Mesh::CopySubMesh(unsigned int index, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer) {
    if (index >= mSubMesh.size()) return false;
    
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) 
        vrtxBuffer.push_back(*it);
    
    for (std::vector<Index>::iterator it = mIndexBuffer.begin() + sourceMesh.indexBegin; it != mIndexBuffer.begin() + sourceMesh.indexBegin + sourceMesh.indexCount; ++it) 
        indxBuffer.push_back(*it);
    
    return true;
}

bool Mesh::ChangeSubMeshPosition(unsigned int index, float x, float y, float z) {
    if (index >= mSubMesh.size()) return false;
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) {
        Vertex& vertex = *it;
        vertex.x -= sourceMesh.position.x;
        vertex.y -= sourceMesh.position.y;
        vertex.z -= sourceMesh.position.z;
        vertex.x += x;
        vertex.y += y;
        vertex.z += z;
        destMesh.push_back(vertex);
    }
    
    mSubMesh[index].position = glm::vec3(x, y, z);
    
    glBindVertexArray(mVertexArray);
    glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}

bool Mesh::ChangeSubMeshColor(unsigned int index, Color newColor) {
    if (index >= mSubMesh.size()) return false;
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) {
        Vertex& vertex = *it;
        vertex.r = newColor.r;
        vertex.g = newColor.g;
        vertex.b = newColor.b;
        destMesh.push_back(vertex);
    }
    
    glBindVertexArray(mVertexArray);
    glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}

bool Mesh::ChangeSubMeshLightingValue(unsigned int index, Color newColor) {
    if (index >= mSubMesh.size()) return false;
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) {
        Vertex& vertex = *it;
        vertex.lr = newColor.r;
        vertex.lg = newColor.g;
        vertex.lb = newColor.b;
        destMesh.push_back(vertex);
    }
    
    glBindVertexArray(mVertexArray);
    glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}

void Mesh::UpdateMesh(void) {
    mVertexBufferSz = mVertexBuffer.size();
    mIndexBufferSz  = mIndexBuffer.size();
    
    glBindVertexArray(mVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, mBufferVertex);
    glBufferData(GL_ARRAY_BUFFER, mVertexBufferSz * sizeof(Vertex), &mVertexBuffer[0], GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferSz * sizeof(Index), &mIndexBuffer[0], GL_DYNAMIC_DRAW);
    return;
}
