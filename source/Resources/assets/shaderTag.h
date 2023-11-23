#ifndef RESOURCE_SHADER_TAG
#define RESOURCE_SHADER_TAG

#include "../../Renderer/RenderSystem.h"

struct __declspec(dllexport) ShaderTag {
    
    /// Shader resource name.
    std::string name;
    
    /// Is the resource loaded into memory.
    bool isLoaded;
    
    /// Vertex shader script.
    std::string vertexScript;
    
    /// Fragment shader script.
    std::string fragmentScript;
    
    ShaderTag();
    
};

#endif
