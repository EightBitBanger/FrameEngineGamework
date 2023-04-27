#ifndef COMPONENT_MESH
#define COMPONENT_MESH

#define GLEW_STATIC
#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../types/bufferlayout.h"
#include "../types/transform.h"
#include "../types/color.h"
#include "../types/viewport.h"

#include "../components/material.h"
#include "../components/shader.h"

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
    
public:
    
    Shader* shader;
    
    Mesh();
    ~Mesh();
    
    /// Reallocate new GPU memory.
    void Reallocate(unsigned int newBufferSize);
    
    /// Set the primitive drawing type for the vertex data.
    void SetPrimitive(int primitiveType);
    
    /// Set and enable a vertex attribute layout.
    void SetAttribute(int index, int attributeCount, int vertexSize, int byteOffset);
    /// Disable a vertex attribute.
    void DisableAttribute(int index);
    
    /// Load vertex buffer data onto the GPU.
    void LoadVertexBuffer(Vertex* bufferData, int vertexCount);
    /// Load index buffer data onto the GPU.
    void LoadIndexBuffer(Index* bufferData, int indexCount);
    
    /// Bind the mesh buffer for rendering.
    void Bind(void);
    /// Run a draw call on this vertex buffer.
    void DrawVertexArray(void);
    /// Run a draw call on this index buffer.
    void DrawIndexArray(void);
    
    /// Return the number of sub meshes in this vertex buffer.
    unsigned int GetSubMeshCount(void);
    
    /// Add a plain sub mesh to the vertex buffer.
    void AddPlain(float x, float y, float z, float width, float height, Color color);
    /// Add a sub divided plain sub mesh to the vertex buffer.
    void AddPlainSubDivided(float x, float y, float z, float width, float height, Color color, unsigned int widthSub, unsigned int heightSub);
    
    /// Add a wall sub mesh to the vertex buffer.
    void AddWall(float x, float y, float z, float width, float height, Color color);
    /// Add a sub divided wall sub mesh to the vertex buffer.
    void AddWallSubDivided(float x, float y, float z, float width, float height, Color color, unsigned int widthSub, unsigned int heightSub);
    
    /// Add a sub mesh into this vertex buffer.
    bool AddSubMesh(float x, float y, float z, SubMesh& mesh);
    /// Add a vertex buffer directly into this vertex buffer.
    bool AddSubMesh(float x, float y, float z, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer);
    
    /// Remove a sub mesh from this vertex buffer.
    bool RemoveSubMesh(unsigned int index);
    /// Copy this vertex buffer into a sub mesh.
    bool CopySubMesh(unsigned int index, SubMesh& mesh);
    /// Copy this vertex buffer directly into a vertex buffer.
    bool CopySubMesh(unsigned int index, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer);
    
    /// Update the color of a sub mesh.
    bool ChangeSubMeshColor(unsigned int index, Color newColor);
    /// Update the position of a sub mesh.
    bool ChangeSubMeshPosition(unsigned int index, float x, float y, float z);
    
    /// Fully re upload the vertex buffer into the GPU buffer.
    void UpdateMesh(void);
    
    
private:
    
    unsigned int vertexArray;
    unsigned int bufferVertex;
    unsigned int bufferIndex;
    
    int primitive;
    
    unsigned int vertexBufferSz;
    unsigned int indexBufferSz;
    unsigned int maxSize;
    
    std::vector<Vertex>   vertexBuffer;
    std::vector<Index>    indexBuffer;
    
    std::vector<SubMesh> subMesh;
    std::vector<SubMesh> freeMesh;
    
    void SetDefaultAttributes(void);
    
    void AllocateBuffers(unsigned int maxBufferSize);
    void FreeBuffers(void);
    
};



#endif
