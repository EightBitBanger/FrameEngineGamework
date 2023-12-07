#ifndef RESOURCE_ALLOCATION_SYSTEM
#define RESOURCE_ALLOCATION_SYSTEM

#include <GameEngineFramework/Renderer/RenderSystem.h>
#include <GameEngineFramework/Physics/PhysicsSystem.h>

#include <GameEngineFramework/Resources/assets/colliderTag.h>
#include <GameEngineFramework/Resources/assets/textureTag.h>
#include <GameEngineFramework/Resources/assets/shaderTag.h>
#include <GameEngineFramework/Resources/assets/MeshTag.h>

#include <GameEngineFramework/Resources/FileSystem.h>

#include <thread>
#include <mutex>

class ENGINE_API ResourceManager {
    
public:
    
    /// Main rendering scene.
    Scene* sceneMain;
    
    /// Prepare the loading system.
    void Initiate(void);
    
    /// Load a wavefront model file and assign it a resource tag name.
    bool LoadWaveFront(std::string path, std::string resourceName, bool loadImmediately=false);
    /// Load a texture image file and assign it a resource tag name.
    bool LoadTexture(std::string path, std::string resourceName, bool loadImmediately=false);
    /// Load a GLSL shader file and assign it a resource tag name.
    bool LoadShaderGLSL(std::string path, std::string resourceName, bool loadImmediately=false);
    
    /// Destroy and deallocate a mesh model resource by its name.
    bool UnloadMeshTag(std::string resourceName);
    /// Destroy and deallocate a texture image resource by its name.
    bool UnloadTextureTag(std::string resourceName);
    /// Destroy and deallocate a shader program resource by its name.
    bool UnloadShaderTag(std::string resourceName);
    
    /// Create a render mesh object from a mesh resource tag.
    Mesh* CreateMeshFromTag(std::string resourceName);
    /// Create a material object from a texture image resource tag.
    Material* CreateMaterialFromTag(std::string resourceName);
    /// Create a shader object from a GLSL shader resource tag.
    Shader* CreateShaderFromTag(std::string resourceName);
    /// Create a physics collision shape from a collider resource tag.
    rp3d::BoxShape* CreateColliderFromTag(std::string resourceName);
    
    /// Purge all assets loaded in the resource system.
    void DestroyAssets(void);
    
    ResourceManager();
    
private:
    
    // Find a resource by its tag name.
    MeshTag* FindMeshTag(std::string resourceName);
    TextureTag* FindTextureTag(std::string resourceName);
    ShaderTag* FindShaderTag(std::string resourceName);
    ColliderTag* FindColliderTag(std::string resourceName);
    
    // Thread loading queue
    std::vector<std::string> streamQueue;
    
    // Asset lists
    std::vector<MeshTag>      mMeshTags;
    std::vector<TextureTag>   mTextureTags;
    std::vector<ShaderTag>    mShaderTags;
    std::vector<ColliderTag>  mColliderTags;
    
};



#endif
