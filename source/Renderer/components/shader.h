#ifndef __COMPONENT_SHADER
#define __COMPONENT_SHADER

#include <string>

#include <iostream>

#define GLEW_STATIC
#include "../../../vendor/gl/glew.h"

#include "../types/transform.h"

#include "../types/color.h"


class Shader {
    
public:
    
    Shader();
    ~Shader();
    
    /// Set the uniform model matrix.
    void SetModelMatrix(glm::mat4 &ModelMatrix);
    /// Set the uniform projection matrix.
    void SetProjectionMatrix(glm::mat4 &projectionMatrix);
    
    /// Set the color of the material.
    void SetMaterialColor(Color color);
    /// Set the texture sampler index.
    void SetTextureSampler(unsigned int index);
    
    /// Set default uniform locations.
    void SetUniformLocations(void);
    
    /// Compile a vertex and fragment script into a shader program.
    int CreateShaderProgram(std::string VertexScript, std::string FragmentScript);
    
    /// Bind the shader program for rendering.
    void Bind(void);
    
    /// Build a default shader program.
    bool BuildDefault(void);
    
    
private:
    
    unsigned int mShaderProgram;
    
    // Uniform locations
    int mProjectionMatrixLocation;
    int mModelMatrixLocation;
    int mMaterialColorLocation;
    int mSamplerLocation;
    
    bool  mIsShaderLoaded;
    
    unsigned int CompileSource(unsigned int Type, std::string Script);
    
};


#endif
