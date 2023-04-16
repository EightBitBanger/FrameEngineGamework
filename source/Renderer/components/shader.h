#ifndef COMPONENT_SHADER
#define COMPONENT_SHADER

#include <string>

#define GLEW_STATIC
#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../types/color.h"

#include "../../std/logging.h"


class Shader {
    
    unsigned int shaderProgram;
    
    int projectionMatrixLocation;
    int modelMatrixLocation;
    
    int materialColorLocation;
    
    int samplerLocation;
    
    bool  isShaderLoaded;
    
    unsigned int CompileSource(unsigned int Type, std::string Script);
    
public:
    
    Shader();
    ~Shader();
    
    void SetModelMatrix(glm::mat4 &ModelMatrix);
    void SetProjectionMatrix(glm::mat4 &projectionMatrix);
    
    void SetMaterialColor(Color color);
    
    void SetTextureSampler(unsigned int index);
    
    void SetUniformLocations(void);
    
    int CreateShaderProgram(std::string VertexScript, std::string FragmentScript);
    
    void Bind(void);
    
    bool BuildDefault(void);
    
};


#endif
