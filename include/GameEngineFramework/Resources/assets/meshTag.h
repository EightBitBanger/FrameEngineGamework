#ifndef RESOURCE_MESH_TAG
#define RESOURCE_MESH_TAG

#include "../../Renderer/RenderSystem.h"

struct ENGINE_API MeshTag {
    
    /// Resource path path.
    std::string path;
    
    /// Mesh resource name.
    std::string name;
    
    /// Is the resource loaded into memory.
    bool isLoaded;
    
    /// Mesh vertex and index data.
    std::vector<SubMesh> subMeshes;
    
    /// Load the data to which this asset points.
    bool Load(void);
    
    /// Frees the memory associated with this asset.
    bool Unload(void);
    
    MeshTag();
    
};

#endif
