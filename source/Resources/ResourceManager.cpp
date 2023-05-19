#include "ResourceManager.h"

#include "plugins.h"

extern RenderSystem Renderer;
extern PhysicsSystem Physics;


bool ResourceManager::LoadScene(std::string path) {
    
    FileLoader sceneLoader(path);
    assert(sceneLoader.CheckIsFileLoaded());
    
    std::string logstr = path;
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

bool ResourceManager::LoadDefinitions(std::string path) {
    
    FileLoader definitionsLoader(path);
    assert(definitionsLoader.CheckIsFileLoaded());
    
    std::string logstr = "  " + path;
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
        
        if (strexp[0] == "define_collider") {
            ColliderTag colliderTag;
            rp3d::Vector3 scale(1, 1, 1);
            scale.x += StringToFloat(strexp[2]);
            scale.y += StringToFloat(strexp[3]);
            scale.z += StringToFloat(strexp[4]);
            
            if (strexp[5] == "dynamic") {
                colliderTag.isStatic = false;
            } else {
                colliderTag.isStatic = true;
            }
            
            colliderTag.name = strexp[1];
            colliderTag.colliderShape = Physics.common.createBoxShape(scale);
            assert(colliderTag.colliderShape != nullptr);
            
            mColliderTags.push_back(colliderTag);
            
            std::string logstr = "  + " + strexp[1];
            Log.Write(logstr);
            std::cout << logstr << std::endl;
            return true;
        }
        
    }
    
    return true;
}

bool ResourceManager::LoadLocations(std::string path) {
    
    FileLoader locationsLoader(path);
    assert(locationsLoader.CheckIsFileLoaded());
    
    std::string logstr = "  " + path;
    Log.WriteLn();
    Log.Write(logstr);
    
    std::vector<std::string> strexp;
    strexp.reserve(11);
    
    for (unsigned int i=0; i < locationsLoader.rawData.size(); i++) {
        
        // Create a new entity
        Entity* newEntity = Renderer.CreateEntity();
        sceneMain->AddEntityToSceneRoot(newEntity);
        
        strexp.clear();
        strexp = StringExplode(locationsLoader.rawData[i], ' ');
        //newEntity->name = strexp[0];
        
        // Assign mesh
        newEntity->AttachMesh(CreateMeshFromTag( strexp[1] ));
        newEntity->AttachMaterial(CreateMaterialFromTag( strexp[2] ));
        
        float posx = StringToFloat(strexp[3]);
        float posy = StringToFloat(strexp[4]);
        float posz = StringToFloat(strexp[5]);
        newEntity->transform.position = glm::vec3(posx, posy, posz);
        
        float rotx = StringToFloat(strexp[6]);
        float roty = StringToFloat(strexp[7]);
        float rotz = StringToFloat(strexp[8]);
        newEntity->transform.rotation = glm::vec4(rotx, roty, rotz, 1);
        
        float sclx = StringToFloat(strexp[9]);
        float scly = StringToFloat(strexp[10]);
        float sclz = StringToFloat(strexp[11]);
        newEntity->transform.scale = glm::vec3(sclx, scly, sclz);
        
        // Check object physics type
        if (strexp[12] != "none") {
            
            // Position
            //rp3d::RigidBody* rigidBody = Physics.CreateRigidBody(posx, posy, posz);
            //newEntity->AttachRidigBody(rigidBody);
            //newEntity->SetRigidBodyStatic();
            //newEntity->AddCollider( FindColliderTag(strexp[12]), 0, 0, 0 );
            
            // Rotate
            //const rp3d::Quaternion quaternion = rp3d::Quaternion::fromEulerAngles(rotx, roty, rotz);
            //rp3d::Transform rbTransform = rigidBody->getTransform();
            //rbTransform.setOrientation(quaternion);
            
        }
        
        std::string logstr = "  + " + strexp[0] + "  " + strexp[1];
        Log.Write(logstr);
        
    }
    
    return true;
}

void ResourceManager::Initiate(void) {
    
    sceneMain = Renderer.CreateScene();
    Renderer.AddToRenderQueue(sceneMain);
    
    stbi_set_flip_vertically_on_load(true);
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
    assert(meshTag != nullptr);
    Mesh* meshPtr = Renderer.CreateMesh();
    meshPtr->AddSubMesh(0,0,0, meshTag->mesh.vertexBuffer, meshTag->mesh.indexBuffer);
    meshPtr->SetAttribute(0, 3, sizeof(Vertex), 0);
    meshPtr->SetAttribute(1, 3, sizeof(Vertex), 12);
    meshPtr->SetAttribute(2, 2, sizeof(Vertex), 24);
    return meshPtr;
}

Material* ResourceManager::CreateMaterialFromTag(std::string resourceName) {
    TextureTag* texTag = FindTextureTag(resourceName);
    assert(texTag != nullptr);
    Material* materialPtr = Renderer.CreateMaterial();
    materialPtr->width  = texTag->width;
    materialPtr->height = texTag->height;
    materialPtr->UpdateTextureBuffer(texTag->buffer);
    return materialPtr;
}

Shader* ResourceManager::CreateShaderFromTag(std::string resourceName) {
    ShaderTag* shaderTag = FindShaderTag(resourceName);
    assert(shaderTag != nullptr);
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
    
    std::string logstr = "  + " + assetName + "  " + name;
    Log.Write(logstr);
    std::cout << logstr << std::endl;
    return true;
}

bool ResourceManager::LoadWaveFront(std::string path, std::string resourceName="meshDefault") {
    
    objl::Loader loader;
    assert(loader.LoadFile(path));
    
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
        
        newAsset.mesh.vertexBuffer.push_back(vertex);
    }
    
    for (unsigned int i=0; i < loader.LoadedMeshes[0].Indices.size(); i++) {
        newAsset.mesh.indexBuffer.push_back(loader.LoadedMeshes[0].Indices[i]);
    }
    
    mMeshTags.push_back(newAsset);
    
    std::string logstr = "  + " + newAsset.name + "  " + path;
    Log.Write(logstr);
    std::cout << logstr << std::endl;
    
    return true;
}

bool ResourceManager::LoadShaderGLSL(std::string path, std::string resourceName="shaDefault") {
    
    FileLoader loader(path);
    if (!loader.CheckIsFileLoaded()) {
        std::string logstr = "! " + path;
        Log.Write(logstr);
        std::cout << logstr << std::endl;
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
    
    std::string logstr = "  + " + newAsset.name + "  " + name;
    Log.Write(logstr);
    std::cout << logstr << std::endl;
    
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
