#include "assets/meshTag.h"
#include "assets/textureTag.h"
#include "assets/shaderTag.h"


class ResourceManager {
    
    std::vector<MeshTag>      meshTags;
    std::vector<TextureTag>   textureTags;
    std::vector<ShaderTag>    shaderTags;
    
    unsigned char* LoadImageRaw(char const* filename, int* width, int* height, int* channels, int req_channels);
    
public:
    
    void Initiate(void);
    
    bool LoadWaveFront(std::string filename, std::string resourceName);
    bool LoadTexture(std::string filename, std::string resourceName);
    bool LoadShaderGLSL(std::string filename, std::string resourceName);
    
    MeshTag*    FindMeshTag(std::string name);
    TextureTag* FindTextureTag(std::string name);
    ShaderTag*  FindShaderTag(std::string name);
    
    bool UnloadMeshTag(std::string name);
    bool UnloadTextureTag(std::string name);
    bool UnloadShaderTag(std::string name);
    
    SubMesh    GetSubMeshFromTag(std::string name);
    
    Mesh*      CreateMeshFromTag(std::string name);
    Material*  CreateMaterialFromTag(std::string name);
    Shader*    CreateShaderFromTag(std::string name);
    
    bool LoadScene(std::string filename);
    bool LoadDefinitions(std::string filename);
    bool LoadLocations(std::string filename);
    
    
    
    
    void DestroyAssets(void);
    
};



