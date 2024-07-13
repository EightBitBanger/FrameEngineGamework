#ifndef __COMPONENT_MESH
#define __COMPONENT_MESH

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/engine/types/bufferlayout.h>

#include <GameEngineFramework/engine/types/color.h>
#include <GameEngineFramework/Math/Math.h>
#include <GameEngineFramework/Math/Random.h>
#include <GameEngineFramework/Renderer/components/submesh.h>

#include <vector>
#include <string>

extern NumberGeneration Random;



class ENGINE_API Mesh {
    
public:
    
    /// Add a plain sub mesh to the vertex buffer.
    void AddPlain(float x, float y, float z, float width, float height, Color color, float uCoord=1, float vCoord=1, float uStart=0, float vStart=0, unsigned int uOffset=0, unsigned int vOffset=0);
    
    /// Add a sub divided plain sub mesh to the vertex buffer.
    void AddPlainSubDivided(float x, float y, float z, float width, float height, Color color, unsigned int widthSub, unsigned int heightSub);
    
    /// Add a wall sub mesh to the vertex buffer.
    void AddWall(float x, float y, float z, float width, float height, Color color);
    
    /// Add a sub divided wall sub mesh to the vertex buffer.
    void AddWallSubDivided(float x, float y, float z, float width, float height, Color color, unsigned int widthSub, unsigned int heightSub);
    
    /// Add a quad sub mesh to the vertex buffer.
    int AddQuad(float x, float y, float z, float width, float height, Color color);
    
    
    /// Add a sub mesh into this vertex buffer. The index of the sub mesh in the mesh will be returned. A return value of negative one indicated an error.
    int AddSubMesh(float x, float y, float z, SubMesh& mesh, bool doUploadToGpu=true);
    
    /// Add a vertex buffer directly into this vertex buffer. The index of the sub mesh in the mesh will be returned. A return value of negative one indicated an error.
    int AddSubMesh(float x, float y, float z, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer, bool doUploadToGpu=true);
    
    /// Remove a sub mesh from this vertex buffer.
    bool RemoveSubMesh(unsigned int index);
    
    /// Get the vertex and index buffer as a sub mesh.
    bool GetSubMesh(unsigned int index, SubMesh& mesh);
    
    /// Get the vertex and index buffer directly as arrays.
    bool GetSubMesh(unsigned int index, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer);
    
    
    /// Update the color of a sub mesh.
    bool ChangeSubMeshColor(unsigned int index, Color newColor);
    
    /// Update the position of a sub mesh.
    bool ChangeSubMeshPosition(unsigned int index, float x, float y, float z);
    
    /// Update the scale of a sub mesh.
    bool ChangeSubMeshPoints(unsigned int index, std::vector<glm::vec3> points);
    
    /// Clear all sub meshes in the mesh.
    void ClearSubMeshes(void);
    
    
    /// Fully re-upload the vertex buffer onto the GPU.
    void Load(void);
    
    /// Purge the vertex buffer from the GPU.
    void Unload(void);
    
    /// Return whether the buffers are allocated on the GPU.
    bool CheckIsAllocatedOnGPU(void);
    
    
    /// Get the number of index locations in the index buffer.
    unsigned int GetNumberOfIndices(void);
    
    /// Get the number of vertex locations in the vertex buffer.
    unsigned int GetNumberOfVertices(void);
    
    /// Get a vertex from the internal vertex array.
    Vertex GetVertex(unsigned int index);
    
    /// Set a vertex to the internal vertex array.
    void SetVertex(unsigned int index, Vertex vertex);
    
    /// Get an index from the internal index array.
    Index GetIndex(unsigned int index);
    
    /// Set an index to the internal index array.
    void SetIndex(unsigned int index, Index position);
    
    
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
    
    /// Generate normals for the current vertex buffer.
    void CalculateNormals(void);
    
    /// Set normals to a default value for the current vertex buffer.
    void SetNormals(glm::vec3 normals);
    
    
    friend class RenderSystem;
    
    Mesh();
    ~Mesh();
    
    /// Is this mesh shared between multiple mesh renderers. (True to prevent garbage collection)
    bool isShared;
    
    
private:
    
    // OpenGL buffers
    unsigned int mVertexArray;
    unsigned int mBufferVertex;
    unsigned int mBufferIndex;
    
    // Render draw type
    int mPrimitive;
    
    // Buffer sizes
    unsigned int mVertexBufferSz;
    unsigned int mIndexBufferSz;
    unsigned int mMaxSize;
    
    bool mAreBuffersAllocated;
    
    // Vertex buffer array
    std::vector<Vertex>   mVertexBuffer;
    // Index buffer array
    std::vector<Index>    mIndexBuffer;
    
    // List of sub meshes in this mesh
    std::vector<SubMesh> mSubMesh;
    // List of freed sub meshes in this mesh
    std::vector<SubMesh> mFreeMesh;
    
    // Apply default vertex layout settings
    void SetDefaultAttributes(void);
    
    // Allocate/deallocate 
    void AllocateBuffers(unsigned int maxBufferSize);
    void FreeBuffers(void);
    
    // Reallocate new GPU memory.
    void Reallocate(unsigned int newBufferSize);
    
};



#endif
