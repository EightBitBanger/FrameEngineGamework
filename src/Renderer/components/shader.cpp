#include "shader.h"

#define GLEW_STATIC
#include <gl/glew.h>

#include <iostream>

Shader::Shader() : 
    mShaderProgram(0),
    
    mProjectionMatrixLocation(0),
    mModelMatrixLocation(0),
    mCameraPosition(0),
    
    mMaterialAmbientLocation(0),
    mMaterialDiffuseLocation(0),
    mMaterialSpecularLocation(0),
    mSamplerLocation(0),
    
    mLightCount(0),
    mLightPosition(0),
    mLightAttenuation(0),
    mLightColor(0),
    
    mIsShaderLoaded(false)
{
}

Shader::~Shader() {
    
    if (mIsShaderLoaded) glDeleteProgram(mShaderProgram);
}

void Shader::SetModelMatrix(glm::mat4 &ModelMatrix) {
    glUniformMatrix4fv(mModelMatrixLocation, 1, 0, &ModelMatrix[0][0]);
}

void Shader::SetProjectionMatrix(glm::mat4 &projectionMatrix) {
    glUniformMatrix4fv(mProjectionMatrixLocation, 1, 0, &projectionMatrix[0][0]);
}

void Shader::SetCameraPosition(glm::vec3 cameraPosition) {
    glUniform3f(mCameraPosition, cameraPosition.x, cameraPosition.y, cameraPosition.z);
}

void Shader::SetMaterialAmbient(Color color) {
    glUniform3f(mMaterialAmbientLocation, color.r, color.g, color.b);
}

void Shader::SetMaterialDiffuse(Color color) {
    glUniform3f(mMaterialDiffuseLocation, color.r, color.g, color.b);
}

void Shader::SetMaterialSpecular(Color color) {
    glUniform3f(mMaterialSpecularLocation, color.r, color.g, color.b);
}

void Shader::SetTextureSampler(unsigned int index) {
    glUniform1i(mSamplerLocation, index);
}

void Shader::SetLightCount(unsigned int numberOfLights) {
    glUniform1i(mLightCount, numberOfLights);
}

void Shader::SetLightPositions(unsigned int numberOfLights, glm::vec3* lightPositions) {
    glUniform3fv(mLightPosition, numberOfLights, &lightPositions[0][0]);
}

void Shader::SetLightAttenuation(unsigned int numberOfLights, glm::vec3* lightAttenuation) {
    glUniform3fv(mLightAttenuation, numberOfLights, &lightAttenuation[0][0]);
}

void Shader::SetLightColors(unsigned int numberOfLights, glm::vec3* lightColors) {
    glUniform3fv(mLightColor, numberOfLights, &lightColors[0][0]);
}

void Shader::SetUniformLocations(void) {
    
    std::string projUniformName         = "u_proj";
    std::string modelUniformName        = "u_model";
    std::string eyeUniformName          = "u_eye";
    std::string matAmbientUniformName   = "m_ambient";
    std::string matDiffuseUniformName   = "m_diffuse";
    std::string matSpecularUniformName  = "m_specular";
    std::string samplerUniformName      = "u_sampler";
    std::string lightCountUniformName        = "u_light_count";
    std::string lightPositionUniformName     = "u_light_position";
    std::string lightAttenuationUniformName  = "u_light_attenuation";
    std::string lightColorUniformName        = "u_light_color";
    
    // Model projection
    mProjectionMatrixLocation  = glGetUniformLocation(mShaderProgram, projUniformName.c_str());;
    mModelMatrixLocation       = glGetUniformLocation(mShaderProgram, modelUniformName.c_str());
    mCameraPosition            = glGetUniformLocation(mShaderProgram, eyeUniformName.c_str());
    // Material
    mMaterialAmbientLocation   = glGetUniformLocation(mShaderProgram, matAmbientUniformName.c_str());
    mMaterialDiffuseLocation   = glGetUniformLocation(mShaderProgram, matDiffuseUniformName.c_str());
    mMaterialSpecularLocation  = glGetUniformLocation(mShaderProgram, matSpecularUniformName.c_str());
    mSamplerLocation           = glGetUniformLocation(mShaderProgram, samplerUniformName.c_str());
    // Lighting
    mLightCount                = glGetUniformLocation(mShaderProgram, lightCountUniformName.c_str());
    mLightPosition             = glGetUniformLocation(mShaderProgram, lightPositionUniformName.c_str());
    mLightAttenuation          = glGetUniformLocation(mShaderProgram, lightAttenuationUniformName.c_str());
    mLightColor                = glGetUniformLocation(mShaderProgram, lightColorUniformName.c_str());
}

int Shader::CreateShaderProgram(std::string VertexScript, std::string FragmentScript) {
    
    // Compile the scripts into a shader program
    unsigned int vs = CompileSource(GL_VERTEX_SHADER,   VertexScript);
    unsigned int fs = CompileSource(GL_FRAGMENT_SHADER, FragmentScript);
    
    if (vs==0) return -1;
    if (fs==0) return -2;
    
    mShaderProgram = glCreateProgram();
    
    glAttachShader(mShaderProgram, vs);
    glAttachShader(mShaderProgram, fs);
    
    glLinkProgram(mShaderProgram);
    
    glDetachShader(mShaderProgram, vs);
    glDetachShader(mShaderProgram, fs);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    GLint state;
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &state);
    if (state == GL_FALSE) {
        std::cout << " ! Shader link error" << std::endl;
        return 0;
    }
    
    SetUniformLocations();
    
    mIsShaderLoaded = true;
    return 1;
}

unsigned int Shader::CompileSource(unsigned int Type, std::string Script) {
    
    unsigned int ShaderID = glCreateShader(Type);
    const char*  SourceScript = Script.c_str();
    
    // Compile source script
    glShaderSource(ShaderID, 1, &SourceScript, nullptr);
    glCompileShader(ShaderID);
    
    // Check compile status
    int vResult;
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &vResult);
    if (!vResult) {
        std::cout << std::endl << " ! Shader compilation error" << std::endl << std::endl << std::endl;
        return 0;
    }
    
    return ShaderID;
}

void Shader::Bind(void) {
    glUseProgram(mShaderProgram);
    return;
}


