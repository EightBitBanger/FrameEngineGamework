#ifndef __SUB_MESH
#define __SUB_MESH

#include <GameEngineFramework/configuration.h>

#include <GameEngineFramework/engine/types/bufferlayout.h>

#include <GameEngineFramework/engine/types/color.h>
#include <GameEngineFramework/Math/Math.h>
#include <GameEngineFramework/Math/Random.h>

#include <vector>
#include <string>

extern NumberGeneration Random;


struct ENGINE_API SubMesh {
    
    /// Name of the sub mesh.
    std::string name;
    
    /// Starting offset position in the vertex buffer.
    unsigned vertexBegin;
    
    /// Number of vertices in the vertex buffer.
    unsigned vertexCount;
    
    /// Starting offset position in the index buffer.
    unsigned indexBegin;
    
    /// Number of indices in the index buffer.
    unsigned indexCount;
    
    /// Current mesh position offset.
    glm::vec3 position;
    
    /// Vertex buffer array.
    std::vector<Vertex>  vertexBuffer;
    
    /// Index buffer array.
    std::vector<Index>   indexBuffer;
    
    SubMesh() : 
        name(""),
        vertexBegin(0),
        vertexCount(0),
        indexBegin(0),
        indexCount(0),
        position(glm::vec3(0, 0, 0))
    {
    }
    
};

#endif
