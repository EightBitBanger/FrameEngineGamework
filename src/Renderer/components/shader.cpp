#include <GameEngineFramework/Renderer/components/shader.h>

#define GLEW_STATIC
#include <gl/glew.h>

#include <iostream>


Shader::Shader() : 
    mShaderProgram(0),
    
    mProjectionMatrixLocation(0),
    mModelMatrixLocation(0),
    mShadowMatrixLocation(0),
    mModelInvMatrixLocation(0),
    
    mCameraPosition(0),
    mCameraAngle(0),
    
    mMaterialAmbientLocation(0),
    mMaterialDiffuseLocation(0),
    mMaterialSpecularLocation(0),
    mSamplerLocation(0),
    
    mFogDensityLocation{0, 1, 2, 3},
    mFogBeginColorLocation{0, 1, 2, 3},
    mFogEndColorLocation{0, 1, 2, 3},
    mFogBeginLocation{0, 1, 2, 3},
    mFogEndLocation{0, 1, 2, 3},
    
    mLightCount(0),
    mLightPosition(0),
    mLightDirection(0),
    mLightAttenuation(0),
    mLightColor(0),
    
    mIsShaderLoaded(false)
{
}

Shader::~Shader() {
    if (mIsShaderLoaded) glDeleteProgram(mShaderProgram);
    return;
}

void Shader::SetModelMatrix(glm::mat4 &ModelMatrix) {
    glUniformMatrix4fv(mModelMatrixLocation, 1, GL_FALSE, &ModelMatrix[0][0]);
    return;
}

void Shader::SetInverseModelMatrix(glm::mat3 &InverseModelMatrix) {
    glUniformMatrix3fv(mModelInvMatrixLocation, 1, GL_FALSE, &InverseModelMatrix[0][0]);
    return;
}

void Shader::SetShadowMatrix(glm::mat4 &shadowMatrix) {
    glUniformMatrix4fv(mShadowMatrixLocation, 1, GL_FALSE, &shadowMatrix[0][0]);
    return;
}

void Shader::SetProjectionMatrix(glm::mat4 &projectionMatrix) {
    glUniformMatrix4fv(mProjectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
    return;
}

void Shader::SetCameraPosition(glm::vec3 cameraPosition) {
    glUniform3f(mCameraPosition, cameraPosition.x, cameraPosition.y, cameraPosition.z);
    return;
}

void Shader::SetCameraAngle(glm::vec3 cameraAngle) {
    glUniform3f(mCameraAngle, cameraAngle.x, cameraAngle.y, cameraAngle.z);
    return;
}

void Shader::SetMaterialAmbient(Color color) {
    glUniform3f(mMaterialAmbientLocation, color.r, color.g, color.b);
    return;
}

void Shader::SetMaterialDiffuse(Color color) {
    glUniform3f(mMaterialDiffuseLocation, color.r, color.g, color.b);
    return;
}

void Shader::SetMaterialSpecular(Color color) {
    glUniform3f(mMaterialSpecularLocation, color.r, color.g, color.b);
    return;
}

void Shader::SetTextureSampler(unsigned int index) {
    glUniform1i(mSamplerLocation, index);
    return;
}


void Shader::SetFogDensity(unsigned int index, float density) {
    glUniform1f(mFogDensityLocation[index], density);
    return;
}

void Shader::SetFogHeightCutoff(unsigned int index, float height) {
    glUniform1f(mFogHeightCutoffLocation[index], height);
    return;
}

void Shader::SetFogBegin(unsigned int index, float begin) {
    glUniform1f(mFogBeginLocation[index], begin);
    return;
}

void Shader::SetFogEnd(unsigned int index, float end) {
    glUniform1f(mFogEndLocation[index], end);
    return;
}

void Shader::SetFogColorBegin(unsigned int index, Color color) {
    glUniform3f(mFogBeginColorLocation[index], color.r, color.g, color.b);
    return;
}

void Shader::SetFogColorEnd(unsigned int index, Color color) {
    glUniform3f(mFogEndColorLocation[index], color.r, color.g, color.b);
    return;
}


void Shader::SetLightCount(unsigned int numberOfLights) {
    glUniform1i(mLightCount, numberOfLights);
    return;
}

void Shader::SetLightPositions(unsigned int numberOfLights, glm::vec3* lightPositions) {
    glUniform3fv(mLightPosition, numberOfLights, &lightPositions[0][0]);
    return;
}

void Shader::SetLightDirections(unsigned int numberOfLights, glm::vec3* lightDirections) {
    glUniform3fv(mLightDirection, numberOfLights, &lightDirections[0][0]);
    return;
}

void Shader::SetLightAttenuation(unsigned int numberOfLights, glm::vec4* lightAttenuation) {
    glUniform4fv(mLightAttenuation, numberOfLights, &lightAttenuation[0][0]);
    return;
}

void Shader::SetLightColors(unsigned int numberOfLights, glm::vec3* lightColors) {
    glUniform3fv(mLightColor, numberOfLights, &lightColors[0][0]);
    return;
}

void Shader::SetUniformLocations(void) {
    
    std::string projUniformName         = "u_proj";
    std::string modelUniformName        = "u_model";
    std::string shadowUniformName       = "u_shadow";
    std::string modelInvUniformName     = "u_inv_model";
    std::string eyeUniformName          = "u_eye";
    std::string angleUniformName        = "u_angle";
    
    std::string matAmbientUniformName   = "m_ambient";
    std::string matDiffuseUniformName   = "m_diffuse";
    std::string matSpecularUniformName  = "m_specular";
    
    std::string samplerUniformName      = "u_sampler";
    
    std::string fogDensityName[4];
    std::string fogHeightCutoffName[4];
    std::string fogBeginColorName[4];
    std::string fogEndColorName[4];
    std::string fogBeginName[4];
    std::string fogEndName[4];
    
    fogDensityName[0]                        = "u_fogDensity1";
    fogHeightCutoffName[0]                   = "u_fogCutoffHeight1";
    fogBeginColorName[0]                     = "u_fogStartColor1";
    fogEndColorName[0]                       = "u_fogEndColor1";
    fogBeginName[0]                          = "u_fogStart1";
    fogEndName[0]                            = "u_fogEnd1";
    
    fogDensityName[1]                        = "u_fogDensity2";
    fogHeightCutoffName[1]                   = "u_fogCutoffHeight2";
    fogBeginColorName[1]                     = "u_fogStartColor2";
    fogEndColorName[1]                       = "u_fogEndColor2";
    fogBeginName[1]                          = "u_fogStart2";
    fogEndName[1]                            = "u_fogEnd2";
    
    fogDensityName[2]                        = "u_fogDensity3";
    fogHeightCutoffName[2]                   = "u_fogCutoffHeight3";
    fogBeginColorName[2]                     = "u_fogStartColor3";
    fogEndColorName[2]                       = "u_fogEndColor3";
    fogBeginName[2]                          = "u_fogStart3";
    fogEndName[2]                            = "u_fogEnd3";
    
    fogDensityName[3]                        = "u_fogDensity4";
    fogHeightCutoffName[3]                   = "u_fogCutoffHeight4";
    fogBeginColorName[3]                     = "u_fogStartColor4";
    fogEndColorName[3]                       = "u_fogEndColor4";
    fogBeginName[3]                          = "u_fogStart4";
    fogEndName[3]                            = "u_fogEnd4";
    
    std::string lightCountUniformName        = "u_light_count";
    std::string lightPositionUniformName     = "u_light_position";
    std::string lightDirectionUniformName    = "u_light_direction";
    std::string lightAttenuationUniformName  = "u_light_attenuation";
    std::string lightColorUniformName        = "u_light_color";
    
    
    // Model projection
    mProjectionMatrixLocation  = glGetUniformLocation(mShaderProgram, projUniformName.c_str());;
    mModelMatrixLocation       = glGetUniformLocation(mShaderProgram, modelUniformName.c_str());
    mShadowMatrixLocation      = glGetUniformLocation(mShaderProgram, shadowUniformName.c_str());
    mModelInvMatrixLocation    = glGetUniformLocation(mShaderProgram, modelInvUniformName.c_str());
    mCameraPosition            = glGetUniformLocation(mShaderProgram, eyeUniformName.c_str());
    mCameraAngle               = glGetUniformLocation(mShaderProgram, angleUniformName.c_str());
    
    // Material
    mMaterialAmbientLocation   = glGetUniformLocation(mShaderProgram, matAmbientUniformName.c_str());
    mMaterialDiffuseLocation   = glGetUniformLocation(mShaderProgram, matDiffuseUniformName.c_str());
    mMaterialSpecularLocation  = glGetUniformLocation(mShaderProgram, matSpecularUniformName.c_str());
    mSamplerLocation           = glGetUniformLocation(mShaderProgram, samplerUniformName.c_str());
    
    // Fog layers
    for (unsigned int i=0; i < 4; i++) {
        
        mFogDensityLocation[i]       = glGetUniformLocation(mShaderProgram, fogDensityName[i].c_str());
        
        mFogHeightCutoffLocation[i]  = glGetUniformLocation(mShaderProgram, fogHeightCutoffName[i].c_str());
        
        mFogBeginColorLocation[i]    = glGetUniformLocation(mShaderProgram, fogBeginColorName[i].c_str()); 
        mFogEndColorLocation[i]      = glGetUniformLocation(mShaderProgram, fogEndColorName[i].c_str());
        
        mFogBeginLocation[i]         = glGetUniformLocation(mShaderProgram, fogBeginName[i].c_str());
        mFogEndLocation[i]           = glGetUniformLocation(mShaderProgram, fogEndName[i].c_str());
        
    }
    
    // Lighting
    mLightCount                = glGetUniformLocation(mShaderProgram, lightCountUniformName.c_str());
    mLightPosition             = glGetUniformLocation(mShaderProgram, lightPositionUniformName.c_str());
    mLightDirection            = glGetUniformLocation(mShaderProgram, lightDirectionUniformName.c_str());
    mLightAttenuation          = glGetUniformLocation(mShaderProgram, lightAttenuationUniformName.c_str());
    mLightColor                = glGetUniformLocation(mShaderProgram, lightColorUniformName.c_str());
    
    return;
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
        std::cout << " ! Shader link error\n";
        return 0;
    }
    
    SetUniformLocations();
    
    mIsShaderLoaded = true;
    return 1;
}

unsigned int Shader::GetProgram(void) {
    return mShaderProgram;
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
        std::cout << "\n ! Shader compilation error\n\n\n";
        return 0;
    }
    
    return ShaderID;
}

void Shader::Bind(void) {
    glUseProgram(mShaderProgram);
    return;
}


