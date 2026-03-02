#include <GameEngineFramework/Renderer/components/mesh.h>
#include <GameEngineFramework/Math/Math.h>

#include <GameEngineFramework/Math/Random.h>

#include <unordered_map>
#include <limits>

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
    
    subMesh.name         = subMeshSrc.name;
    subMesh.vertexCount += subMeshSrc.vertexCount;
    subMesh.indexCount  += subMeshSrc.indexCount;
    subMesh.position     = subMeshSrc.position;
    return true;
}

SubMesh* Mesh::GetSubMesh(unsigned int index) {
    if (index >= mSubMesh.size()) 
        return nullptr;
    return &mSubMesh[index];
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


bool Mesh::GenerateSimplifiedLOD(unsigned int submeshIndex, float reductionFactor, SubMesh& outSubMesh) {
    // Use the existing vector-based simplifier
    std::vector<Vertex> lodVertices;
    std::vector<Index>  lodIndices;

    if (!GenerateSimplifiedLOD(submeshIndex, reductionFactor, lodVertices, lodIndices)) {
        return false;
    }

    if (submeshIndex >= mSubMesh.size()) {
        return false;
    }

    const SubMesh& sourceSubMesh = mSubMesh[submeshIndex];

    // Build a standalone SubMesh using the simplified data.
    // vertexBegin/indexBegin are 0 because this is not yet packed
    // into Mesh::mVertexBuffer / mIndexBuffer.
    SubMesh lodSubMesh;
    lodSubMesh.vertexBuffer.swap(lodVertices);
    lodSubMesh.indexBuffer.swap(lodIndices);

    lodSubMesh.vertexBegin = 0;
    lodSubMesh.indexBegin  = 0;
    lodSubMesh.vertexCount = static_cast<unsigned int>(lodSubMesh.vertexBuffer.size());
    lodSubMesh.indexCount  = static_cast<unsigned int>(lodSubMesh.indexBuffer.size());

    // Keep original name/position (caller can rename if desired)
    lodSubMesh.name     = sourceSubMesh.name;
    lodSubMesh.position = sourceSubMesh.position;

    outSubMesh = lodSubMesh;
    return true;
}

bool Mesh::GenerateSimplifiedLOD(unsigned int submeshIndex, float reductionFactor, std::vector<Vertex>& outVertices, std::vector<Index>& outIndices) {
    outVertices.clear();
    outIndices.clear();

    if (submeshIndex >= mSubMesh.size()) {
        return false;
    }

    if (reductionFactor <= 0.0f || reductionFactor > 1.0f) {
        return false;
    }

    const SubMesh& sourceSubMesh = mSubMesh[submeshIndex];

    // Extract source vertices/indices for this submesh
    std::vector<Vertex> srcVertices;
    std::vector<Index>  srcIndices;

    srcVertices.reserve(sourceSubMesh.vertexCount);
    srcIndices.reserve(sourceSubMesh.indexCount);

    std::vector<Vertex>::const_iterator vBegin =
        mVertexBuffer.begin() + sourceSubMesh.vertexBegin;
    std::vector<Vertex>::const_iterator vEnd =
        vBegin + sourceSubMesh.vertexCount;

    srcVertices.insert(srcVertices.end(), vBegin, vEnd);

    std::vector<Index>::const_iterator iBegin =
        mIndexBuffer.begin() + sourceSubMesh.indexBegin;
    std::vector<Index>::const_iterator iEnd =
        iBegin + sourceSubMesh.indexCount;

    srcIndices.insert(srcIndices.end(), iBegin, iEnd);

    if (srcVertices.empty() || srcIndices.size() < 3) {
        return false;
    }

    // ------------------------------------------------------------
    // Step 1: Compute bounding box of the vertices
    // ------------------------------------------------------------
    glm::vec3 minPos(
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()
    );
    glm::vec3 maxPos(
        -std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max(),
        -std::numeric_limits<float>::max()
    );

    for (size_t i = 0; i < srcVertices.size(); ++i) {
        const Vertex& v = srcVertices[i];
        if (v.x < minPos.x) minPos.x = v.x;
        if (v.y < minPos.y) minPos.y = v.y;
        if (v.z < minPos.z) minPos.z = v.z;

        if (v.x > maxPos.x) maxPos.x = v.x;
        if (v.y > maxPos.y) maxPos.y = v.y;
        if (v.z > maxPos.z) maxPos.z = v.z;
    }

    glm::vec3 extent = maxPos - minPos;

    const float epsilon = 1e-5f;
    if (extent.x < epsilon) extent.x = 1.0f;
    if (extent.y < epsilon) extent.y = 1.0f;
    if (extent.z < epsilon) extent.z = 1.0f;

    // A small tolerance for detecting boundary vertices
    float maxExtent = glm::max(extent.x, glm::max(extent.y, extent.z));
    float boundaryEps = maxExtent * 1e-3f; // 0.1% of the largest dimension

    // ------------------------------------------------------------
    // Step 2: Decide grid resolution from target vertex count
    // ------------------------------------------------------------
    float srcVertexCountF     = static_cast<float>(srcVertices.size());
    float targetVertexCountF  = srcVertexCountF * reductionFactor;
    if (targetVertexCountF < 3.0f) {
        targetVertexCountF = 3.0f;
    }

    float cellsPerAxisF = glm::pow(targetVertexCountF, 1.0f / 3.0f);
    if (cellsPerAxisF < 2.0f) {
        cellsPerAxisF = 2.0f; // at least 2 cells per axis so we preserve min/max
    }

    unsigned int cellsPerAxis = static_cast<unsigned int>(cellsPerAxisF);
    if (cellsPerAxis < 2u) {
        cellsPerAxis = 2u;
    }

    // ------------------------------------------------------------
    // Step 3: Cluster vertices into grid cells, track boundary flags
    // ------------------------------------------------------------
    struct Cluster {
        glm::vec3 positionSum;
        glm::vec3 normalSum;
        glm::vec3 colorSum;
        glm::vec2 uvSum;
        unsigned int count;

        bool hasMinX;
        bool hasMaxX;
        bool hasMinY;
        bool hasMaxY;
        bool hasMinZ;
        bool hasMaxZ;

        Cluster()
            : positionSum(0.0f)
            , normalSum(0.0f)
            , colorSum(0.0f)
            , uvSum(0.0f)
            , count(0)
            , hasMinX(false)
            , hasMaxX(false)
            , hasMinY(false)
            , hasMaxY(false)
            , hasMinZ(false)
            , hasMaxZ(false)
        {}
    };

    std::unordered_map<std::uint64_t, unsigned int> cellToClusterIndex;
    std::vector<Cluster> clusterList;
    std::vector<unsigned int> vertexToCluster(srcVertices.size(), 0);

    clusterList.reserve(static_cast<size_t>(targetVertexCountF));

    for (size_t i = 0; i < srcVertices.size(); ++i) {
        const Vertex& v = srcVertices[i];

        glm::vec3 pos(v.x, v.y, v.z);
        glm::vec3 relative = (pos - minPos) / extent;

        // Clamp to [0,1]
        if (relative.x < 0.0f) relative.x = 0.0f;
        if (relative.y < 0.0f) relative.y = 0.0f;
        if (relative.z < 0.0f) relative.z = 0.0f;
        if (relative.x > 1.0f) relative.x = 1.0f;
        if (relative.y > 1.0f) relative.y = 1.0f;
        if (relative.z > 1.0f) relative.z = 1.0f;

        unsigned int ix = static_cast<unsigned int>(relative.x * static_cast<float>(cellsPerAxis - 1));
        unsigned int iy = static_cast<unsigned int>(relative.y * static_cast<float>(cellsPerAxis - 1));
        unsigned int iz = static_cast<unsigned int>(relative.z * static_cast<float>(cellsPerAxis - 1));

        std::uint64_t key = 0;
        key |= static_cast<std::uint64_t>(ix);
        key |= static_cast<std::uint64_t>(iy) << 21;
        key |= static_cast<std::uint64_t>(iz) << 42;

        std::unordered_map<std::uint64_t, unsigned int>::iterator it =
            cellToClusterIndex.find(key);

        unsigned int clusterIndex;
        if (it == cellToClusterIndex.end()) {
            clusterList.push_back(Cluster());
            clusterIndex = static_cast<unsigned int>(clusterList.size() - 1);
            cellToClusterIndex[key] = clusterIndex;
        } else {
            clusterIndex = it->second;
        }

        Cluster& cluster = clusterList[clusterIndex];

        cluster.positionSum += pos;
        cluster.normalSum   += glm::vec3(v.nx, v.ny, v.nz);
        cluster.colorSum    += glm::vec3(v.r, v.g, v.b);
        cluster.uvSum       += glm::vec2(v.u, v.v);
        cluster.count++;

        if (glm::abs(pos.x - minPos.x) <= boundaryEps) cluster.hasMinX = true;
        if (glm::abs(pos.x - maxPos.x) <= boundaryEps) cluster.hasMaxX = true;
        if (glm::abs(pos.y - minPos.y) <= boundaryEps) cluster.hasMinY = true;
        if (glm::abs(pos.y - maxPos.y) <= boundaryEps) cluster.hasMaxY = true;
        if (glm::abs(pos.z - minPos.z) <= boundaryEps) cluster.hasMinZ = true;
        if (glm::abs(pos.z - maxPos.z) <= boundaryEps) cluster.hasMaxZ = true;

        vertexToCluster[i] = clusterIndex;
    }

    if (clusterList.empty()) {
        return false;
    }

    // ------------------------------------------------------------
    // Step 4: Build new vertex buffer from clusters, snapping bounds
    // ------------------------------------------------------------
    const size_t newVertexCount = clusterList.size();
    outVertices.resize(newVertexCount);

    for (size_t ci = 0; ci < newVertexCount; ++ci) {
        const Cluster& cluster = clusterList[ci];
        float invCount = 1.0f / static_cast<float>(cluster.count);

        glm::vec3 avgPos    = cluster.positionSum * invCount;
        glm::vec3 avgColor  = cluster.colorSum * invCount;
        glm::vec3 avgNormal = cluster.normalSum * invCount;
        glm::vec2 avgUv     = cluster.uvSum * invCount;

        if (glm::length(avgNormal) > epsilon) {
            avgNormal = glm::normalize(avgNormal);
        } else {
            avgNormal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        // Snap to global bounds if this cluster contains boundary verts
        if (cluster.hasMinX && !cluster.hasMaxX) {
            avgPos.x = minPos.x;
        } else if (cluster.hasMaxX && !cluster.hasMinX) {
            avgPos.x = maxPos.x;
        }
        if (cluster.hasMinY && !cluster.hasMaxY) {
            avgPos.y = minPos.y;
        } else if (cluster.hasMaxY && !cluster.hasMinY) {
            avgPos.y = maxPos.y;
        }
        if (cluster.hasMinZ && !cluster.hasMaxZ) {
            avgPos.z = minPos.z;
        } else if (cluster.hasMaxZ && !cluster.hasMinZ) {
            avgPos.z = maxPos.z;
        }

        Vertex v;
        v.x  = avgPos.x;
        v.y  = avgPos.y;
        v.z  = avgPos.z;

        v.r  = avgColor.x;
        v.g  = avgColor.y;
        v.b  = avgColor.z;

        v.nx = avgNormal.x;
        v.ny = avgNormal.y;
        v.nz = avgNormal.z;

        v.u  = avgUv.x;
        v.v  = avgUv.y;

        outVertices[ci] = v;
    }

    // ------------------------------------------------------------
    // Step 5: Rebuild index buffer using cluster indices
    // ------------------------------------------------------------
    outIndices.reserve(srcIndices.size());

    for (size_t i = 0; i + 2 < srcIndices.size(); i += 3) {
        unsigned int i0 = srcIndices[i + 0].index;
        unsigned int i1 = srcIndices[i + 1].index;
        unsigned int i2 = srcIndices[i + 2].index;

        if (i0 >= srcVertices.size() || i1 >= srcVertices.size() || i2 >= srcVertices.size()) {
            continue;
        }

        unsigned int c0 = vertexToCluster[i0];
        unsigned int c1 = vertexToCluster[i1];
        unsigned int c2 = vertexToCluster[i2];

        // Skip degenerate triangles after clustering
        if (c0 == c1 || c1 == c2 || c0 == c2) {
            continue;
        }

        Index outI0;
        Index outI1;
        Index outI2;

        outI0.index = c0;
        outI1.index = c1;
        outI2.index = c2;

        outIndices.push_back(outI0);
        outIndices.push_back(outI1);
        outIndices.push_back(outI2);
    }

    if (outIndices.size() < 3) {
        outVertices.clear();
        outIndices.clear();
        return false;
    }

    return true;
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

unsigned int Mesh::RemoveDegenerateTriangles(SubMesh& submesh, float epsilon) {
    // Nothing to do if we don't have enough data for even a single triangle
    if (submesh.vertexBuffer.empty() || submesh.indexBuffer.size() < 3) {
        return 0;
    }
    
    const unsigned int vertexCount = static_cast<unsigned int>(submesh.vertexBuffer.size());
    const unsigned int indexCount  = static_cast<unsigned int>(submesh.indexBuffer.size());
    
    std::vector<Index> cleaned;
    cleaned.reserve(indexCount);
    
    unsigned int removedCount = 0;
    const float epsilonSq = epsilon * epsilon;
    
    for (unsigned int i = 0; i + 2 < indexCount; i += 3) {
        
        unsigned int i0 = submesh.indexBuffer[i + 0].index;
        unsigned int i1 = submesh.indexBuffer[i + 1].index;
        unsigned int i2 = submesh.indexBuffer[i + 2].index;
        
        bool invalid = false;
        
        // Basic index range and duplicate-index checks
        if (i0 >= vertexCount || i1 >= vertexCount || i2 >= vertexCount) {
            invalid = true;
        }
        if (i0 == i1 || i1 == i2 || i0 == i2) {
            invalid = true;
        }
        
        // If still potentially valid, test geometry
        if (!invalid) {
            const Vertex& v0 = submesh.vertexBuffer[i0];
            const Vertex& v1 = submesh.vertexBuffer[i1];
            const Vertex& v2 = submesh.vertexBuffer[i2];
            
            glm::vec3 p0(v0.x, v0.y, v0.z);
            glm::vec3 p1(v1.x, v1.y, v1.z);
            glm::vec3 p2(v2.x, v2.y, v2.z);
            
            glm::vec3 e0 = p1 - p0;
            glm::vec3 e1 = p2 - p0;
            glm::vec3 e2 = p2 - p1;
            
            // Any nearly-zero edge length -> degenerate
            float d0 = glm::dot(e0, e0);
            float d1 = glm::dot(e1, e1);
            float d2 = glm::dot(e2, e2);
            
            if (d0 <= epsilonSq || d1 <= epsilonSq || d2 <= epsilonSq) {
                invalid = true;
            } else {
                // Check area via cross product – near-zero area => degenerate
                glm::vec3 n = glm::cross(e0, e1);
                float area2Sq = glm::dot(n, n);  // proportional to squared area
            
                if (area2Sq <= epsilonSq) {
                    invalid = true;
                }
            }
        }
        
        if (invalid) {
            removedCount++;
            continue;
        }
        
        // Keep this triangle
        cleaned.push_back(submesh.indexBuffer[i + 0]);
        cleaned.push_back(submesh.indexBuffer[i + 1]);
        cleaned.push_back(submesh.indexBuffer[i + 2]);
    }
    
    if (removedCount > 0) {
        submesh.indexBuffer.swap(cleaned);
        submesh.indexCount = static_cast<unsigned int>(submesh.indexBuffer.size());
    }
    
    return removedCount;
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
