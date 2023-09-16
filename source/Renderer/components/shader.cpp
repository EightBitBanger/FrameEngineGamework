#include "shader.h"

Shader::Shader() : 
    mShaderProgram(0),
    
    mProjectionMatrixLocation(0),
    mModelMatrixLocation(0),
    mCameraPosition(0),
    
    mMaterialAmbientLocation(0),
    mMaterialDiffuseLocation(0),
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
    
}

bool Shader::BuildDefault(void) {
    std::string numberOfLights = IntToString(RENDER_NUMBER_OF_LIGHTS);
    
    std::string vertexShader = 
        "#version 330 core\n"
        
        "layout(location = 0) in vec3 l_position;"
        "layout(location = 1) in vec3 l_color;"
        "layout(location = 2) in vec3 l_normal;"
        "layout(location = 3) in vec2 l_uv;"
        
        "uniform mat4 u_proj;"
        "uniform mat4 u_model;"
        "uniform vec3 u_eye;"
        
        "varying vec2 v_coord;"
        "varying vec3 v_color;"
        
        "uniform vec3 m_ambient;"
        "uniform vec3 m_diffuse;"
        
        "uniform int   u_light_count;"
        "uniform vec3  u_light_position["+numberOfLights+"];"
        "uniform vec3  u_light_attenuation["+numberOfLights+"];"
        "uniform vec3  u_light_color["+numberOfLights+"];"
        
        "float l_diff = 1.0;"
        "float l_dist = 1.0;"
        
        "void main() "
        "{"
        
        "  vec4 vertPos = u_model * vec4(l_position, 1);"
        "  vec3 finalColor = m_ambient;";
        
        
    //
    // Crude but quick vertex lighting implementation
    //
    
    vertexShader += 
        "  for (int i=0; i<=u_light_count; i++) {"
        
        "    float l_intensity    = u_light_attenuation[i].r;"
        "    float l_range        = u_light_attenuation[i].g;"
        "    float l_attenuation  = u_light_attenuation[i].b;"
        
        "    vec3 l_direction = normalize(u_light_position[i] - vec3(vertPos));"
        
        "    vec3 normal = normalize(l_normal);"
        
        "    l_diff = max(dot(normal, l_direction), 0.0);"
        "    l_dist = length( u_light_position[i] - vec3(vertPos));"
        
        "    if (l_dist < l_range) {"
        "      float f_attenuation = l_attenuation * l_range * l_dist;"
        "      finalColor += (l_diff * u_light_color[i] * l_intensity) / f_attenuation;"
        "    }"
        "  }";
    
    
    // Finalize vertex colors
    vertexShader += 
        "  v_color = finalColor + l_color + m_diffuse;"
        "  v_coord = l_uv;"
        "  gl_Position = u_proj * vertPos;"
        "};";
    
    
    std::string fragmentShader = 
        "#version 330 core\n"
        ""
        "varying vec3 v_color;"
        "varying vec2 v_coord;"
        ""
        "uniform sampler2D u_sampler;"
        ""
        "out vec4 color;"
        ""
        "void main()"
        "{"
        "  "
        "  float Gamma = 2.2;"
        "  "
        "  vec4 vColor = vec4(v_color, 1);"
        "  "
        "  color = vec4( pow(vColor.rgb, vec3(1.0/Gamma)), 1) * texture(u_sampler, v_coord);"
        "  "
        "}";
    
    if (CreateShaderProgram(vertexShader, fragmentShader) == 1) return true;
    
    return false;
}
