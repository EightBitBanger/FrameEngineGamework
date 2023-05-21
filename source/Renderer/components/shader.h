#ifndef __COMPONENT_SHADER
#define __COMPONENT_SHADER

#include <string>

#include <iostream>

#define GLEW_STATIC
#include <gl/glew.h>

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
    /// Set the uniform camera position.
    void SetCameraPosition(glm::vec3 cameraPosition);
    
    /// Set the color of the material.
    void SetMaterialColor(Color color);
    /// Set the texture sampler index.
    void SetTextureSampler(unsigned int index);
    
    /// Set the number of lights.
    void SetLightCount(unsigned int numberOfLights);
    /// Set the array of light positions.
    void SetLightPositions(unsigned int numberOfLights, glm::vec3* lightPositions);
    /// Set the array of light attenuation.
    void SetLightIntensity(unsigned int numberOfLights, float* lightIntensity);
    
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
    int mCameraPosition;
    int mMaterialColorLocation;
    int mSamplerLocation;
    int mLightCount;
    int mLightPosition;
    int mLightIntensity;
    
    bool  mIsShaderLoaded;
    
    unsigned int CompileSource(unsigned int Type, std::string Script);
    
};


#endif
