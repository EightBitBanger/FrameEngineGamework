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
    
public:
    
    Shader* shader;
    
    Mesh();
    ~Mesh();
    
    void SetPrimitive(int primitiveType);
    
    void SetAttribute(int index, int attributeCount, int vertexSize, int byteOffset);
    void DisableAttribute(int index);
    void SetDefaultAttributes(void);
    
    void LoadVertexBuffer(Vertex* bufferData, int vertexCount);
    void LoadIndexBuffer(Index* bufferData, int indexCount);
    
    
    void Bind(void);
    
    void AllocateBuffers(unsigned int maxBufferSize);
    void FreeBuffers(void);
    
    void DrawVertexArray(void);
    void DrawIndexArray(void);
    
    
    unsigned int GetSubMeshCount(void);
    
    void AddPlain(float x, float y, float z, float width, float height, Color color);
    void AddPlainSubDivided(float x, float y, float z, float width, float height, Color color, unsigned int widthSub, unsigned int heightSub);
    
    void AddWall(float x, float y, float z, float width, float height, Color color);
    void AddWallSubDivided(float x, float y, float z, float width, float height, Color color, unsigned int widthSub, unsigned int heightSub);
    
    
    bool AddSubMesh(float x, float y, float z, SubMesh& mesh);
    bool AddSubMesh(float x, float y, float z, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer);
    
    
    bool RemoveSubMesh(unsigned int index);
    
    bool CopySubMesh(unsigned int index, SubMesh& mesh);
    bool CopySubMesh(unsigned int index, std::vector<Vertex>& vrtxBuffer, std::vector<Index>& indxBuffer);
    
    bool ChangeSubMeshColor(unsigned int index, Color newColor);
    bool ChangeSubMeshPosition(unsigned int index, float x, float y, float z);
    
    void UpdateMesh(void);
    
};



#endif
