#ifndef RESOURCE_SHADER_TAG
#define RESOURCE_SHADER_TAG

#include "../../Renderer/RenderSystem.h"

struct ShaderTag {
    
    /// Shader resource name.
    std::string name;
    
    /// Vertex shader script.
    std::string vertexScript;
    
    /// Fragment shader script.
    std::string fragmentScript;
    
    ShaderTag();
    
};

#endif
