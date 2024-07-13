#include <GameEngineFramework/MemoryAllocation/PoolAllocator.h>
#include <GameEngineFramework/Resources/ResourceManager.h>
#include <GameEngineFramework/Types/Types.h>

extern RenderSystem   Renderer;
extern PhysicsSystem  Physics;
extern FileSystemDir  Directory;
extern StringType     String;
extern Logger         Log;

ResourceManager::ResourceManager() {
    return;
}

void ResourceManager::Initiate(void) {
    
    Log.Write("Resource definitions");
    Log.WriteLn();
    
    // Load resource directories if they exist
    std::vector<std::string> shaderDirectoryList   = Directory.GetList(".\\core\\shaders\\");
    std::vector<std::string> modelDirectoryList    = Directory.GetList(".\\core\\models\\");
    std::vector<std::string> materialDirectoryList = Directory.GetList(".\\core\\materials\\");
    
    for (unsigned int i=0; i < shaderDirectoryList.size(); i++) 
        LoadShaderGLSL("core/shaders/" + shaderDirectoryList[i], String.GetNameFromFilenameNoExt( shaderDirectoryList[i] ));
    
    for (unsigned int i=0; i < modelDirectoryList.size(); i++) 
        LoadWaveFront("core/models/" + modelDirectoryList[i], String.GetNameFromFilenameNoExt( modelDirectoryList[i] ));
    
    for (unsigned int i=0; i < materialDirectoryList.size(); i++) 
        LoadTexture("core/materials/" + materialDirectoryList[i], String.GetNameFromFilenameNoExt( materialDirectoryList[i] ));
    
    return;
}

MeshTag* ResourceManager::FindMeshTag(std::string resourceName) {
    for (std::vector<MeshTag>::iterator it = mMeshTags.begin(); it != mMeshTags.end(); ++it) 
        if (it->name == resourceName) 
            return &it[0];
    return nullptr;
}

TextureTag* ResourceManager::FindTextureTag(std::string resourceName) {
    for (std::vector<TextureTag>::iterator it = mTextureTags.begin(); it != mTextureTags.end(); ++it) 
        if (it->name == resourceName) 
            return &it[0];
    return nullptr;
}

ShaderTag* ResourceManager::FindShaderTag(std::string resourceName) {
    for (std::vector<ShaderTag>::iterator it = mShaderTags.begin(); it != mShaderTags.end(); ++it) 
        if (it->name == resourceName) 
            return &it[0];
    return nullptr;
}

ColliderTag* ResourceManager::FindColliderTag(std::string resourceName) {
    for (std::vector<ColliderTag>::iterator it = mColliderTags.begin(); it != mColliderTags.end(); ++it) 
        if (it->name == resourceName) 
            return &it[0];
    return nullptr;
}

Mesh* ResourceManager::CreateMeshFromTag(std::string resourceName) {
    MeshTag* meshTag = FindMeshTag(resourceName);
    if (meshTag == nullptr) return nullptr;
    if (!meshTag->isLoaded) 
        if (!meshTag->Load()) 
            return nullptr;
    Mesh* meshPtr = Renderer.CreateMesh();
    for (unsigned int i=0; i < meshTag->subMeshes.size(); i++) 
        meshPtr->AddSubMesh(0, 0, 0, meshTag->subMeshes[i].vertexBuffer, meshTag->subMeshes[i].indexBuffer, false);
    meshPtr->Load();
    return meshPtr;
}

Material* ResourceManager::CreateMaterialFromTag(std::string resourceName) {
    TextureTag* texTag = FindTextureTag(resourceName);
    if (texTag == nullptr) return nullptr;
    if (!texTag->isLoaded) 
        texTag->Load();
    Material* materialPtr = Renderer.CreateMaterial();
    materialPtr->texture.UploadTextureToGPU(texTag->buffer, texTag->width, texTag->height, texTag->filtration);
    return materialPtr;
}

Shader* ResourceManager::CreateShaderFromTag(std::string resourceName) {
    ShaderTag* shaderTag = FindShaderTag(resourceName);
    if (shaderTag == nullptr) return nullptr;
    if (!shaderTag->isLoaded) 
        shaderTag->Load();
    Shader* shaderPtr = Renderer.CreateShader();
    shaderPtr->CreateShaderProgram(shaderTag->vertexScript, shaderTag->fragmentScript);
    return shaderPtr;
}

rp3d::BoxShape* ResourceManager::CreateColliderFromTag(std::string resourceName) {
    for (std::vector<ColliderTag>::iterator it = mColliderTags.begin(); it != mColliderTags.end(); ++it) 
        if (it->name == resourceName) 
            return it->colliderShape;
    return nullptr;
}

void ResourceManager::DestroyAssets(void) {
    for (std::vector<TextureTag>::iterator it = mTextureTags.begin(); it != mTextureTags.end(); ++it) 
        if (it->buffer != nullptr) 
            it->Unload();
    
    return;
}

bool ResourceManager::LoadTexture(std::string path, std::string resourceName, bool loadImmediately) {
    
    TextureTag textureTag;
    
    textureTag.name = resourceName;
    textureTag.path = path;
    
    if (loadImmediately) 
        textureTag.Load();
    
    mTextureTags.push_back(textureTag);
    
    std::string logstr = "  + " + resourceName + "  " + path;
    Log.Write(logstr);
    
    return true;
}

bool ResourceManager::LoadWaveFront(std::string path, std::string resourceName, bool loadImmediately) {
    
    // should probably load sub meshes as well
    //newAsset.name = loader.LoadedMeshes[ i ].MeshName;
    
    MeshTag newAsset;
    
    newAsset.name = resourceName;
    newAsset.path = path;
    
    if (loadImmediately) 
        newAsset.Load();
    
    mMeshTags.push_back(newAsset);
    
    std::string logstr = "  + " + newAsset.name + "  " + path;
    Log.Write(logstr);
    
    return true;
}

bool ResourceManager::LoadShaderGLSL(std::string path, std::string resourceName, bool loadImmediately) {
    
    ShaderTag newAsset;
    newAsset.name = resourceName;
    newAsset.path = path;
    
    if (loadImmediately) 
        newAsset.Load();
    
    mShaderTags.push_back(newAsset);
    
    std::string logstr = "  + " + newAsset.name + "  " + path;
    Log.Write(logstr);
    
    return true;
}

bool ResourceManager::UnloadMeshTag(std::string resourceName) {
    for (std::vector<MeshTag>::iterator it = mMeshTags.begin(); it != mMeshTags.end(); ++it) {
        if (it->name == resourceName) {
            if (it->isLoaded) 
                it->Unload();
            mMeshTags.erase(it);
            return true;
        }
    }
    return false;
}

bool ResourceManager::UnloadTextureTag(std::string resourceName) {
    for (std::vector<TextureTag>::iterator it = mTextureTags.begin(); it != mTextureTags.end(); ++it) {
        if (it->name == resourceName) {
            if (it->isLoaded) 
                it->Unload();
            mTextureTags.erase(it);
            return true;
        }
    }
    return false;
}

bool ResourceManager::UnloadShaderTag(std::string resourceName) {
    for (std::vector<ShaderTag>::iterator it = mShaderTags.begin(); it != mShaderTags.end(); ++it) {
        if (it->name == resourceName) {
            mShaderTags.erase(it);
            return true;
        }
    }
    return false;
}
