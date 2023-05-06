#ifndef RENDER_PIPELINE_SYSTEM_COMPONENT
#define RENDER_PIPELINE_SYSTEM_COMPONENT

#include "components/shader.h"

struct RenderPipeline {
    
    RenderPipeline();
    
    void Bind(void);
    
    Shader* currentShader;
    
};

#endif
