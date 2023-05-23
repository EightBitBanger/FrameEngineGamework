#include "pipeline.h"


RenderPipeline::RenderPipeline() : 
    
    currentShader(nullptr)
{
}

void RenderPipeline::Bind(void) {
    assert(currentShader != nullptr);
    currentShader->Bind();
    return;
}
