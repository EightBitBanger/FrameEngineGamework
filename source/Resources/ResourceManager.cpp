#include "ResourceManager.h"

#include "plugins.h"

extern RenderSystem Renderer;
extern PhysicsSystem Physics;

ResourceManager::ResourceManager() :
    
    sceneMain(nullptr)
{
}

void ResourceManager::Initiate(void) {
    
    sceneMain = Renderer.CreateScene();
    Renderer.AddSceneToRenderQueue(sceneMain);
    
    stbi_set_flip_vertically_on_load(true);
    
    // Load resource directories if they exist
    std::vector<std::string> shaderDirectoryList = DirectoryGetList(".\\core\\shaders\\");
    std::vector<std::string> modelDirectoryList = DirectoryGetList(".\\core\\models\\");
    std::vector<std::string> materialDirectoryList = DirectoryGetList(".\\core\\materials\\");
    
    for (int i=0; i < shaderDirectoryList.size(); i++) 
        LoadShaderGLSL("core/shaders/" + shaderDirectoryList[i], StringGetNameFromFilenameNoExt( shaderDirectoryList[i] ));
    
    for (int i=0; i < modelDirectoryList.size(); i++) 
        LoadWaveFront("core/models/" + modelDirectoryList[i], StringGetNameFromFilenameNoExt( modelDirectoryList[i] ));
    
    for (int i=0; i < materialDirectoryList.size(); i++) 
        LoadTexture("core/materials/" + materialDirectoryList[i], StringGetNameFromFilenameNoExt( materialDirectoryList[i] ));
    
    return;
}

unsigned char* ResourceManager::LoadImageRaw(char const* path, int* width, int* height, int* channels, int req_channels) {
    return stbi_load(path, width, height, channels, req_channels);
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
    Mesh* meshPtr = Renderer.CreateMesh();
    meshPtr->AddSubMesh(0,0,0, meshTag->mesh.vertexBuffer, meshTag->mesh.indexBuffer);
    return meshPtr;
}

Material* ResourceManager::CreateMaterialFromTag(std::string resourceName) {
    TextureTag* texTag = FindTextureTag(resourceName);
    if (texTag == nullptr) return nullptr;
    Material* materialPtr = Renderer.CreateMaterial();
    materialPtr->width  = texTag->width;
    materialPtr->height = texTag->height;
    materialPtr->UpdateTextureBuffer(texTag->buffer);
    return materialPtr;
}

Shader* ResourceManager::CreateShaderFromTag(std::string resourceName) {
    ShaderTag* shaderTag = FindShaderTag(resourceName);
    if (shaderTag == nullptr) return nullptr;
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
            stbi_image_free(it->buffer);
    
    return;
}

bool ResourceManager::LoadTexture(std::string path, std::string resourceName="texDefault") {
    
    std::string name       = StringGetNameFromFilename(path);
    std::string assetName  = StringGetNameFromFilenameNoExt(path);
    
    if (resourceName != "") 
        assetName = resourceName;
    
    TextureTag textureTag;
    textureTag.name = assetName;
    
    textureTag.buffer = LoadImageRaw(path.c_str(), &textureTag.width, &textureTag.height, &textureTag.channels, 3);
    assert(textureTag.buffer != nullptr);
    
    mTextureTags.push_back(textureTag);
    
    std::string logstr = "  + " + assetName + "  " + path;
    Log.Write(logstr);
    return true;
}

bool ResourceManager::LoadWaveFront(std::string path, std::string resourceName="meshDefault") {
    
    objl::Loader loader;
    if (!loader.LoadFile(path)) {
        Log.Write("!! File not found - " + path); Log.WriteLn();
    }
    
    unsigned int numberOfMeshes = loader.LoadedMeshes.size();
    if (numberOfMeshes == 0) return false;
    
    MeshTag newAsset;
    newAsset.name = loader.LoadedMeshes[0].MeshName;
    
    if (resourceName != "") 
        newAsset.name = resourceName;
    
    for (unsigned int i=0; i < loader.LoadedMeshes[0].Vertices.size(); i++) {
        
        objl::Vertex objlVertex = loader.LoadedMeshes[0].Vertices[i];
        
        Vertex vertex;
        vertex.x = objlVertex.Position.X;
        vertex.y = objlVertex.Position.Y;
        vertex.z = objlVertex.Position.Z;
        vertex.r = 1;
        vertex.g = 1;
        vertex.b = 1;
        vertex.nx = objlVertex.Normal.X;
        vertex.ny = objlVertex.Normal.Y;
        vertex.nz = objlVertex.Normal.Z;
        vertex.u = objlVertex.TextureCoordinate.X;
        vertex.v = objlVertex.TextureCoordinate.Y;
        
        newAsset.mesh.vertexBuffer.push_back(vertex);
    }
    
    for (unsigned int i=0; i < loader.LoadedMeshes[0].Indices.size(); i++) {
        newAsset.mesh.indexBuffer.push_back(loader.LoadedMeshes[0].Indices[i]);
    }
    
    mMeshTags.push_back(newAsset);
    
    std::string logstr = "  + " + newAsset.name + "  " + path;
    Log.Write(logstr);
    
    return true;
}

bool ResourceManager::LoadShaderGLSL(std::string path, std::string resourceName="shaDefault") {
    
    FileLoader loader(path);
    if (!loader.CheckIsFileLoaded()) {
        std::string logstr = "! " + path;
        Log.Write(logstr);
        return false;
    }
    
    std::string name = StringGetNameFromFilenameNoExt(path);
    
    std::string vertex   = loader.GetDataBlockByName("vertex");
    std::string fragment = loader.GetDataBlockByName("fragment");
    
    ShaderTag newAsset;
    newAsset.name = name;
    if (resourceName != "") 
        newAsset.name = resourceName;
    
    newAsset.vertexScript   = vertex;
    newAsset.fragmentScript = fragment;
    
    mShaderTags.push_back(newAsset);
    
    std::string logstr = "  + " + newAsset.name + "  " + path;
    Log.Write(logstr);
    
    return true;
}

bool ResourceManager::UnloadMeshTag(std::string resourceName) {
    for (std::vector<MeshTag>::iterator it = mMeshTags.begin(); it != mMeshTags.end(); ++it) {
        if (it->name == resourceName) {
            mMeshTags.erase(it);
            return true;
        }
    }
    return false;
}

bool ResourceManager::UnloadTextureTag(std::string resourceName) {
    for (std::vector<TextureTag>::iterator it = mTextureTags.begin(); it != mTextureTags.end(); ++it) {
        if (it->name == resourceName) {
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
