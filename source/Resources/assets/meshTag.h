#ifndef RESOURCE_MESH_TAG
#define RESOURCE_MESH_TAG

#include "../../Renderer/RenderSystem.h"

struct MeshTag {
    
    /// Mesh resource name.
    std::string name;
    
    /// Mesh vertex and index data.
    SubMesh mesh;
    
    MeshTag();
    
};

#endif
