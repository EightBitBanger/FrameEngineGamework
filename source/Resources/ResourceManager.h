#include "assets/meshTag.h"
#include "assets/textureTag.h"
#include "assets/shaderTag.h"
#include "assets/colliderTag.h"


class ResourceManager {
    
public:
    
    void Initiate(void);
    
    bool LoadWaveFront(std::string path, std::string resourceName);
    bool LoadTexture(std::string path, std::string resourceName);
    bool LoadShaderGLSL(std::string path, std::string resourceName);
    
    MeshTag*     FindMeshTag(std::string name);
    TextureTag*  FindTextureTag(std::string name);
    ShaderTag*   FindShaderTag(std::string name);
    ColliderTag* FindColliderTag(std::string name);
    
    bool UnloadMeshTag(std::string name);
    bool UnloadTextureTag(std::string name);
    bool UnloadShaderTag(std::string name);
    
    rp3d::BoxShape* GetColliderFromTag(std::string name);
    
    Mesh*     CreateMeshFromTag(std::string name);
    Material* CreateMaterialFromTag(std::string name);
    Shader*   CreateShaderFromTag(std::string name);
    
    bool LoadScene(std::string path);
    bool LoadDefinitions(std::string path);
    bool LoadLocations(std::string path);
    
    void DestroyAssets(void);
    
    
private:
    
    std::vector<MeshTag>      meshTags;
    std::vector<TextureTag>   textureTags;
    std::vector<ShaderTag>    shaderTags;
    std::vector<ColliderTag>  colliderTags;
    
    unsigned char* LoadImageRaw(char const* path, int* width, int* height, int* channels, int req_channels);
    
};



