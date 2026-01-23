#ifndef __COMPONENT_SHADER
#define __COMPONENT_SHADER

#include <GameEngineFramework/Engine/types/color.h>
#include <GameEngineFramework/Transform/Transform.h>

#include <string>


class ENGINE_API Shader {
    
public:
    
    /// Set the uniform model matrix.
    void SetModelMatrix(glm::mat4 &ModelMatrix);
    
    /// Set the uniform inverse model matrix.
    void SetInverseModelMatrix(glm::mat3 &InverseModelMatrix);
    
    /// Set the uniform shadow matrix.
    void SetShadowMatrix(glm::mat4 &shadowMatrix);
    
    /// Set the uniform projection matrix.
    void SetProjectionMatrix(glm::mat4 &projectionMatrix);
    
    
    /// Set the uniform camera position.
    void SetCameraPosition(glm::vec3 cameraPosition);
    
    /// Set the uniform camera looking angle.
    void SetCameraAngle(glm::vec3 cameraAngle);
    
    
    /// Set the uniform ambient color of the material.
    void SetMaterialAmbient(Color color);
    
    /// Set the uniform diffuse color of the material.
    void SetMaterialDiffuse(Color color);
    
    /// Set the uniform specular color of the material.
    void SetMaterialSpecular(Color color);
    
    
    /// Set the number of textures to be used.
    void SetTextureSamplerCount(unsigned int numberOfTextures);
    
    /// Set the uniform texture sampler index.
    void SetTextureSampler(unsigned int index);
    
    
    /// Set the number of fog layers to be drawn.
    void SetFogCount(int numberOfFogLayers);
    
    /// Set the density for the fog effect.
    void SetFogDensity(unsigned int numberOfLayers, float* density);
    
    /// Set the height cutoff for the fog effect.
    void SetFogHeightCutoff(unsigned int numberOfLayers, float* height);
    
    /// Set the distance to begin rendering fog.
    void SetFogBegin(unsigned int numberOfLayers, float* begin);
    
    /// Set the distance to stop rendering fog.
    void SetFogEnd(unsigned int numberOfLayers, float* end);
    
    /// Set the color at the start of the fog.
    void SetFogColorBegin(unsigned int numberOfLayers, glm::vec3* color);
    
    /// Set the fog at the end of the fog.
    void SetFogColorEnd(unsigned int numberOfLayers, glm::vec3* color);
    
    
    /// Set the number of uniform lights.
    void SetLightCount(unsigned int numberOfLights);
    
    /// Set the array of uniform light positions.
    void SetLightPositions(unsigned int numberOfLights, glm::vec3* lightPositions);
    
    /// Set the array of uniform light directions.
    void SetLightDirections(unsigned int numberOfLights, glm::vec3* lightDirections);
    
    /// Set the array of uniform light attenuation.
    void SetLightAttenuation(unsigned int numberOfLights, glm::vec4* lightAttenuation);
    
    /// Set the array of uniform light colors.
    void SetLightColors(unsigned int numberOfLights, glm::vec3* lightColors);
    
    
    /// Set default uniform locations.
    void SetUniformLocations(void);
    
    /// Compile a vertex and fragment script into a shader program.
    int CreateShaderProgram(std::string VertexScript, std::string FragmentScript);
    
    /// Return the opengl index of the shader program.
    unsigned int GetProgram(void);
    
    /// Bind the shader program for rendering.
    void Bind(void);
    
    
    friend class RenderSystem;
    
    Shader();
    ~Shader();
    
    
private:
    
    unsigned int mShaderProgram;
    
    // Uniform locations
    int mProjectionMatrixLocation;
    int mModelMatrixLocation;
    int mShadowMatrixLocation;
    int mModelInvMatrixLocation;
    
    int mCameraPosition;
    int mCameraAngle;
    
    int mMaterialColorLocation;
    int mMaterialAmbientLocation;
    int mMaterialDiffuseLocation;
    int mMaterialSpecularLocation;
    
    int mSamplerCountLocation;
    int mSamplerLocation;
    
    int mFogCountLocation;
    int mFogDensityLocation;
    int mFogHeightCutoffLocation;
    int mFogBeginColorLocation;
    int mFogEndColorLocation;
    int mFogBeginLocation;
    int mFogEndLocation;
    
    int mLightCount;
    int mLightPosition;
    int mLightDirection;
    int mLightAttenuation;
    int mLightColor;
    
    bool  mIsShaderLoaded;
    
    unsigned int CompileSource(unsigned int Type, std::string Script);
    
};


#endif
