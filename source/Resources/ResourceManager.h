#include "assets/meshTag.h"
#include "assets/textureTag.h"
#include "assets/shaderTag.h"


class ResourceManager {
    
    std::vector<MeshTag>      meshTags;
    std::vector<TextureTag>   textureTags;
    std::vector<ShaderTag>    shaderTags;
    
public:
    
    void Initiate(void);
    
    bool LoadWaveFront(std::string filename, std::string resourceName);
    bool LoadTexture(std::string filename, std::string resourceName);
    bool LoadShaderGLSL(std::string path, std::string resourceName);
    
    MeshTag*    FindMeshTag(std::string name);
    TextureTag* FindTextureTag(std::string name);
    ShaderTag*  FindShaderTag(std::string name);
    
    bool UnloadMeshTag(std::string name);
    bool UnloadTextureTag(std::string name);
    bool UnloadShaderTag(std::string name);
    
    Mesh*      CreateMeshFromTag(std::string name);
    Material*  CreateMaterialFromTag(std::string name);
    Shader*    CreateShaderFromTag(std::string name);
    
    void DestroyAssets(void);
    
};




void ResourceManager::Initiate(void) {
    
    stbi_set_flip_vertically_on_load(true);
    
}

MeshTag* ResourceManager::FindMeshTag(std::string name) {
    for (std::vector<MeshTag>::iterator it = meshTags.begin(); it != meshTags.end(); ++it) {
        if (it->name == name) 
            return &it[0];
    }
    return nullptr;
}

TextureTag* ResourceManager::FindTextureTag(std::string name) {
    for (std::vector<TextureTag>::iterator it = textureTags.begin(); it != textureTags.end(); ++it) {
        if (it->name == name) 
            return &it[0];
    }
    return nullptr;
}

ShaderTag* ResourceManager::FindShaderTag(std::string name) {
    for (std::vector<ShaderTag>::iterator it = shaderTags.begin(); it != shaderTags.end(); ++it) {
        if (it->name == name) 
            return &it[0];
    }
    return nullptr;
}





bool ResourceManager::UnloadMeshTag(std::string name) {
    for (std::vector<MeshTag>::iterator it = meshTags.begin(); it != meshTags.end(); ++it) {
        if (it->name == name) {
            meshTags.erase(it);
            return true;
        }
    }
    return false;
}

bool ResourceManager::UnloadTextureTag(std::string name) {
    for (std::vector<TextureTag>::iterator it = textureTags.begin(); it != textureTags.end(); ++it) {
        if (it->name == name) {
            textureTags.erase(it);
            return true;
        }
    }
    return false;
}

bool ResourceManager::UnloadShaderTag(std::string name) {
    for (std::vector<ShaderTag>::iterator it = shaderTags.begin(); it != shaderTags.end(); ++it) {
        if (it->name == name) {
            shaderTags.erase(it);
            return true;
        }
    }
    return false;
}



Mesh* ResourceManager::CreateMeshFromTag(std::string name) {
    MeshTag* meshTag = FindMeshTag(name);
    if (meshTag == nullptr) return nullptr;
    Mesh* meshPtr = Renderer.CreateMesh();
    meshPtr->AddSubMesh(0,0,0, meshTag->vertexBuffer, meshTag->indexBuffer);
    meshPtr->SetAttribute(0, 3, sizeof(Vertex), 0);
    meshPtr->SetAttribute(1, 3, sizeof(Vertex), 12);
    meshPtr->SetAttribute(2, 2, sizeof(Vertex), 24);
    return meshPtr;
}

Material* ResourceManager::CreateMaterialFromTag(std::string name) {
    TextureTag* texTag = FindTextureTag(name);
    if (texTag == nullptr) return nullptr;
    Material* materialPtr = Renderer.CreateMaterial();
    materialPtr->width  = texTag->width;
    materialPtr->height = texTag->height;
    materialPtr->UpdateTextureBuffer(texTag->buffer);
    return materialPtr;
}

Shader* ResourceManager::CreateShaderFromTag(std::string name) {
    ShaderTag* shaderTag = FindShaderTag(name);
    if (shaderTag == nullptr) return nullptr;
    Shader* shaderPtr = Renderer.CreateShader();
    shaderPtr->CreateShaderProgram(shaderTag->vertexScript, shaderTag->fragmentScript);
    return shaderPtr;
}




void ResourceManager::DestroyAssets(void) {
    
    for (std::vector<MeshTag>::iterator it = meshTags.begin(); it != meshTags.end(); ++it) 
        it->UnloadBuffers();
    
    for (std::vector<TextureTag>::iterator it = textureTags.begin(); it != textureTags.end(); ++it) 
        it->UnloadBuffers();
    
}












bool ResourceManager::LoadTexture(std::string path, std::string resourceName="") {
    
    std::string filename   = StringGetNameFromFilename(path);
    std::string assetName  = StringGetNameFromFilenameNoExt(filename);
    
    if (resourceName != "") 
        assetName = resourceName;
    
    TextureTag textureTag;
    textureTag.name = assetName;
    
    textureTag.buffer = stbi_load(path.c_str(), &textureTag.width, &textureTag.height, &textureTag.channels, 3);
    
    if (textureTag.buffer == nullptr) {
        std::string logstr = " ! " + path;
        Log.Write(logstr);
        std::cout << logstr << std::endl;
        return false;
    }
    
    textureTags.push_back(textureTag);
    
    std::string logstr = " + " + assetName + "  " + path;
    Log.Write(logstr);
    std::cout << logstr << std::endl;
    return true;
}


bool ResourceManager::LoadWaveFront(std::string path, std::string resourceName="") {
    
    objl::Loader loader;
    if (!loader.LoadFile(path)) {
        std::string logstr = " ! " + path;
        Log.Write(logstr);
        std::cout << logstr << std::endl;
        return false;
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
        vertex.u = objlVertex.TextureCoordinate.X;
        vertex.v = objlVertex.TextureCoordinate.Y;
        
        newAsset.vertexBuffer.push_back(vertex);
    }
    
    for (unsigned int i=0; i < loader.LoadedMeshes[0].Indices.size(); i++) {
        newAsset.indexBuffer.push_back(loader.LoadedMeshes[0].Indices[i]);
    }
    
    meshTags.push_back(newAsset);
    
    std::string logstr = " + " + newAsset.name + "  " + path;
    Log.Write(logstr);
    std::cout << logstr << std::endl;
    
    return true;
}


bool ResourceManager::LoadShaderGLSL(std::string path, std::string resourceName="") {
    
    FileLoader loader(path);
    if (!loader.CheckIsFileLoaded()) {
        std::string logstr = " ! " + path;
        Log.Write(logstr);
        std::cout << logstr << std::endl;
        return false;
    }
    
    std::string filename = StringGetNameFromFilenameNoExt(path);
    
    std::string vertex   = loader.GetDataBlockByName("vertex");
    std::string fragment = loader.GetDataBlockByName("fragment");
    
    ShaderTag newAsset;
    newAsset.name = filename;
    if (resourceName != "") 
        newAsset.name = resourceName;
    
    newAsset.vertexScript   = vertex;
    newAsset.fragmentScript = fragment;
    
    shaderTags.push_back(newAsset);
    
    std::string logstr = " + " + newAsset.name + "  " + path;
    Log.Write(logstr);
    std::cout << logstr << std::endl;
    
    return true;
}






