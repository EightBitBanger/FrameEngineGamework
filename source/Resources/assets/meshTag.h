#ifndef RESOURCE_MESH_TAG
#define RESOURCE_MESH_TAG

#include "../../Renderer/RenderSystem.h"

struct __declspec(dllexport) MeshTag {
    
    /// Resource path name.
    std::string path;
    
    /// Mesh resource name.
    std::string name;
    
    /// Is the resource loaded into memory.
    bool isLoaded;
    
    /// Mesh vertex and index data.
    SubMesh mesh;
    
    /// Load the data to which this asset points.
    bool Load(void);
    
    /// Frees the memory associated with this asset.
    bool Unload(void);
    
    MeshTag();
    
};

#endif
