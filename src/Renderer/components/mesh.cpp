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
    
    mAreBuffersAllocated(true)
{
    
    AllocateBuffers();
    
    SetDefaultAttributes();
}

Mesh::~Mesh() {
    FreeBuffers();
}


void Mesh::AddPlain(float x, float y, float z, float width, float height, Color color, float uCoord, float vCoord, float uStart, float vStart, unsigned int uOffset, unsigned int vOffset) {
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
    
    AddSubMesh(x, y, z, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
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
}

void Mesh::AddWall(float x, float y, float z, float width, float height, Color color) {
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
    
    AddSubMesh(x, y, z, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
}

void Mesh::AddWallSubDivided(float x, float y, float z, float width, float height, Color color, unsigned int widthSub, unsigned int heightSub) {
    for (unsigned int h=0; h < heightSub; h++) 
        for (unsigned int w=0; w < widthSub; w++) 
            AddWall( x + (w * width), y + (h * height), z, width, height, color);
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

void Mesh::AddSphere(float x, float y, float z, float radius, unsigned int rings, unsigned int sectors, Color color) {
    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    
    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);
    unsigned int r, s;
    
    for (r = 0; r < rings; r++) {
        
        for (s = 0; s < sectors; s++) {
            
            float const ys = sin(-(glm::pi<float>() / 2.0f) + glm::pi<float>() * r * R);
            float const xs = cos(2 * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);
            float const zs = sin(2 * glm::pi<float>() * s * S) * sin(glm::pi<float>() * r * R);
            
            vertices.push_back(Vertex(
                xs * radius, ys * radius, zs * radius,
                color.r, color.g, color.b,
                xs, ys, zs,
                (float)s * S, (float)r * R
            ));
            
        }
    }
    
    for (r = 0; r < rings - 1; r++) {
        
        for (s = 0; s < sectors - 1; s++) {
            
            indices.push_back(r * sectors + s);
            indices.push_back(r * sectors + (s + 1));
            indices.push_back((r + 1) * sectors + (s + 1));
            indices.push_back(r * sectors + s);
            indices.push_back((r + 1) * sectors + (s + 1));
            indices.push_back((r + 1) * sectors + s);
            
        }
        
    }
    AddSubMesh(x, y, z, vertices, indices, false);
}

int Mesh::AddSubMesh(float x, float y, float z, SubMesh& mesh, bool doUploadToGpu) {
    return AddSubMesh(x, y, z, mesh.vertexBuffer, mesh.indexBuffer, doUploadToGpu);
}

int Mesh::AddSubMesh(float x, float y, float z, std::vector<Vertex>& vertexBuffer, std::vector<Index>& indexBuffer, bool doUploadToGpu) {
    for (std::vector<SubMesh>::iterator it = mFreeMesh.begin(); it != mFreeMesh.end(); ++it) {
        if (vertexBuffer.size() != it->vertexCount || indexBuffer.size() != it->indexCount)
            continue;
        
        const SubMesh& slot = *it;
        
        if ((slot.vertexBegin + slot.vertexCount) > mVertexBuffer.size() ||
            (slot.indexBegin  + slot.indexCount)  > mIndexBuffer.size())
            return -1;
        
        SubMesh freeMesh = slot;
        mFreeMesh.erase(it);
        freeMesh.position = glm::vec3(x, y, z);
        
        mSubMesh.push_back(freeMesh);
        const int submeshIdx = static_cast<int>(mSubMesh.size() - 1);
        
        for (size_t i = 0; i < vertexBuffer.size(); ++i) {
            Vertex v = vertexBuffer[i];
            v.x += x; v.y += y; v.z += z;
            mVertexBuffer[freeMesh.vertexBegin + i] = v;
        }
        for (size_t i = 0; i < indexBuffer.size(); ++i) {
            mIndexBuffer[freeMesh.indexBegin + i].index =
                indexBuffer[i].index + freeMesh.vertexBegin;
        }
        
        if (doUploadToGpu) 
            Load();
        return submeshIdx;
    }
    
    unsigned int startVertex = mVertexBuffer.size();
    unsigned int startIndex  = mIndexBuffer.size();
    
    SubMesh newMesh;
    newMesh.vertexBegin = startVertex;
    newMesh.vertexCount = vertexBuffer.size();
    newMesh.indexBegin  = startIndex;
    newMesh.indexCount  = indexBuffer.size();
    newMesh.position    = glm::vec3(x, y, z);
    
    mSubMesh.push_back(newMesh);
    
    // Reserve with padding to reduce reallocations
    size_t requiredVertexSize = mVertexBuffer.size() + vertexBuffer.size();
    size_t requiredIndexSize  = mIndexBuffer.size() + indexBuffer.size();
    
    if (mVertexBuffer.capacity() < requiredVertexSize)
        mVertexBuffer.reserve(requiredVertexSize + 1000);
    
    if (mIndexBuffer.capacity() < requiredIndexSize)
        mIndexBuffer.reserve(requiredIndexSize + 1000);
    
    for (std::vector<Vertex>::iterator it = vertexBuffer.begin(); it != vertexBuffer.end(); ++it) {
        Vertex vertex = *it;
        vertex.x += x;
        vertex.y += y;
        vertex.z += z;
        mVertexBuffer.push_back(vertex);
    }
    
    for (std::vector<Index>::iterator it = indexBuffer.begin(); it != indexBuffer.end(); ++it) {
        Index index = *it;
        index.index += startVertex;
        mIndexBuffer.push_back(index);
    }
    
    if (doUploadToGpu)
        Load();
    return mSubMesh.size() - 1;
}

bool Mesh::ReplaceSubMesh(unsigned int index, SubMesh& newMesh, bool doUploadToGpu) {
    if (index >= mSubMesh.size())
        return false;
    const SubMesh& dst = mSubMesh[index];
    
    if (newMesh.vertexBuffer.size() != dst.vertexCount ||
        newMesh.indexBuffer.size()  != dst.indexCount)
        return false;
    
    const glm::vec3 pos = dst.position;
    for (size_t i = 0; i < newMesh.vertexBuffer.size(); ++i) {
        Vertex v = newMesh.vertexBuffer[i];
        v.x += pos.x; v.y += pos.y; v.z += pos.z;
        mVertexBuffer[dst.vertexBegin + i] = v;
    }
    
    for (size_t i = 0; i < newMesh.indexBuffer.size(); ++i) {
        mIndexBuffer[dst.indexBegin + i].index =
            newMesh.indexBuffer[i].index + dst.vertexBegin;
    }
    
    if (doUploadToGpu)
        Load();
    return true;
}

bool Mesh::ReplaceSubMesh(unsigned int index, std::vector<Vertex>& vertexBuffer, std::vector<Index>&  indexBuffer, bool doUploadToGpu) {
    if (index >= mSubMesh.size())
        return false;

    const SubMesh& dst = mSubMesh[index];
    if (vertexBuffer.size() != dst.vertexCount ||
        indexBuffer.size()  != dst.indexCount)
        return false;

    const glm::vec3 pos = dst.position;

    for (size_t i = 0; i < vertexBuffer.size(); ++i) {
        Vertex v = vertexBuffer[i];
        v.x += pos.x; v.y += pos.y; v.z += pos.z;
        mVertexBuffer[dst.vertexBegin + i] = v;
    }

    for (size_t i = 0; i < indexBuffer.size(); ++i) {
        mIndexBuffer[dst.indexBegin + i].index = indexBuffer[i].index + dst.vertexBegin;
    }

    if (doUploadToGpu)
        Load();
    return true;
}

bool Mesh::RemoveSubMesh(unsigned int index, bool doUploadToGpu) {
    if (index >= mSubMesh.size()) 
        return false;
    SubMesh& subMesh = mSubMesh[index];
    
    Vertex zeroV{};
    for (unsigned int i = 0; i < subMesh.vertexCount; ++i) 
        mVertexBuffer[subMesh.vertexBegin + i] = zeroV;
    
    Index zeroI{0};
    for (unsigned int i = 0; i < subMesh.indexCount; ++i) 
        mIndexBuffer[subMesh.indexBegin + i] = zeroI;
    
    if (doUploadToGpu) {
        LoadVertexRange(subMesh.vertexBegin, subMesh.vertexCount);
        LoadIndexRange (subMesh.indexBegin,  subMesh.indexCount);
    }
    
    mFreeMesh.push_back(subMesh);
    mSubMesh.erase(mSubMesh.begin() + index);
    return true;
}

bool Mesh::GetSubMesh(unsigned int index, SubMesh& subMesh) {
    if (index >= mSubMesh.size()) 
        return false;
    const SubMesh& subMeshSrc = mSubMesh[index];
    
    std::vector<Vertex>::iterator vertexStart = mVertexBuffer.begin() + subMeshSrc.vertexBegin;
    std::vector<Vertex>::iterator vertexEnd   = vertexStart + subMeshSrc.vertexCount;
    subMesh.vertexBuffer.insert(subMesh.vertexBuffer.end(), vertexStart, vertexEnd);
    
    std::vector<Index>::iterator indexStart = mIndexBuffer.begin() + subMeshSrc.indexBegin;
    std::vector<Index>::iterator indexEnd   = indexStart + subMeshSrc.indexCount;
    subMesh.indexBuffer.insert(subMesh.indexBuffer.end(), indexStart, indexEnd);
    
    subMesh.vertexCount += subMeshSrc.vertexCount;
    subMesh.indexCount  += subMeshSrc.indexCount;
    subMesh.position     = subMeshSrc.position;
    return true;
}

bool Mesh::GetSubMesh(unsigned int index, std::vector<Vertex>& vertexBuffer, std::vector<Index>& indexBuffer) {
    if (index >= mSubMesh.size()) 
        return false;
    const SubMesh& subMesh = mSubMesh[index];
    
    std::vector<Vertex>::iterator vertexStart = mVertexBuffer.begin() + subMesh.vertexBegin;
    std::vector<Vertex>::iterator vertexEnd   = vertexStart + subMesh.vertexCount;
    vertexBuffer.insert(vertexBuffer.end(), vertexStart, vertexEnd);
    
    std::vector<Index>::iterator indexStart = mIndexBuffer.begin() + subMesh.indexBegin;
    std::vector<Index>::iterator indexEnd   = indexStart + subMesh.indexCount;
    indexBuffer.insert(indexBuffer.end(), indexStart, indexEnd);
    return true;
}

bool Mesh::ChangeSubMeshPosition(unsigned int index, float x, float y, float z) {
    if (index >= mSubMesh.size()) 
        return false;
    SubMesh& subMesh = mSubMesh[index];
    
    float dx = x - subMesh.position.x;
    float dy = y - subMesh.position.y;
    float dz = z - subMesh.position.z;
    
    std::vector<Vertex>::iterator vertexStart = mVertexBuffer.begin() + subMesh.vertexBegin;
    std::vector<Vertex>::iterator vertexEnd   = vertexStart + subMesh.vertexCount;
    
    for (std::vector<Vertex>::iterator it = vertexStart; it != vertexEnd; ++it) {
        Vertex& vertex = *it;
        vertex.x += dx;
        vertex.y += dy;
        vertex.z += dz;
    }
    
    subMesh.position = glm::vec3(x, y, z);
    return true;
}

bool Mesh::ChangeSubMeshRotation(unsigned int index, float angle, glm::vec3 axis) {
    if (index >= mSubMesh.size()) 
        return false;
    SubMesh& subMesh = mSubMesh[index];
    
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    glm::vec3 pivot = subMesh.position;
    
    unsigned int start = subMesh.vertexBegin;
    unsigned int end   = start + subMesh.vertexCount;
    
    for (unsigned int i = start; i < end; i++) {
        Vertex& vertex = mVertexBuffer[i];
        glm::vec4 localPos(vertex.x - pivot.x, vertex.y - pivot.y, vertex.z - pivot.z, 1.0f);
        glm::vec4 rotated = rotationMatrix * localPos;
        
        vertex.x = rotated.x + pivot.x;
        vertex.y = rotated.y + pivot.y;
        vertex.z = rotated.z + pivot.z;
    }
    return true;
}

bool Mesh::ChangeSubMeshRotationAroundPoint(unsigned int index, float angle, glm::vec3 axis, glm::vec3 center) {
    if (index >= mSubMesh.size())
        return false;
    const SubMesh& subMesh = mSubMesh[index];
    
    glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(angle), axis);
    
    unsigned int start = subMesh.vertexBegin;
    unsigned int end   = start + subMesh.vertexCount;
    
    for (unsigned int i = start; i < end; ++i) {
        Vertex& v = mVertexBuffer[i];
        
        glm::vec4 toLocal(v.x - center.x, v.y - center.y, v.z - center.z, 1.0f);
        glm::vec4 rotated = rotationMatrix * toLocal;
        
        v.x = rotated.x + center.x;
        v.y = rotated.y + center.y;
        v.z = rotated.z + center.z;
    }
    return true;
}

bool Mesh::ChangeSubMeshScale(unsigned int index, float scaleX, float scaleY, float scaleZ) {
    if (index >= mSubMesh.size()) 
        return false;
    
    SubMesh& subMesh = mSubMesh[index];
    glm::vec3 pivot = subMesh.position;
    
    unsigned int start = subMesh.vertexBegin;
    unsigned int end   = start + subMesh.vertexCount;
    
    for (unsigned int i = start; i < end; ++i) {
        Vertex& vertex = mVertexBuffer[i];

        vertex.x = (vertex.x - pivot.x) * scaleX + pivot.x;
        vertex.y = (vertex.y - pivot.y) * scaleY + pivot.y;
        vertex.z = (vertex.z - pivot.z) * scaleZ + pivot.z;
    }
    return true;
}

bool Mesh::ChangeSubMeshColor(unsigned int index, Color newColor) {
    if (index >= mSubMesh.size()) 
        return false;
    
    SubMesh& subMesh = mSubMesh[index];
    
    std::vector<Vertex>::iterator itStart = mVertexBuffer.begin() + subMesh.vertexBegin;
    std::vector<Vertex>::iterator itEnd   = itStart + subMesh.vertexCount;
    
    for (std::vector<Vertex>::iterator it = itStart; it != itEnd; ++it) {
        it->r = newColor.r;
        it->g = newColor.g;
        it->b = newColor.b;
    }
    return true;
}

bool Mesh::ChangeSubMeshPoints(unsigned int index, std::vector<glm::vec3> points) {
    if (index >= mSubMesh.size()) 
        return false;
    
    SubMesh& subMesh = mSubMesh[index];
    
    if (points.size() < subMesh.vertexCount)
        return false;
    
    unsigned int start = subMesh.vertexBegin;
    // unsigned int end = start + subMesh.vertexCount;
    
    for (unsigned int i = 0; i < subMesh.vertexCount; ++i) {
        const glm::vec3& point = points[i];
        Vertex& vertex = mVertexBuffer[start + i];
        vertex.x = point.x;
        vertex.y = point.y;
        vertex.z = point.z;
    }
    return true;
}

void Mesh::ClearSubMeshes(void) {
    mSubMesh.clear();
    mVertexBuffer.clear();
    mIndexBuffer.clear();
}

void Mesh::Load(void) {
    mVertexBufferSz = mVertexBuffer.size();
    mIndexBufferSz  = mIndexBuffer.size();
    
    if (!mAreBuffersAllocated) {
        AllocateBuffers();
        mAreBuffersAllocated = true;
    }
    
    glBindVertexArray(mVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, mBufferVertex);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(mVertexBufferSz * sizeof(Vertex)),
        mVertexBuffer.data(),
        GL_STATIC_DRAW
    );
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferIndex);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(mIndexBufferSz * sizeof(Index)),
        mIndexBuffer.data(),
        GL_STATIC_DRAW
    );
}

void Mesh::Unload(void) {
    if (!mAreBuffersAllocated) 
        return;
    
    FreeBuffers();
    
    mAreBuffersAllocated = false;
}


bool Mesh::LoadVertexRange(unsigned int vertexStart, unsigned int vertexCount) {
    if (vertexCount == 0) return true;
    
    if (!mAreBuffersAllocated) {
        AllocateBuffers();
        mAreBuffersAllocated = true;
    }
    
    // Check to do a full reupload
    const unsigned int cpuVerts = static_cast<unsigned int>(mVertexBuffer.size());
    if (cpuVerts > mVertexBufferSz) {
        Load();
        return true;
    }
    
    // Bounds check against both CPU data and GPU allocation size
    if (vertexStart >= cpuVerts || (vertexStart + vertexCount) > cpuVerts) return false;
    if ((vertexStart + vertexCount) > mVertexBufferSz) return false;
    
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferVertex);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        static_cast<GLintptr>(vertexStart * sizeof(Vertex)),
        static_cast<GLsizeiptr>(vertexCount * sizeof(Vertex)),
        &mVertexBuffer[vertexStart]
    );
    return true;
}

bool Mesh::LoadIndexRange(unsigned int indexStart, unsigned int indexCount) {
    if (indexCount == 0) return true;
    
    if (!mAreBuffersAllocated) {
        AllocateBuffers();
        mAreBuffersAllocated = true;
    }
    
    // If CPU buffer grew beyond last allocated GPU size, do a full reupload.
    const unsigned int cpuIdx = static_cast<unsigned int>(mIndexBuffer.size());
    if (cpuIdx > mIndexBufferSz) {
        Load();
        return true;
    }
    
    // Bounds check against both CPU data and GPU allocation size
    if (indexStart >= cpuIdx || (indexStart + indexCount) > cpuIdx) return false;
    if ((indexStart + indexCount) > mIndexBufferSz) return false;
    
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferIndex);
    glBufferSubData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLintptr>(indexStart * sizeof(Index)),
        static_cast<GLsizeiptr>(indexCount * sizeof(Index)),
        &mIndexBuffer[indexStart]
    );
    return true;
}

bool Mesh::LoadSubMesh(unsigned int submeshIndex) {
    if (submeshIndex >= mSubMesh.size()) return false;
    const SubMesh& sm = mSubMesh[submeshIndex];
    bool okV = LoadVertexRange(sm.vertexBegin, sm.vertexCount);
    bool okI = LoadIndexRange (sm.indexBegin,  sm.indexCount);
    return okV && okI;
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
}

Index Mesh::GetIndex(unsigned int index) {
    return mIndexBuffer[index];
}

void Mesh::SetIndex(unsigned int index, Index position) {
    mIndexBuffer[index] = position;
}

void Mesh::CalculateNormals(void) {
    // Ensure we have enough vertices to process full triangles
    const size_t limit = std::min(mVertexBufferSz, static_cast<unsigned int>(mVertexBuffer.size()));
    
    for (size_t i = 0; i + 2 < limit; i += 3) {
        const Vertex& vertA = mVertexBuffer[i];
        const Vertex& vertB = mVertexBuffer[i + 1];
        const Vertex& vertC = mVertexBuffer[i + 2];
        
        glm::vec3 U = glm::vec3(vertB.x - vertA.x, vertB.y - vertA.y, vertB.z - vertA.z);
        glm::vec3 V = glm::vec3(vertC.x - vertA.x, vertC.y - vertA.y, vertC.z - vertA.z);
        
        glm::vec3 normal = glm::normalize(glm::cross(U, V));
        
        for (int j = 0; j < 3; ++j) {
            mVertexBuffer[i + j].nx = -normal.x;
            mVertexBuffer[i + j].ny = -normal.y;
            mVertexBuffer[i + j].nz = -normal.z;
        }
    }
}

void Mesh::SetNormals(glm::vec3 normals) {
    for (unsigned int i=0; i < mVertexBuffer.size(); i++) {
        mVertexBuffer[i].nx = normals.x;
        mVertexBuffer[i].ny = normals.y;
        mVertexBuffer[i].nz = normals.z;
    }
}

void Mesh::SetPrimitive(int primitiveType) {
    mPrimitive = primitiveType;
}

void Mesh::SetAttribute(int index, int attributeCount, int vertexSize, int byteOffset) {
    Bind();
    glEnableVertexAttribArray(index);
    GLintptr offset = byteOffset;
    glVertexAttribPointer(index, attributeCount, GL_FLOAT, GL_FALSE, vertexSize, (GLvoid*)offset);
}

void Mesh::DisableAttribute(int index) {
    Bind();
    glDisableVertexAttribArray(index);
}

void Mesh::LoadVertexBuffer(Vertex* bufferData, int vertexCount) {
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferVertex);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), &bufferData[0], GL_STATIC_DRAW);
}

void Mesh::LoadIndexBuffer(Index* bufferData, int indexCount) {
    glBindVertexArray(mVertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(Index), &bufferData[0], GL_STATIC_DRAW);
}

void Mesh::Bind() {
    glBindVertexArray(mVertexArray);
}

void Mesh::AllocateBuffers(void) {
    mVertexBufferSz = mVertexBuffer.size();
    mIndexBufferSz  = mIndexBuffer.size();
    
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);
    
    glGenBuffers(1, &mBufferVertex);
    glBindBuffer(GL_ARRAY_BUFFER, mBufferVertex);
    glBufferData(GL_ARRAY_BUFFER, mVertexBufferSz * sizeof(Vertex), NULL, GL_STATIC_DRAW);
    
    glGenBuffers(1, &mBufferIndex);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferIndex);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferSz * sizeof(Index), NULL, GL_STATIC_DRAW);
}

void Mesh::FreeBuffers(void) {
    if (mVertexArray != 0) glDeleteVertexArrays(1, &mVertexArray);
    if (mBufferVertex != 0) glDeleteBuffers(1, &mBufferVertex);
    if (mBufferIndex  != 0) glDeleteBuffers(1, &mBufferIndex);
    mVertexArray = 0;
    mBufferVertex = 0;
    mBufferIndex = 0;
}

void Mesh::DrawVertexArray(void) {
    glDrawArrays(mPrimitive, 0, mVertexBufferSz);
}

void Mesh::DrawIndexArray(void) {
    glDrawElements(mPrimitive, mIndexBufferSz, GL_UNSIGNED_INT, (void*)0);
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
}
