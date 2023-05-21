#include "shader.h"

Shader::Shader() {
    mShaderProgram = 0;
    
    mProjectionMatrixLocation=0;
    mModelMatrixLocation=0;
    mCameraPosition=0;
    mMaterialColorLocation=0;
    mSamplerLocation=0;
    mLightCount=0;
    mLightPosition=0;
    mLightAttenuation=0;
    
    mIsShaderLoaded = false;
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

void Shader::SetMaterialColor(Color color) {
    glUniform4f(mMaterialColorLocation, color.r, color.g, color.b, color.a);
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

void Shader::SetUniformLocations(void) {
    
    std::string projUniformName     = "u_proj";
    std::string modelUniformName    = "u_model";
    std::string eyeUniformName      = "u_eye";
    std::string colorUniformName    = "m_color";
    std::string samplerUniformName  = "u_sampler";
    std::string lightCountUniformName        = "u_light_count";
    std::string lightPositionUniformName     = "u_light_position";
    std::string lightAttenuationUniformName  = "u_light_attenuation";
    
    // Model projection
    mProjectionMatrixLocation  = glGetUniformLocation(mShaderProgram, projUniformName.c_str());;
    mModelMatrixLocation       = glGetUniformLocation(mShaderProgram, modelUniformName.c_str());
    mCameraPosition            = glGetUniformLocation(mShaderProgram, eyeUniformName.c_str());
    // Material
    mMaterialColorLocation     = glGetUniformLocation(mShaderProgram, colorUniformName.c_str());
    mSamplerLocation           = glGetUniformLocation(mShaderProgram, samplerUniformName.c_str());
    // Lighting
    mLightCount                = glGetUniformLocation(mShaderProgram, lightCountUniformName.c_str());
    mLightPosition             = glGetUniformLocation(mShaderProgram, lightPositionUniformName.c_str());
    mLightAttenuation          = glGetUniformLocation(mShaderProgram, lightAttenuationUniformName.c_str());
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
        assert(0);
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
        assert(0);
        return 0;
    }
    
    return ShaderID;
}

void Shader::Bind(void) {
    glUseProgram(mShaderProgram);
    
}

bool Shader::BuildDefault(void) {
    
    std::string vertexShader = 
        "#version 330 core\n"
        ""
        "layout(location = 0) in vec3 l_position;"
        "layout(location = 1) in vec3 l_color;"
        "layout(location = 2) in vec3 l_normal;"
        "layout(location = 3) in vec2 l_uv;"
        ""
        "uniform mat4 u_proj;"
        "uniform mat4 u_model;"
        "uniform vec3 u_eye;"
        ""
        "varying vec2 v_coord;"
        "varying vec3 v_color;"
        ""
        "uniform int   u_light_count;"
        "uniform vec3  u_light_position["+IntToString(RENDER_NUMBER_OF_LIGHTS)+"];"
        "uniform vec3  u_light_attenuation["+IntToString(RENDER_NUMBER_OF_LIGHTS)+"];"
        ""
        "void main() "
        "{"
        "  vec4 modelPosition = u_model * vec4(l_position, 1);"
        "  "
        "  "
        "  vec3 finalColor = vec3(0.01, 0.01, 0.01);"
        "  for (int i=0; i<=u_light_count; i++) {"
        "    "
        "    float intensity = u_light_attenuation[i].r;"
        "    float range     = u_light_attenuation[i].g;"
        "    "
        //"    vec3 lightDir = normalize(u_light_position[i] - vec3(modelPosition));"
        "    "
        //"    float diff = max(dot(l_normal, lightDir), 0.0);"
        "    "
        "    "
        "    float dist = length( u_light_position[i] - vec3(modelPosition) );"
        "    if (dist < range) finalColor = vec3(1, 1, 1);"
        "    "
        "    "
        "  }"
        "  "
        "  v_color = finalColor;"
        "  v_coord = l_uv;"
        "  "
        "  gl_Position = u_proj * modelPosition;"
        "  "
        "};";
    
    std::string fragmentShader = 
        "#version 330 core\n"
        ""
        "uniform vec4 m_color;"
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
