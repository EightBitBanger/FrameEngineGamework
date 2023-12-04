#ifndef RESOURCE_SHADER_TAG
#define RESOURCE_SHADER_TAG

#include "../../Renderer/RenderSystem.h"

struct ENGINE_API ShaderTag {
    
    /// Shader resource path.
    std::string path;
    
    /// Shader resource name.
    std::string name;
    
    /// Is the resource loaded into memory.
    bool isLoaded;
    
    /// Vertex shader script.
    std::string vertexScript;
    
    /// Fragment shader script.
    std::string fragmentScript;
    
    /// Load the data to which this asset points.
    bool Load(void);
    
    /// Frees the memory associated with this texture.
    bool Unload(void);
    
    ShaderTag();
    
};

#endif
