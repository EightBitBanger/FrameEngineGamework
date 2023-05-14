#include "pipeline.h"


RenderPipeline::RenderPipeline() {
    
    currentShader = nullptr;
    return;
}

void RenderPipeline::Bind(void) {
    assert(currentShader != nullptr);
    currentShader->Bind();
    return;
}
