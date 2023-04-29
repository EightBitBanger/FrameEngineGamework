#ifndef RESOURCE_ALLOCATION_SYSTEM
#define RESOURCE_ALLOCATION_SYSTEM

#include "../Renderer/RenderSystem.h"
#include "../Physics/PhysicsSystem.h"

#include "assets/meshTag.h"
#include "assets/textureTag.h"
#include "assets/shaderTag.h"
#include "assets/colliderTag.h"


class ResourceManager {
    
public:
    /// Main rendering scene.
    Scene* sceneMain;
    
    /// Prepare the loading system.
    void Initiate(void);
    
    /// Load a wavefront model file and assign it a resource tag name.
    bool LoadWaveFront(std::string path, std::string resourceName);
    /// Load a texture image file and assign it a resource tag name.
    bool LoadTexture(std::string path, std::string resourceName);
    /// Load a GLSL shader file and assign it a resource tag name.
    bool LoadShaderGLSL(std::string path, std::string resourceName);
    
    /// Find a mesh resource by its resource tag name.
    MeshTag*     FindMeshTag(std::string name);
    /// Find a texture resource by its resource tag name.
    TextureTag*  FindTextureTag(std::string name);
    /// Find a shader resource by its resource tag name.
    ShaderTag*   FindShaderTag(std::string name);
    /// Find a resource by its resource tag name.
    ColliderTag* FindColliderTag(std::string name);
    
    /// Destroy and deallocate a mesh model resource by its name.
    bool UnloadMeshTag(std::string name);
    /// Destroy and deallocate a texture image resource by its name.
    bool UnloadTextureTag(std::string name);
    /// Destroy and deallocate a shader program resource by its name.
    bool UnloadShaderTag(std::string name);
    
    /// Get a physics collision shape from a collider resource tag.
    rp3d::BoxShape* GetColliderFromTag(std::string name);
    
    /// Create a render mesh object from a mesh resource tag.
    Mesh*     CreateMeshFromTag(std::string name);
    /// Create a material object from a texture image resource tag.
    Material* CreateMaterialFromTag(std::string name);
    /// Create a shader object from a GLSL shader resource tag.
    Shader*   CreateShaderFromTag(std::string name);
    
    /// Load a ".scene" file. Scene files construct scenes on load from definition and location files.
    bool LoadScene(std::string path);
    /// Definition files initiate and define resources for use within the engine.
    bool LoadDefinitions(std::string path);
    /// Locations files place and structure objects within the scene.
    bool LoadLocations(std::string path);
    
    /// Purge all assets loaded in the resource system.
    void DestroyAssets(void);
    
    
private:
    
    // Asset lists
    std::vector<MeshTag>      meshTags;
    std::vector<TextureTag>   textureTags;
    std::vector<ShaderTag>    shaderTags;
    std::vector<ColliderTag>  colliderTags;
    
    // Raw image data loader
    unsigned char* LoadImageRaw(char const* path, int* width, int* height, int* channels, int req_channels);
    
};



#endif
