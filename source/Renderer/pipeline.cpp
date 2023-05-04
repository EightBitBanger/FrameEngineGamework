#include "pipeline.h"


RenderPipeline::RenderPipeline() {
    
    return;
}


void RenderPipeline::SetShader(Shader* newShader) {
    assert(newShader != nullptr);
    currentShader = newShader;
    return;
}

void RenderPipeline::Bind(void) {
    
    currentShader->Bind();
}
