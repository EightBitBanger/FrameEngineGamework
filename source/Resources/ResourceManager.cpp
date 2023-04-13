#include "plugins.h"

#include "../Renderer/RenderSystem.h"
extern RenderSystem Renderer;

#include "ResourceManager.h"


unsigned char* ResourceManager::LoadImageRaw(char const* filename, int* width, int* height, int* channels, int req_channels) {
    return stbi_load(filename, width, height, channels, req_channels);
}

bool ResourceManager::LoadScene(std::string filename) {
    
    FileLoader sceneLoader(filename);
    if (!sceneLoader.CheckIsFileLoaded()) {
        std::string logstr = "! " + filename;
        Log.WriteLn();
        Log.Write(logstr);
        return false;
    }
    
    std::string logstr = filename;
    Log.Write(logstr);
    
    std::vector<std::string> definitionsList;
    std::vector<std::string> locationsList;
    
    for (unsigned int i=0; i < sceneLoader.rawData.size(); i++) {
        std::string fileExt = StringGetExtFromFilename(sceneLoader.rawData[i]);
        if (fileExt == "definitions") definitionsList.push_back( sceneLoader.rawData[i] );
        if (fileExt == "locations")   locationsList.push_back( sceneLoader.rawData[i] );
    }
    
    // Process definitions files
    for (unsigned int i=0; i < definitionsList.size(); i++) 
        LoadDefinitions(definitionsList[i]);
    
    // Process locations files
    for (unsigned int i=0; i < locationsList.size(); i++) 
        LoadLocations(locationsList[i]);
    
    return true;
}


bool ResourceManager::LoadDefinitions(std::string filename) {
    
    FileLoader definitionsLoader(filename);
    if (!definitionsLoader.CheckIsFileLoaded()) {
        std::string logstr = "! " + filename;
        Log.WriteLn();
        Log.Write(logstr);
        return false;
    }
    
    std::string logstr = "  " + filename;
    Log.WriteLn();
    Log.Write(logstr);
    
    for (unsigned int i=0; i < definitionsLoader.rawData.size(); i++) {
        
        std::vector<std::string> strexp = StringExplode(definitionsLoader.rawData[i], ' ');
        
        if (strexp[0] == "define_mesh") {
            LoadWaveFront(strexp[2], strexp[1]);
        }
        
        if (strexp[0] == "define_texture") {
            LoadTexture(strexp[2], strexp[1]);
        }
        
        if (strexp[0] == "define_shader") {
            LoadShaderGLSL(strexp[2], strexp[1]);
        }
        
    }
    
    return true;
}

bool ResourceManager::LoadLocations(std::string filename) {
    
    FileLoader locationsLoader(filename);
    if (!locationsLoader.CheckIsFileLoaded()) {
        std::string logstr = "! " + filename;
        Log.WriteLn();
        Log.Write(logstr);
        return false;
    }
    
    std::string logstr = "  " + filename;
    Log.WriteLn();
    Log.Write(logstr);
    
    for (unsigned int i=0; i < locationsLoader.rawData.size(); i++) {
        
        std::vector<std::string> strexp = StringExplode(locationsLoader.rawData[i], ' ');
        
        std::string logstr = "  + " + strexp[0] + "  " + strexp[1];
        Log.Write(logstr);
        
        
        
    }
    
    return true;
}

















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
    
    for (std::vector<TextureTag>::iterator it = textureTags.begin(); it != textureTags.end(); ++it) 
        if (it->buffer != nullptr) 
            stbi_image_free(it->buffer);
    
}












bool ResourceManager::LoadTexture(std::string filename, std::string resourceName="") {
    
    std::string name       = StringGetNameFromFilename(filename);
    std::string assetName  = StringGetNameFromFilenameNoExt(filename);
    
    if (resourceName != "") 
        assetName = resourceName;
    
    TextureTag textureTag;
    textureTag.name = assetName;
    
    textureTag.buffer = LoadImageRaw(filename.c_str(), &textureTag.width, &textureTag.height, &textureTag.channels, 3);
    
    if (textureTag.buffer == nullptr) {
        std::string logstr = " ! " + name;
        Log.Write(logstr);
        std::cout << logstr << std::endl;
        return false;
    }
    
    textureTags.push_back(textureTag);
    
    std::string logstr = "  + " + assetName + "  " + name;
    Log.Write(logstr);
    std::cout << logstr << std::endl;
    return true;
}


bool ResourceManager::LoadWaveFront(std::string filename, std::string resourceName="") {
    
    objl::Loader loader;
    if (!loader.LoadFile(filename)) {
        std::string logstr = " ! " + filename;
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
    
    std::string logstr = "  + " + newAsset.name + "  " + filename;
    Log.Write(logstr);
    std::cout << logstr << std::endl;
    
    return true;
}


bool ResourceManager::LoadShaderGLSL(std::string filename, std::string resourceName="") {
    
    FileLoader loader(filename);
    if (!loader.CheckIsFileLoaded()) {
        std::string logstr = " ! " + filename;
        Log.Write(logstr);
        std::cout << logstr << std::endl;
        return false;
    }
    
    std::string name = StringGetNameFromFilenameNoExt(filename);
    
    std::string vertex   = loader.GetDataBlockByName("vertex");
    std::string fragment = loader.GetDataBlockByName("fragment");
    
    ShaderTag newAsset;
    newAsset.name = name;
    if (resourceName != "") 
        newAsset.name = resourceName;
    
    newAsset.vertexScript   = vertex;
    newAsset.fragmentScript = fragment;
    
    shaderTags.push_back(newAsset);
    
    std::string logstr = "  + " + newAsset.name + "  " + name;
    Log.Write(logstr);
    std::cout << logstr << std::endl;
    
    return true;
}

