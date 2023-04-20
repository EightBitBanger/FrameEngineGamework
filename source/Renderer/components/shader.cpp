#include "shader.h"
extern Logger Log;

Shader::Shader() {
    shaderProgram = 0;
    
    projectionMatrixLocation = 0;
    modelMatrixLocation = 0;
    materialColorLocation = 0;
    samplerLocation = 0;
    
    isShaderLoaded = false;
}

Shader::~Shader() {
    
    if (isShaderLoaded) glDeleteProgram(shaderProgram);
}


void Shader::SetModelMatrix(glm::mat4 &ModelMatrix) {glUniformMatrix4fv(modelMatrixLocation, 1, 0, &ModelMatrix[0][0]);}
void Shader::SetProjectionMatrix(glm::mat4 &projectionMatrix) {glUniformMatrix4fv(projectionMatrixLocation, 1, 0, &projectionMatrix[0][0]);}

void Shader::SetMaterialColor(Color color) {glUniform4f(materialColorLocation, color.r, color.g, color.b, color.a);}

void Shader::SetTextureSampler(unsigned int index) {glUniform1i(samplerLocation, index);}


void Shader::SetUniformLocations(void) {
    
    std::string projUniformName     = "u_proj";
    std::string modelUniformName    = "u_model";
    std::string colorUniformName    = "m_color";
    std::string samplerUniformName  = "u_sampler";
    
    projectionMatrixLocation  = glGetUniformLocation(shaderProgram, projUniformName.c_str());;
    modelMatrixLocation       = glGetUniformLocation(shaderProgram, modelUniformName.c_str());
    materialColorLocation     = glGetUniformLocation(shaderProgram, colorUniformName.c_str());
    samplerLocation           = glGetUniformLocation(shaderProgram, samplerUniformName.c_str());
    
}

int Shader::CreateShaderProgram(std::string VertexScript, std::string FragmentScript) {
    
    // Compile the scripts into a shader program
    unsigned int vs = CompileSource(GL_VERTEX_SHADER,   VertexScript);
    unsigned int fs = CompileSource(GL_FRAGMENT_SHADER, FragmentScript);
    
    if (vs==0) return -1;
    if (fs==0) return -2;
    
    shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    
    glLinkProgram(shaderProgram);
    
    glDetachShader(shaderProgram, vs);
    glDetachShader(shaderProgram, fs);
    
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    GLint state;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &state);
    if (state == GL_FALSE) {
        Log.WriteLn();
        Log.Write(" ! Shader link error");
        Log.WriteLn();
        return 0;
    }
    
    SetUniformLocations();
    
    isShaderLoaded = true;
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
        Log.WriteLn();
        Log.Write(" ! Shader compilation error");
        Log.WriteLn();
        return 0;
    }
    
    return ShaderID;
}


void Shader::Bind(void) {
    glUseProgram(shaderProgram);
}


bool Shader::BuildDefault(void) {
    
    std::string vertexShader = 
        "#version 330 core\n"
        ""
        "layout(location = 0) in vec3 l_position;"
        "layout(location = 1) in vec3 l_color;"
        "layout(location = 2) in vec2 l_uv;"
        ""
        "uniform mat4 u_proj;"
        "uniform mat4 u_model;"
        ""
        "varying vec2 v_coord;"
        "varying vec3 v_color;"
        ""
        "void main() "
        "{"
        ""
        "v_color = l_color;"
        "v_coord = l_uv;"
        ""
        "gl_Position = u_proj * u_model * vec4(l_position, 1);"
        ""
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


