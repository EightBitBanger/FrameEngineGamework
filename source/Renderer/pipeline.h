#ifndef RENDER_PIPELINE_SYSTEM_COMPONENT
#define RENDER_PIPELINE_SYSTEM_COMPONENT

#include "components/shader.h"

class RenderPipeline {
    
public:
    
    void SetShader(Shader* newShader);
    
    RenderPipeline();
    
    void Bind(void);
    
private:
    
    Shader* currentShader;
    
};

#endif
