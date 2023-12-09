#ifndef __COMPONENT_SHADER
#define __COMPONENT_SHADER

#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Transform/Transform.h>

#include <string>


class ENGINE_API Shader {
    
public:
    
    friend class RenderSystem;
    
    Shader();
    ~Shader();
    
    /// Set the uniform model matrix.
    void SetModelMatrix(glm::mat4 &ModelMatrix);
    /// Set the uniform projection matrix.
    void SetProjectionMatrix(glm::mat4 &projectionMatrix);
    /// Set the uniform camera position.
    void SetCameraPosition(glm::vec3 cameraPosition);
    
    /// Set the ambient color of the material.
    void SetMaterialAmbient(Color color);
    /// Set the diffuse color of the material.
    void SetMaterialDiffuse(Color color);
    /// Set the specular color of the material.
    void SetMaterialSpecular(Color color);
    
    /// Set the texture sampler index.
    void SetTextureSampler(unsigned int index);
    
    /// Set the number of lights.
    void SetLightCount(unsigned int numberOfLights);
    /// Set the array of light positions.
    void SetLightPositions(unsigned int numberOfLights, glm::vec3* lightPositions);
    /// Set the array of light attenuation.
    void SetLightAttenuation(unsigned int numberOfLights, glm::vec3* lightAttenuation);
    /// Set the array of light colors.
    void SetLightColors(unsigned int numberOfLights, glm::vec3* lightColors);
    
    /// Set default uniform locations.
    void SetUniformLocations(void);
    
    /// Compile a vertex and fragment script into a shader program.
    int CreateShaderProgram(std::string VertexScript, std::string FragmentScript);
    
    /// Bind the shader program for rendering.
    void Bind(void);
    
    
private:
    
    unsigned int mShaderProgram;
    
    // Uniform locations
    int mProjectionMatrixLocation;
    int mModelMatrixLocation;
    int mCameraPosition;
    int mMaterialColorLocation;
    int mMaterialAmbientLocation;
    int mMaterialDiffuseLocation;
    int mMaterialSpecularLocation;
    int mSamplerLocation;
    int mLightCount;
    int mLightPosition;
    int mLightAttenuation;
    int mLightColor;
    
    bool  mIsShaderLoaded;
    
    unsigned int CompileSource(unsigned int Type, std::string Script);
    
};


#endif
