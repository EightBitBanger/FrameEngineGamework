#ifndef RESOURCE_SHADER_TAG
#define RESOURCE_SHADER_TAG

#include "../../Renderer/RenderSystem.h"

struct ShaderTag {
    
    std::string name;
    
    std::string vertexScript;
    std::string fragmentScript;
    
    ShaderTag();
    
};

#endif
