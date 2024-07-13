#include <GameEngineFramework/Renderer/components/mesh.h>
#include <GameEngineFramework/Math/Math.h>

#include <GameEngineFramework/Math/Random.h>

#define GLEW_STATIC
#include <gl/glew.h>

extern MathCore Math;
extern NumberGeneration Random;


Mesh::Mesh() : 
    
    isShared(false),
    
    mPrimitive(GL_TRIANGLES),
    
    mVertexBufferSz(0),
    mIndexBufferSz(0),
    mMaxSize(0),
    
    mAreBuffersAllocated(false)
{
    AllocateBuffers(1024);
    
    SetDefaultAttributes();
    return;
}

Mesh::~Mesh() {
    
    FreeBuffers();
    return;
}


void Mesh::AddPlain(float x, float y, float z, float width, float height, Color color, float uCoord, float vCoord, float uStart, float vStart, unsigned int uOffset, unsigned int vOffset) {
    
    // Align the mesh
    z = (z * 2) - x;
    
    Vertex vertex[4];
    vertex[0] = Vertex( x-width, y, z+height,   color.r, color.g, color.b,   0, 1, 0,   uStart + (uOffset * uCoord) + 0,      vStart + (vOffset * vCoord) + vCoord);
    vertex[1] = Vertex( x+width, y, z+height,   color.r, color.g, color.b,   0, 1, 0,   uStart + (uOffset * uCoord) + 0,      vStart + (vOffset * vCoord) + 0 );
    vertex[2] = Vertex( x+width, y, z-height,   color.r, color.g, color.b,   0, 1, 0,   uStart + (uOffset * uCoord) + uCoord, vStart + (vOffset * vCoord) + 0 );
    vertex[3] = Vertex( x-width, y, z-height,   color.r, color.g, color.b,   0, 1, 0,   uStart + (uOffset * uCoord) + uCoord, vStart + (vOffset * vCoord) + vCoord);
    
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
    for (unsigned int w=0; w < widthSub; w++) {
        
        for (unsigned int h=0; h < heightSub; h++) {
            
            AddPlain(x + (w * width), 
                     y, 
                     z + (h * height), 
                     width, height, 
                     color);
            
        }
        
    }
    return;
}

void Mesh::AddWall(float x, float y, float z, float width, float height, Color color) {
    
    // Align the mesh
    z -= x;
    
    Vertex vertex[4];
    vertex[0] = Vertex( x-width, y+height, z,   color.r, color.g, color.b,   0,0,1,  0, 1 );
    vertex[1] = Vertex( x+width, y+height, z,   color.r, color.g, color.b,   0,0,1,  0, 0 );
    vertex[2] = Vertex( x+width, y-height, z,   color.r, color.g, color.b,   0,0,1,  1, 0 );
    vertex[3] = Vertex( x-width, y-height, z,   color.r, color.g, color.b,   0,0,1,  1, 1 );
    
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
    return;
}

int Mesh::AddQuad(float x, float y, float z, float width, float height, Color color) {
    
    Vertex vertex[4];
    vertex[0] = Vertex( x, y,       z,         color.r, color.g, color.b,   0, 1, 0,  0, 0 );
    vertex[1] = Vertex( x+width, y, z,         color.r, color.g, color.b,   0, 1, 0,  1, 0 );
    vertex[2] = Vertex( x+width, y, z+height,  color.r, color.g, color.b,   0, 1, 0,  1, 1 );
    vertex[3] = Vertex( x, y,       z+height,  color.r, color.g, color.b,   0, 1, 0,  0, 1 );
    
    SubMesh subBuffer;
    subBuffer.vertexBuffer.push_back(vertex[0]);
    subBuffer.vertexBuffer.push_back(vertex[1]);
    subBuffer.vertexBuffer.push_back(vertex[2]);
    subBuffer.vertexBuffer.push_back(vertex[3]);
    
    subBuffer.indexBuffer.push_back(0);
    subBuffer.indexBuffer.push_back(2);
    subBuffer.indexBuffer.push_back(1);
    
    subBuffer.indexBuffer.push_back(0);
    subBuffer.indexBuffer.push_back(3);
    subBuffer.indexBuffer.push_back(2);
    
    return AddSubMesh(x, y, z, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
}

int Mesh::AddSubMesh(float x, float y, float z, SubMesh& mesh, bool doUploadToGpu) {
    return AddSubMesh(x, y, z, mesh.vertexBuffer, mesh.indexBuffer, doUploadToGpu);
}

int Mesh::AddSubMesh(float x, float y, float z, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer, bool doUploadToGpu) {
    
    if (mFreeMesh.size() > 0) {
        
        // Find an open slot in the buffer for the mesh
        for (std::vector<SubMesh>::iterator it = mFreeMesh.begin(); it != mFreeMesh.end(); ++it) {
            SubMesh freeMeshPtr = *it;
            
            if (vrtxBuffer.size() != freeMeshPtr.vertexCount) 
                continue;
            
            mFreeMesh.erase(it);
            
            freeMeshPtr.position = glm::vec3(x, y, z);
            
            mSubMesh.push_back(freeMeshPtr);
            
            unsigned int i = 0;
            for (std::vector<Vertex>::iterator itsub = mVertexBuffer.begin() + freeMeshPtr.vertexBegin; itsub != mVertexBuffer.begin() + freeMeshPtr.vertexBegin + freeMeshPtr.vertexCount; ++itsub) {
                Vertex& vertex = *itsub;
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
            for (std::vector<Index>::iterator itsub = mIndexBuffer.begin() + freeMeshPtr.indexBegin; itsub != mIndexBuffer.begin() + freeMeshPtr.indexBegin + freeMeshPtr.indexCount; ++itsub) {
                Index& index = *itsub;
                index.index = indxBuffer[i].index + freeMeshPtr.vertexBegin;
                i++;
            }
            
            if (doUploadToGpu) 
                Load();
            
            return -1;
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
    
    // Copy the buffers
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
    
    if (doUploadToGpu) 
        Load();
    
    return startVertex;
}

bool Mesh::RemoveSubMesh(unsigned int index) {
    if (index >= mSubMesh.size()) 
        return false;
    
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

bool Mesh::GetSubMesh(unsigned int index, SubMesh& mesh) {
    
    if (index >= mSubMesh.size()) 
        return false;
    
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) 
        mesh.vertexBuffer.push_back(*it);
    
    for (std::vector<Index>::iterator it = mIndexBuffer.begin() + sourceMesh.indexBegin; it != mIndexBuffer.begin() + sourceMesh.indexBegin + sourceMesh.indexCount; ++it) 
        mesh.indexBuffer.push_back(*it);
    
    mesh.vertexCount += sourceMesh.vertexCount;
    mesh.indexCount  += sourceMesh.indexCount;
    
    return true;
}

bool Mesh::GetSubMesh(unsigned int index, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer) {
    
    if (index >= mSubMesh.size()) 
        return false;
    
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) 
        vrtxBuffer.push_back(*it);
    
    for (std::vector<Index>::iterator it = mIndexBuffer.begin() + sourceMesh.indexBegin; it != mIndexBuffer.begin() + sourceMesh.indexBegin + sourceMesh.indexCount; ++it) 
        indxBuffer.push_back(*it);
    
    return true;
}

bool Mesh::ChangeSubMeshPosition(unsigned int index, float x, float y, float z) {
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) {
        Vertex& vertex = *it;
        // Zero the sub-mesh position
        vertex.x -= sourceMesh.position.x;
        vertex.y -= sourceMesh.position.y;
        vertex.z -= sourceMesh.position.z;
        // Move sub-mesh to new offset
        vertex.x += x;
        vertex.y += y;
        vertex.z += z;
        destMesh.push_back(vertex);
    }
    
    mSubMesh[index].position = glm::vec3(x, y, z);
    
    //glBindVertexArray(mVertexArray);
    //glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}

bool Mesh::ChangeSubMeshColor(unsigned int index, Color newColor) {
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = mSubMesh[index];
    
    for (std::vector<Vertex>::iterator it = mVertexBuffer.begin() + sourceMesh.vertexBegin; it != mVertexBuffer.begin() + sourceMesh.vertexBegin + sourceMesh.vertexCount; ++it) {
        Vertex& vertex = *it;
        vertex.r = newColor.r;
        vertex.g = newColor.g;
        vertex.b = newColor.b;
        destMesh.push_back(vertex);
    }
    
    //glBindVertexArray(mVertexArray);
    //glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}

bool Mesh::ChangeSubMeshPoints(unsigned int index, std::vector<glm::vec3> points) {
    
    std::vector<Vertex> destMesh;
    SubMesh sourceMesh = mSubMesh[index];
    
    for (unsigned int i=0; i < sourceMesh.vertexCount; i++) {
        
        mVertexBuffer[i].x = points[i].x;
        mVertexBuffer[i].y = points[i].y;
        mVertexBuffer[i].z = points[i].z;
        
        destMesh.push_back(mVertexBuffer[i]);
        
        if (points.size() >= i) 
            return true;
        
    }
    
    //glBindVertexArray(mVertexArray);
    //glBufferSubData(GL_ARRAY_BUFFER, sourceMesh.vertexBegin * sizeof(Vertex), sourceMesh.vertexCount * sizeof(Vertex), &destMesh[0]);
    
    return true;
}

void Mesh::ClearSubMeshes(void) {
    mSubMesh.clear();
    mVertexBuffer.clear();
    mIndexBuffer.clear();
    return;
}

void Mesh::Load(void) {
    
    /*
    
    if (!mAreBuffersAllocated) {
        
        mVertexBufferSz = mVertexBuffer.size();
        mIndexBufferSz  = mIndexBuffer.size();
        
        if (mVertexBufferSz > mIndexBufferSz) {
            
            AllocateBuffers( mVertexBufferSz );
            
        } else {
            
            AllocateBuffers( mIndexBufferSz );
            
        }
        
    }
    
    */
    
    glBindVertexArray(mVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, mBufferVertex);
    glBufferData(GL_ARRAY_BUFFER, mVertexBufferSz * sizeof(Vertex), &mVertexBuffer[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferSz * sizeof(Index), &mIndexBuffer[0], GL_STATIC_DRAW);
    
    mAreBuffersAllocated = true;
    
    return;
}

void Mesh::Unload(void) {
    /*
    
    if (!mAreBuffersAllocated) 
        return;
    */
    
    FreeBuffers();
    
    mAreBuffersAllocated = false;
    
    return;
}

bool Mesh::CheckIsAllocatedOnGPU(void) {
    return mAreBuffersAllocated;
}

unsigned int Mesh::GetNumberOfVertices(void) {
    return mVertexBuffer.size();
}

unsigned int Mesh::GetNumberOfIndices(void) {
    return mIndexBuffer.size();
}

Vertex Mesh::GetVertex(unsigned int index) {
    return mVertexBuffer[index];
}

void Mesh::SetVertex(unsigned int index, Vertex vertex) {
    mVertexBuffer[index] = vertex;
    return;
}

Index Mesh::GetIndex(unsigned int index) {
    return mIndexBuffer[index];
}

void Mesh::SetIndex(unsigned int index, Index position) {
    mIndexBuffer[index] = position;
    return;
}

void Mesh::CalculateNormals(void) {
    
    for (unsigned int i=0; i < mVertexBufferSz; i += 3) {
        
        Vertex vertA = mVertexBuffer[i  ];
        Vertex vertB = mVertexBuffer[i+1];
        Vertex vertC = mVertexBuffer[i+2];
        
        glm::vec3 U;
        U.x = vertB.x - vertA.x;
        U.y = vertB.y - vertA.y;
        U.z = vertB.z - vertA.z;
        
        glm::vec3 V;
        V.x = vertC.x - vertA.x;
        V.y = vertC.y - vertA.y;
        V.z = vertC.z - vertA.z;
        
        glm::vec3 normal;
        normal.x = (U.y * V.z) - (U.z * V.y);
        normal.y = (U.z * V.x) - (U.x * V.z);
        normal.z = (U.x * V.y) - (U.y * V.x);
        
        mVertexBuffer[i].nx   = -normal.x;
        mVertexBuffer[i].ny   = -normal.y;
        mVertexBuffer[i].nz   = -normal.z;
        
        mVertexBuffer[i+1].nx = -normal.x;
        mVertexBuffer[i+1].ny = -normal.y;
        mVertexBuffer[i+1].nz = -normal.z;
        
        mVertexBuffer[i+2].nx = -normal.x;
        mVertexBuffer[i+2].ny = -normal.y;
        mVertexBuffer[i+2].nz = -normal.z;
        
        continue;
    }
    
    return;
}

void Mesh::SetNormals(glm::vec3 normals) {
    
    for (unsigned int i=0; i < mVertexBufferSz; i++) {
        
        mVertexBuffer[i].nx = normals.x;
        mVertexBuffer[i].ny = normals.y;
        mVertexBuffer[i].nz = normals.z;
        
        continue;
    }
    
    return;
}

void Mesh::Reallocate(unsigned int newBufferSize) {
    FreeBuffers();
    AllocateBuffers(newBufferSize);
    
    SetDefaultAttributes();
    return;
}

void Mesh::SetPrimitive(int primitiveType) {
    mPrimitive = primitiveType;
    return;
}

void Mesh::SetAttribute(int index, int attributeCount, int vertexSize, int byteOffset) {
    Bind();
    glEnableVertexAttribArray(index);
    GLintptr offset = byteOffset;
    glVertexAttribPointer(index, attributeCount, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)offset);
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
    glDeleteVertexArrays(1, &mVertexArray);
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
    
    return;
}
