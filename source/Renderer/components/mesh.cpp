#include "mesh.h"


Mesh::Mesh() {
    
    primitive = GL_TRIANGLES;
    shader    = nullptr;
    
    AllocateBuffers(0);
}

Mesh::~Mesh() {
    
    FreeBuffers();
}


void Mesh::SetPrimitive(int primitiveType) {primitive = primitiveType;}

void Mesh::SetAttribute(int index, int attributeCount, int vertexSize, int byteOffset) {
    Bind();
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, attributeCount, GL_FLOAT, GL_FALSE, vertexSize, (void*) byteOffset);
}

void Mesh::DisableAttribute(int index) {
    Bind();
    glDisableVertexAttribArray(index);
}



void Mesh::LoadVertexBuffer(Vertex* bufferData, int vertexCount) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, bufferVertex);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), &bufferData[0], GL_DYNAMIC_DRAW);
}

void Mesh::LoadIndexBuffer(Index* bufferData, int indexCount) {
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), &bufferData[0], GL_DYNAMIC_DRAW);
}


void Mesh::Bind() {
    glBindVertexArray(vertexArray);
}


void Mesh::AllocateBuffers(unsigned int maxBufferSize) {
    maxSize = maxBufferSize;
    
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    glGenBuffers(1, &bufferVertex);
    glBindBuffer(GL_ARRAY_BUFFER, bufferVertex);
    glBufferData(GL_ARRAY_BUFFER, maxSize * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);
    
    glGenBuffers(1, &bufferIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxSize * sizeof(Index), NULL, GL_DYNAMIC_DRAW);
    
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

unsigned int Mesh::GetSubMeshCount(void) {
    return subMesh.size();
}

void Mesh::SetDefaultAttributes(void) {
    
    Bind();
    
    SetAttribute(0, 3, sizeof(Vertex), 0);
    SetAttribute(1, 3, sizeof(Vertex), 12);
    SetAttribute(2, 2, sizeof(Vertex), 24);
    
}


bool Mesh::AddPlane(float x, float y, float z, float width, float height, Color color) {
    
    Vertex vertex[4];
    vertex[0] = Vertex(x-width, y, z+height,  color.r, color.g, color.b, 0, 1);
    vertex[1] = Vertex(x+width, y, z+height,  color.r, color.g, color.b, 1, 1);
    vertex[2] = Vertex(x+width, y, z-height,  color.r, color.g, color.b, 1, 0);
    vertex[3] = Vertex(x-width, y, z-height,  color.r, color.g, color.b, 0, 0);
    
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
    
    return AddSubMesh(x, y, x, subBuffer.vertexBuffer, subBuffer.indexBuffer);
}


bool Mesh::AddSubMesh(float x, float y, float z, SubMesh& mesh) {
    return AddSubMesh(x, y, z, mesh.vertexBuffer, mesh.indexBuffer);
}

bool Mesh::AddSubMesh(float x, float y, float z, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer) {
    
    if (freeMesh.size() > 0) {
        
        for (std::vector<SubMesh>::iterator it = freeMesh.begin(); it != freeMesh.end(); ++it) {
            SubMesh freeMeshPtr = *it;
            
            if (vrtxBuffer.size() != freeMeshPtr.vertexCount) continue;
            
            freeMesh.erase(it);
            
            freeMeshPtr.position = glm::vec3(x, y, z);
            
            subMesh.push_back(freeMeshPtr);
            
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
                i++;
            }
            
            i = 0;
            for (std::vector<Index>::iterator it = indexBuffer.begin() + freeMeshPtr.indexBegin; it != indexBuffer.begin() + freeMeshPtr.indexBegin + freeMeshPtr.indexCount; ++it) {
                Index& index = *it;
                index.index = indxBuffer[i].index + freeMeshPtr.vertexBegin;
                i++;
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, bufferVertex);
            glBufferSubData(GL_ARRAY_BUFFER, freeMeshPtr.vertexBegin * sizeof(Vertex), freeMeshPtr.vertexCount * sizeof(Vertex), &vertexBuffer[freeMeshPtr.vertexBegin]);
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, freeMeshPtr.indexBegin * sizeof(Index),  freeMeshPtr.indexCount * sizeof(Index), &indexBuffer[freeMeshPtr.indexBegin]);
            
            return false;
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
    
    // Check to allocate new GPU memory
    if ((vertexBuffer.size() > maxSize) | (indexBuffer.size() > maxSize)) {
        glBindVertexArray(vertexArray);
        
        glBindBuffer(GL_ARRAY_BUFFER, bufferVertex);
        glBufferData(GL_ARRAY_BUFFER, (vertexBuffer.size() * sizeof(Vertex)) + (vrtxBuffer.size() * 100), NULL, GL_DYNAMIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (indexBuffer.size() * sizeof(Index)) + (indxBuffer.size() * 100), NULL, GL_DYNAMIC_DRAW);
    }
    
    // Re-upload the buffer
    glBindBuffer(GL_ARRAY_BUFFER, bufferVertex);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertexBuffer.size() * sizeof(Vertex), &vertexBuffer[0]);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indexBuffer.size() * sizeof(Index), &indexBuffer[0]);
    
    return true;
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
    
    glBindVertexArray(vertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, bufferVertex);
    glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(Vertex), &vertexBuffer[0], GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(Index), &indexBuffer[0], GL_DYNAMIC_DRAW);
    
    //LoadVertexBuffer(vertexBuffer.data(), vertexBuffer.size());
    //LoadIndexBuffer(indexBuffer.data(), indexBuffer.size());
    
}


