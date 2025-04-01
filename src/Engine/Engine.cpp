#include <GameEngineFramework/Engine/Engine.h>


EngineSystemManager::EngineSystemManager(void) : 
    sceneMain(nullptr),
    sceneOverlay(nullptr),
    cameraController(nullptr),
    
    mIsProfilerEnabled(false),
    
    mIsConsoleEnabled(false),
    mShowConsoleBackPanel(true),
    mConsoleCloseAfterCommandEntered(true),
    mConsoleDoFadeOutTexts(true),
    
    mConsoleFadeOutTimer(700),
    
    mConsolePrompt("-"),
    mConsoleString(""),
    mConsoleInput(nullptr),
    mConsoleInputObject(nullptr),
    mConsolePanelObject(nullptr),
    
    mDataStreamIndex(0),
    mObjectIndex(0),
    mStreamSize(0),
    
    usePhysicsDebugRenderer(false),
    debugMeshGameObject(nullptr),
    debugLinesGameObject(nullptr)
{
}


//
// Generation
//

void EngineSystemManager::SetHeightFieldValues(float* heightField, unsigned int width, unsigned int height, float value) {
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        heightField[i] = value;
    }
    return;
}

void EngineSystemManager::SetColorFieldValues(glm::vec3* colorField, unsigned int width, unsigned int height, Color color) {
    glm::vec3 colorVec(color.r, color.g, color.b);
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        colorField[i] = colorVec;
    }
    return;
}

float EngineSystemManager::AddHeightFieldFromPerlinNoise(float* heightField, unsigned int width, unsigned int height, 
                                                        float noiseWidth, float noiseHeight, 
                                                        float noiseMul, int offsetX, int offsetZ, int seed) {
    float minimumHeight = 1000.0f;
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        unsigned int x = i % width;
        unsigned int z = i / width;
        
        float xCoord = ((float)x + offsetX) * noiseWidth;
        float zCoord = ((float)z + offsetZ) * noiseHeight;
        
        float noise = Random.Perlin(xCoord, 0, zCoord, seed) * noiseMul;
        
        heightField[i] += Math.Round((noise * 10.0)) * 0.1;
        
        if (heightField[i] < minimumHeight)
            minimumHeight = heightField[i];
    }
    
    return minimumHeight;
}

void EngineSystemManager::GenerateWaterTableFromHeightField(float* heightField, 
                                                            unsigned int width, unsigned int height, 
                                                            float tableHeight) {
    unsigned int size = width * height;
    for (unsigned int i = 0; i < size; i++) {
        if (heightField[i] < tableHeight) heightField[i] *= 0.3;
    }
    return;
}

void EngineSystemManager::GenerateColorFieldFromHeightField(glm::vec3* colorField, float* heightField, 
                                                            unsigned int width, unsigned int height, 
                                                            Color low, Color high, float bias) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        float heightBias = heightField[i] * bias;
        heightBias = glm::clamp(heightBias, 0.0f, 1.0f);
        
        Color color = Colors.Lerp(low, high, heightBias);
        
        float uniformVariant = (Random.Range(0, 100) * 0.00001f) - (Random.Range(0, 10) * 0.00001f);
        
        color.r += uniformVariant;
        color.g += uniformVariant;
        color.b += uniformVariant;
        
        colorField[i] = glm::vec3(color.r, color.g, color.b);
    }
    return;
}

void EngineSystemManager::SetColorFieldFromPerlinNoise(glm::vec3* colorField, unsigned int width, unsigned int height, 
                                                       float noiseWidth, float noiseHeight, float noiseThreshold, 
                                                       Color first, Color second, int offsetX, int offsetZ) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        unsigned int x = i % width;
        unsigned int z = i / width;
        
        float xCoord = ((float)x + offsetX) * noiseWidth;
        float zCoord = ((float)z + offsetZ) * noiseHeight;
        
        float noise = Random.Perlin(xCoord, 0, zCoord, 100) + noiseThreshold;
        noise = glm::clamp(noise, 0.0f, 1.0f);
        
        Color result = Colors.Lerp(first, second, noise);
        
        colorField[i] = glm::vec3(result.r, result.g, result.b);
    }
    
    return;
}

void EngineSystemManager::AddColorFieldSnowCap(glm::vec3* colorField, float* heightField, 
                                               unsigned int width, unsigned int height, 
                                               Color capColor, float beginHeight, float bias) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        float heightBias = heightField[i] * 0.01;
        heightBias = glm::clamp(heightBias, 0.0f, 1.0f);
        
        Color color(colorField[i].x, colorField[i].y, colorField[i].z);
        int diff = ((beginHeight - (beginHeight - 20)) - (heightField[i] - beginHeight)) * bias;
        
        if (Random.Range(0, 100) > diff) 
            color = Colors.Lerp(color, capColor, heightField[i] * 0.07);
        
        colorField[i] = glm::vec3(color.r, color.g, color.b);
    }
    
    return;
}

void EngineSystemManager::AddColorFieldWaterTable(glm::vec3* colorField, float* heightField, 
                                                  unsigned int width, unsigned int height, 
                                                  Color waterColor, float beginHeight, float bias, float waterTableHeight) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) {
        
        if (heightField[i] > waterTableHeight) continue;
        
        Color color( colorField[i].x, colorField[i].y, colorField[i].z );
        waterColor = 0.001f;
        
        float height = heightField[i];
        if (height <= (waterTableHeight - 9.0f))   color = Colors.Lerp(color, waterColor, 1.0f);
        else if (height <= (waterTableHeight - 8.0f)) color = Colors.Lerp(color, waterColor, 0.9f);
        else if (height <= (waterTableHeight - 7.0f)) color = Colors.Lerp(color, waterColor, 0.85f);
        else if (height <= (waterTableHeight - 6.0f)) color = Colors.Lerp(color, waterColor, 0.8f);
        else if (height <= (waterTableHeight - 5.0f)) color = Colors.Lerp(color, waterColor, 0.75f);
        else if (height <= (waterTableHeight - 4.0f)) color = Colors.Lerp(color, waterColor, 0.7f);
        else if (height <= (waterTableHeight - 3.0f)) color = Colors.Lerp(color, waterColor, 0.65f);
        else if (height <= (waterTableHeight - 2.0f)) color = Colors.Lerp(color, waterColor, 0.6f);
        else if (height <= (waterTableHeight - 1.0f)) color = Colors.Lerp(color, waterColor, 0.55f);
        else if (height <= waterTableHeight) color = Colors.Lerp(color, waterColor, 0.5f);
        
        if (height < (waterTableHeight - 7.0f) && Random.Range(1, 100) > 70) color = Colors.orange * 0.003f;
        if (height < (waterTableHeight - 10.0f) && Random.Range(1, 100) > 90) color = Colors.green * 0.003f;
        if (height >= (waterTableHeight - 10.0f) && height < (waterTableHeight - 4.0f) && Random.Range(1, 100) > 88) color = Colors.green * 0.002f;
        if (height >= (waterTableHeight - 10.0f) && height < (waterTableHeight - 4.0f) && Random.Range(1, 100) > 80) color = Colors.green * 0.005f;
        if (height >= (waterTableHeight - 7.0f) && height < waterTableHeight && Random.Range(1, 100) > 90) color = Colors.brown * 0.01f;
        
        colorField[i] = glm::vec3(color.r, color.g, color.b);
    }
    
    return;
}

Mesh* EngineSystemManager::CreateMeshFromHeightField(float* heightField, 
                                                     glm::vec3* colorField, 
                                                     unsigned int width, unsigned int height, 
                                                     float offsetX, float offsetZ) {
    Mesh* mesh = Create<Mesh>();
    
    AddHeightFieldToMesh(mesh, heightField, colorField, width, height, offsetX, offsetZ);
    
    return mesh;
}

void EngineSystemManager::AddHeightStepToMesh(float* heightField, unsigned int width, unsigned int height) {
    unsigned int size = width * height;
    
    for (unsigned int i = 0; i < size; i++) 
        heightField[i] = Math.Round((heightField[i] * 10)) / 10;
    
    return;
}

void EngineSystemManager::AddHeightFieldToMesh(Mesh* mesh, 
                                               float* heightField, glm::vec3* colorField, 
                                               unsigned int width, unsigned int height, 
                                               float offsetX, float offsetZ, 
                                               unsigned int subTessX, unsigned int subTessZ) {
    
    unsigned int fieldWidth = (width / subTessX) - 1;
    unsigned int fieldHeight = (height / subTessZ) - 1;
    
    float sx = (subTessX > 1) ? subTessX * 4.0f : 1.0f;
    float sz = (subTessZ > 1) ? subTessZ * 4.0f : 1.0f;
    
    for (unsigned int x = 0; x < fieldWidth; x++) {
        
        for (unsigned int z = 0; z < fieldHeight; z++) {
            unsigned int xa = x * subTessX;
            unsigned int za = z * subTessZ;
            
            float yyA = heightField[za * width + xa];
            float yyB = heightField[za * width + (xa + 1)];
            float yyC = heightField[(za + 1) * width + (xa + 1)];
            float yyD = heightField[(za + 1) * width + xa];
            
            glm::vec3 cA = colorField[za * width + xa];
            glm::vec3 cB = colorField[za * width + xa];
            glm::vec3 cC = colorField[za * width + xa];
            glm::vec3 cD = colorField[za * width + xa];
            
            float xx = (((float)x + offsetX - (float)width / 2) / 2) + 0.25;
            float zz = (((float)z + offsetZ - (float)height / 2) / 2) + 0.25;
            
            Vertex vertex[4] = {
                Vertex(xx, yyA, zz, cA.x, cA.y, cA.z, 0, 1, 0, 0, 0),
                Vertex(xx + sx, yyB, zz, cB.x, cB.y, cB.z, 0, 1, 0, 1, 0),
                Vertex(xx + sx, yyC, zz + sz, cC.x, cC.y, cC.z, 0, 1, 0, 1, 1),
                Vertex(xx, yyD, zz + sz, cD.x, cD.y, cD.z, 0, 1, 0, 0, 1)
            };
            
            glm::vec3 U = glm::vec3(vertex[2].x, vertex[2].y, vertex[2].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 V = glm::vec3(vertex[1].x, vertex[1].y, vertex[1].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 normal = glm::cross(U, V);
            
            for (int i = 0; i < 4; i++) {
                vertex[i].nx = normal.x;
                vertex[i].ny = normal.y;
                vertex[i].nz = normal.z;
            }
            
            SubMesh subBuffer;
            subBuffer.vertexBuffer.assign(vertex, vertex + 4);
            subBuffer.indexBuffer = {0, 2, 1, 0, 3, 2};
            
            mesh->AddSubMesh(xx, 0, zz, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
        }
        
    }
    
    return;
}



void EngineSystemManager::AddHeightFieldToMeshLOD(Mesh* mesh, 
                                                  float* heightField, glm::vec3* colorField, 
                                                  unsigned int width, unsigned int height, 
                                                  float offsetX, float offsetZ, 
                                                  unsigned int lodFactor) {
    
    unsigned int fieldWidth = (width / lodFactor) - 1;
    unsigned int fieldHeight = (height / lodFactor) - 1;
    
    float sx = (lodFactor > 1) ? lodFactor * 4.0f : 1.0f;
    float sz = (lodFactor > 1) ? lodFactor * 4.0f : 1.0f;
    
    for (unsigned int x = 0; x < fieldWidth; x++) {
        for (unsigned int z = 0; z < fieldHeight; z++) {
            unsigned int xa = x * lodFactor;
            unsigned int za = z * lodFactor;
            
            float yyA = heightField[za * width + xa];
            float yyB = heightField[za * width + (xa + lodFactor)];
            float yyC = heightField[(za + lodFactor) * width + (xa + lodFactor)];
            float yyD = heightField[(za + lodFactor) * width + xa];
            
            glm::vec3 cA = colorField[za * width + xa];
            glm::vec3 cB = colorField[za * width + (xa + lodFactor)];
            glm::vec3 cC = colorField[(za + lodFactor) * width + (xa + lodFactor)];
            glm::vec3 cD = colorField[(za + lodFactor) * width + xa];
            
            float xx = (((float)x + offsetX - (float)width / 2) / 2) + 0.25;
            float zz = (((float)z + offsetZ - (float)height / 2) / 2) + 0.25;
            
            Vertex vertex[4] = {
                Vertex(xx, yyA, zz, cA.x, cA.y, cA.z, 0, 1, 0, 0, 0),
                Vertex(xx + sx, yyB, zz, cB.x, cB.y, cB.z, 0, 1, 0, 1, 0),
                Vertex(xx + sx, yyC, zz + sz, cC.x, cC.y, cC.z, 0, 1, 0, 1, 1),
                Vertex(xx, yyD, zz + sz, cD.x, cD.y, cD.z, 0, 1, 0, 0, 1)
            };
            
            glm::vec3 U = glm::vec3(vertex[2].x, vertex[2].y, vertex[2].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 V = glm::vec3(vertex[1].x, vertex[1].y, vertex[1].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 normal = glm::cross(U, V);
            
            for (int i = 0; i < 4; i++) {
                vertex[i].nx = normal.x;
                vertex[i].ny = normal.y;
                vertex[i].nz = normal.z;
            }
            
            SubMesh subBuffer;
            subBuffer.vertexBuffer.assign(vertex, vertex + 4);
            subBuffer.indexBuffer = {0, 2, 1, 0, 3, 2};
            
            mesh->AddSubMesh(xx, 0, zz, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
        }
    }
    
    return;
}


void EngineSystemManager::AddHeightFieldToMeshHalfSize(Mesh* mesh, 
                                               float* heightField, glm::vec3* colorField, 
                                               unsigned int width, unsigned int height, 
                                               float offsetX, float offsetZ) {
    
    unsigned int halfWidth = width / 2.0f;
    unsigned int halfHeight = height / 2.0f;
    
    unsigned int fieldWidth = halfWidth;
    unsigned int fieldHeight = halfHeight;
    
    for (unsigned int x = 0; x < fieldWidth; x++) {
        
        for (unsigned int z = 0; z < fieldHeight; z++) {
            unsigned int xa = x * 2;
            unsigned int za = z * 2;
            
            float yyA = heightField[za * width + xa] - 0.25f;
            float yyB = heightField[za * width + (xa + 2)] - 0.25f;
            float yyC = heightField[(za + 2) * width + (xa + 2)] - 0.25f;
            float yyD = heightField[(za + 2) * width + xa] - 0.25f;
            
            glm::vec3 cA = colorField[za * width + xa];
            glm::vec3 cB = colorField[za * width + (xa + 2)];
            glm::vec3 cC = colorField[(za + 2) * width + (xa + 2)];
            glm::vec3 cD = colorField[(za + 2) * width + xa];
            
            float xx = (((float)x * 2.5f + offsetX - (float)halfWidth)  / 2.5f) - 2.5f;
            float zz = (((float)z * 2.5f + offsetZ - (float)halfHeight) / 2.5f) - 2.5f;
            
            Vertex vertex[4] = {
                Vertex(xx, yyA, zz, cA.x, cA.y, cA.z, 0, 1, 0, 0, 0),
                Vertex(xx + 2.0f, yyB, zz, cB.x, cB.y, cB.z, 0, 1, 0, 1, 0),
                Vertex(xx + 2.0f, yyC, zz + 2.0f, cC.x, cC.y, cC.z, 0, 1, 0, 1, 1),
                Vertex(xx, yyD, zz + 2.0f, cD.x, cD.y, cD.z, 0, 1, 0, 0, 1)
            };
            
            glm::vec3 U = glm::vec3(vertex[2].x, vertex[2].y, vertex[2].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 V = glm::vec3(vertex[1].x, vertex[1].y, vertex[1].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 normal = glm::cross(U, V);
            
            for (int i = 0; i < 4; i++) {
                vertex[i].nx = normal.x;
                vertex[i].ny = normal.y;
                vertex[i].nz = normal.z;
            }
            
            SubMesh subBuffer;
            subBuffer.vertexBuffer.assign(vertex, vertex + 4);
            subBuffer.indexBuffer = {0, 2, 1, 0, 3, 2};
            
            mesh->AddSubMesh(xx, 0, zz, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
        }
        
    }
    
    return;
}

void EngineSystemManager::AddHeightFieldToMeshReduced(Mesh* mesh, 
                                                      float* heightField, 
                                                      glm::vec3* colorField, 
                                                      unsigned int width, unsigned int height, 
                                                      float offsetX, float offsetZ, 
                                                      unsigned int resolution) {
    
    int fieldWidth = width / resolution;
    int fieldHeight = height / resolution;
    
    for (int x = 0; x < fieldWidth; x++) {
        
        for (int z = 0; z < fieldHeight; z++) {
            int xa = x * resolution;
            int za = z * resolution;
            
            float yyA = heightField[za * width + xa];
            float yyB = heightField[za * width + (xa + resolution)];
            float yyC = heightField[(za + resolution) * width + (xa + resolution)];
            float yyD = heightField[(za + resolution) * width + xa];
            
            glm::vec3 cA = colorField[za * width + xa];
            glm::vec3 cB = colorField[za * width + (xa + resolution)];
            glm::vec3 cC = colorField[(za + resolution) * width + (xa + resolution)];
            glm::vec3 cD = colorField[(za + resolution) * width + xa];
            
            float xx = (((float)x * resolution + offsetX - (float)fieldWidth * resolution / 2) / 2) + 0.25;
            float zz = (((float)z * resolution + offsetZ - (float)fieldHeight * resolution / 2) / 2) + 0.25;
            
            Vertex vertex[4] = {
                Vertex(xx, yyA, zz, cA.x, cA.y, cA.z, 0, 1, 0, 0, 0),
                Vertex(xx + resolution, yyB, zz, cB.x, cB.y, cB.z, 0, 1, 0, 1, 0),
                Vertex(xx + resolution, yyC, zz + resolution, cC.x, cC.y, cC.z, 0, 1, 0, 1, 1),
                Vertex(xx, yyD, zz + resolution, cD.x, cD.y, cD.z, 0, 1, 0, 0, 1)
            };
            
            glm::vec3 U = glm::vec3(vertex[2].x, vertex[2].y, vertex[2].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 V = glm::vec3(vertex[1].x, vertex[1].y, vertex[1].z) - glm::vec3(vertex[0].x, vertex[0].y, vertex[0].z);
            glm::vec3 normal = glm::cross(U, V);
            
            for (int i = 0; i < 4; i++) {
                vertex[i].nx = normal.x;
                vertex[i].ny = normal.y;
                vertex[i].nz = normal.z;
            }
            
            float scale = 1.0f;
            float scaleDv = 1.0f / 2.0f;
            
            SubMesh subBuffer;
            for (int i = 0; i < 4; i++) {
                vertex[i].x  = (vertex[i].x * scale) - scaleDv;
                vertex[i].y += (vertex[i].y) - scaleDv;
                vertex[i].z  = (vertex[i].z * scale) - scaleDv;
            }
            
            subBuffer.vertexBuffer.assign(vertex, vertex + 4);
            subBuffer.indexBuffer = {0, 2, 1, 0, 3, 2};
            
            mesh->AddSubMesh(xx, 0, zz, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
        }
        
    }
    
    return;
}


// UI


Button* EngineSystemManager::CreateOverlayButtonCallback(float x, float y, float width, float height, ButtonCallBack callback) {
    Button* newButton = mButtons.Create();
    newButton->x = x;
    newButton->y = y;
    newButton->w = width;
    newButton->h = height;
    newButton->callback = callback;
    return newButton;
}

bool EngineSystemManager::DestroyOverlayButtonCallback(Button* button) {
    return mButtons.Destroy( button );
}

void EngineSystemManager::AddMeshText(GameObject* overlayObject, 
                                      float xPos, float yPos, 
                                      float width, float height, 
                                      std::string text, Color textColor) {
    
    Mesh* meshPtr = overlayObject->GetComponent<MeshRenderer>()->mesh;
    if (meshPtr == nullptr) 
        return;
    
    for (unsigned int i=0; i < text.size(); i++)
        AddMeshSubSprite(overlayObject, xPos + i, yPos, width, height, text[i], textColor);
    
    meshPtr->Load();
    
    return;
}

void EngineSystemManager::AddMeshSubSprite(GameObject* overlayObject, 
                                           float xPos, float yPos, 
                                           float width, float height, 
                                           int index, Color meshColor) {
    
    Mesh* meshPtr = overlayObject->GetComponent<MeshRenderer>()->mesh;
    if (meshPtr == nullptr) 
        return;
    
    Text* textPtr = overlayObject->GetComponent<Text>();
    if (textPtr == nullptr) 
        return;
    
    // Sprite atlas layout
    float spriteStartX  = textPtr->sprite.subSpriteX;
    float spriteStartY  = textPtr->sprite.subSpriteY;
    float spriteWidth   = textPtr->sprite.subSpriteWidth;
    float spriteHeight  = textPtr->sprite.subSpriteHeight;
    
    float spacingWidth  = textPtr->width;
    float spacingHeight = textPtr->height;
    
    int mapWidth  = textPtr->sprite.width;
    int mapHeight = textPtr->sprite.height;
    
    // Calculate the sub sprite in the map grid
    int subWidth  = 0;
    int subHeight = 0;
    
    for (int i=0; i < index; i++) {
        
        subWidth++;
        
        if (subWidth > mapWidth) {
            subWidth=0;
            
            subHeight++;
            
            if (subHeight > mapHeight)
                return;
        }
    }
    
    meshPtr->AddPlain(yPos * spacingHeight, 
                      0, 
                      -(xPos * spacingWidth), 
                      width, height, 
                      meshColor, 
                      spriteWidth, spriteHeight, 
                      spriteStartX, spriteStartY, 
                      subWidth, subHeight);
    
    return;
}

GameObject* EngineSystemManager::GetGameObject(unsigned int index) {
    if (index < mGameObjects.Size()) 
        return mGameObjects[index];
    return nullptr;
}

void EngineSystemManager::Initiate() {
    
    // Load default shaders
    shaders.texture       = Resources.CreateShaderFromTag("texture");
    shaders.textureUnlit  = Resources.CreateShaderFromTag("textureUnlit");
    shaders.color         = Resources.CreateShaderFromTag("color");
    shaders.colorUnlit    = Resources.CreateShaderFromTag("colorUnlit");
    shaders.UI            = Resources.CreateShaderFromTag("UI");
    shaders.shadowCaster  = Resources.CreateShaderFromTag("shadowCaster");
    shaders.sky           = Resources.CreateShaderFromTag("sky");
    shaders.water         = Resources.CreateShaderFromTag("water");
    
    // Load default meshes
    meshes.grassHorz       = Resources.CreateMeshFromTag("grassHorz");
    meshes.grassVert       = Resources.CreateMeshFromTag("grassVert");
    
    meshes.stemHorz        = Resources.CreateMeshFromTag("stemHorz");
    meshes.stemVert        = Resources.CreateMeshFromTag("stemVert");
    
    meshes.wallHorizontal  = Resources.CreateMeshFromTag("wallh");
    meshes.wallVertical    = Resources.CreateMeshFromTag("wallv");
    
    meshes.log             = Resources.CreateMeshFromTag("log");
    
    meshes.cube            = Resources.CreateMeshFromTag("cube");
    meshes.chunk           = Resources.CreateMeshFromTag("chunk");
    meshes.plain           = Resources.CreateMeshFromTag("plain");
    meshes.sphere          = Resources.CreateMeshFromTag("sphere");
    
    // Prevent the meshes from being garbage collected
    meshes.cube->isShared            = true;
    meshes.chunk->isShared           = true;
    meshes.plain->isShared           = true;
    meshes.sphere->isShared          = true;
    meshes.wallHorizontal->isShared  = true;
    meshes.wallVertical->isShared    = true;
    
    // Generate default colliders
    colliders.box = Physics.common.createBoxShape( rp3d::Vector3(0.5f, 0.5f, 0.5f) );
    
    // Main world scene
    sceneMain = Create<Scene>();
    Renderer.AddSceneToRenderQueue( sceneMain );
    
    // Initiate render system defaults
    Renderer.shaders.texture      = shaders.texture;
    Renderer.shaders.textureUnlit = shaders.textureUnlit;
    Renderer.shaders.color        = shaders.color;
    Renderer.shaders.colorUnlit   = shaders.colorUnlit;
    Renderer.shaders.UI           = shaders.UI;
    Renderer.shaders.shadowCaster = shaders.shadowCaster;
    Renderer.shaders.sky          = shaders.sky;
    
    Renderer.meshes.cube            = meshes.cube;
    Renderer.meshes.chunk           = meshes.chunk;
    Renderer.meshes.plain           = meshes.plain;
    Renderer.meshes.sphere          = meshes.sphere;
    Renderer.meshes.wallHorizontal  = meshes.wallHorizontal;
    Renderer.meshes.wallVertical    = meshes.wallVertical;
    
    // Initiate overlay scene
    sceneOverlay = Create<Scene>();
    Renderer.AddSceneToRenderQueue(sceneOverlay);
    
    sceneOverlay->camera = Create<Camera>();
    sceneOverlay->camera->isOrthographic = true;
    sceneOverlay->camera->isFixedAspect  = true;
    
    sceneOverlay->camera->viewport.w = Renderer.displaySize.x;
    sceneOverlay->camera->viewport.h = Renderer.displaySize.y;
    
    sceneOverlay->camera->clipFar  =  100;
    sceneOverlay->camera->clipNear = -100;
    
    // Console panel overlay
    mConsolePanelObject = CreateOverlayPanelRenderer(200, -8, 5000, 10, "panel_blue");
    MeshRenderer* panelRenderer = mConsolePanelObject->GetComponent<MeshRenderer>();
    sceneOverlay->AddMeshRendererToSceneRoot( panelRenderer, RENDER_QUEUE_BACKGROUND );
    mConsolePanelObject->isActive = false;
    
    float alphaBlend = 0.87;
    panelRenderer->material->ambient.g = alphaBlend;
    
    Panel* panel = mConsolePanelObject->GetComponent<Panel>();
    panel->canvas.anchorRight = false;
    panel->canvas.anchorTop = false;
    
    // Initiate console input text
    
    mConsoleInputObject = CreateOverlayTextRenderer(0, 0, "", 9, Colors.ltgray, "font");
    MeshRenderer* inputRenderer = mConsoleInputObject->GetComponent<MeshRenderer>();
    inputRenderer->isActive = false;
    
    sceneOverlay->AddMeshRendererToSceneRoot( inputRenderer, RENDER_QUEUE_OVERLAY );
    
    mConsoleInput = mConsoleInputObject->GetComponent<Text>();
    mConsoleInput->canvas.anchorTop = false;
    
    mConsoleInput->canvas.x = 0;
    mConsoleInput->canvas.y = 0;
    
    // Initiate console text elements
    
    for (unsigned int i=0; i < CONSOLE_NUMBER_OF_ELEMENTS; i++) {
        
        mConsoleTextObjects[i] = CreateOverlayTextRenderer(0, 0, "", 9, Colors.MakeGrayScale(0.87), "font");
        
        MeshRenderer* meshRenderer = mConsoleTextObjects[i]->GetComponent<MeshRenderer>();
        meshRenderer->material->EnableBlending();
        meshRenderer->material->SetBlending(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        
        sceneOverlay->AddMeshRendererToSceneRoot( meshRenderer, RENDER_QUEUE_PREGEOMETRY );
        
        mConsoleText[i] = mConsoleTextObjects[i]->GetComponent<Text>();
        mConsoleText[i]->canvas.anchorTop = false;
        
        mConsoleText[i]->text = "";
        
        mConsoleText[i]->canvas.x = 0;
        mConsoleText[i]->canvas.y = -(2 * i + 4) + 40;
        
        mConsoleText[i]->canvas.y -= 39;
        
    }
    
    // Initiate the profiler text elements
    
    for (unsigned int i=0; i < PROFILER_NUMBER_OF_ELEMENTS; i++) {
        
        mProfilerTextObjects[i] = CreateOverlayTextRenderer(0, 0, "", 9, Colors.MakeGrayScale(0.87), "font");
        
        MeshRenderer* meshRenderer = mProfilerTextObjects[i]->GetComponent<MeshRenderer>();
        meshRenderer->material->EnableBlending();
        meshRenderer->material->SetBlending(BLEND_SRC_ALPHA, BLEND_ONE_MINUS_SRC_ALPHA);
        
        sceneOverlay->AddMeshRendererToSceneRoot( meshRenderer, RENDER_QUEUE_OVERLAY );
        
        mProfilerText[i] = mProfilerTextObjects[i]->GetComponent<Text>();
        mProfilerText[i]->canvas.anchorTop = true;
        
        mProfilerText[i]->canvas.x = 0;
        mProfilerText[i]->canvas.y = (2 * i + 4);
        
    }
    
    return;
}

void EngineSystemManager::Shutdown(void) {
    
    Destroy<Scene>(sceneMain);
    
    Destroy<Scene>(sceneOverlay);
    
    Renderer.DestroyShader(shaders.texture);
    Renderer.DestroyShader(shaders.textureUnlit);
    Renderer.DestroyShader(shaders.color);
    Renderer.DestroyShader(shaders.colorUnlit);
    Renderer.DestroyShader(shaders.UI);
    Renderer.DestroyShader(shaders.shadowCaster);
    
    return;
}



//
// Physics debug renderer
//

void EngineSystemManager::EnablePhysicsDebugRenderer(void) {
    
    usePhysicsDebugRenderer = true;
    
    Physics.world->setIsDebugRenderingEnabled(true);
    
    rp3d::DebugRenderer& debugRenderer = Physics.world->getDebugRenderer();
    
    debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, true);
    //debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_AABB, true);
    //debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, true);
    
    Mesh* debugLines = Create<Mesh>();
    Mesh* debugMesh  = Create<Mesh>();
    
    Material* debugMaterialLines = Create<Material>();
    Material* debugMaterialMesh  = Create<Material>();
    
    debugMaterialLines->shader = shaders.colorUnlit;
    debugMaterialLines->ambient = Colors.white;
    debugMaterialLines->diffuse = Colors.white;
    
    debugMaterialMesh->shader = shaders.colorUnlit;
    debugMaterialMesh->ambient = Colors.white;
    debugMaterialMesh->diffuse = Colors.white;
    
    debugLines->SetPrimitive( MESH_LINES );
    debugMesh->SetPrimitive( MESH_LINES );
    
    
    debugLinesGameObject = Create<GameObject>();
    debugMeshGameObject = Create<GameObject>();
    
    debugLinesGameObject->AddComponent( CreateComponent<MeshRenderer>() );
    debugMeshGameObject->AddComponent( CreateComponent<MeshRenderer>() );
    
    MeshRenderer* meshRendererLines = debugLinesGameObject->GetComponent<MeshRenderer>();
    MeshRenderer* meshRendererMeshes = debugMeshGameObject->GetComponent<MeshRenderer>();
    
    meshRendererLines->mesh     = debugLines;
    meshRendererLines->material = debugMaterialLines;
    meshRendererLines->DisableFrustumCulling();
    
    meshRendererMeshes->mesh     = debugMesh;
    meshRendererMeshes->material = debugMaterialMesh;
    meshRendererMeshes->DisableFrustumCulling();
    
    sceneMain->AddMeshRendererToSceneRoot( meshRendererLines, RENDER_QUEUE_FOREGROUND );
    sceneMain->AddMeshRendererToSceneRoot( meshRendererMeshes, RENDER_QUEUE_FOREGROUND );
        
    return;
}

void EngineSystemManager::UpdatePhysicsDebugRenderer(void) {
    
    if (!usePhysicsDebugRenderer) 
        return;
    
    rp3d::DebugRenderer& debugRenderer = Physics.world->getDebugRenderer();
    
    
    //
    // Lines
    //
    
    unsigned int numberOfLines = debugRenderer.getNbLines();
    rp3d::Array<rp3d::DebugRenderer::DebugLine> lineArray = debugRenderer.getLines();
    
    SubMesh subMeshLines;
    for (unsigned int i=0; i < numberOfLines; i++) {
        
        rp3d::DebugRenderer::DebugLine line = lineArray[i];
        
        Vertex vertA;
        vertA.x = line.point1.x;
        vertA.y = line.point1.y;
        vertA.z = line.point1.z;
        vertA.r = 1;
        vertA.g = 1;
        vertA.b = 1;
        vertA.nx = 0;
        vertA.ny = 1;
        vertA.nz = 0;
        
        Vertex vertB;
        vertB.x = line.point2.x;
        vertB.y = line.point2.y;
        vertB.z = line.point2.z;
        vertB.r = 1;
        vertB.g = 1;
        vertB.b = 1;
        vertB.nx = 0;
        vertB.ny = 1;
        vertB.nz = 0;
        
        subMeshLines.vertexBuffer.push_back(vertA);
        subMeshLines.vertexBuffer.push_back(vertB);
        
        subMeshLines.indexBuffer.push_back( (i * 2) + 0 );
        subMeshLines.indexBuffer.push_back( (i * 2) + 1 );
        
    }
    
    subMeshLines.indexCount  = numberOfLines * 2;
    subMeshLines.vertexCount = numberOfLines * 2;
    
    MeshRenderer* debugLinesRenderer = debugMeshGameObject->GetComponent<MeshRenderer>();
    
    Mesh* debugLines = debugLinesRenderer->mesh;
    
    debugLines->ClearSubMeshes();
    
    debugLines->AddSubMesh(0, 0, 0, subMeshLines);
    
    debugLines->Load();
    
    
    //
    // Triangles
    //
    
    unsigned int numberOfTriangles = debugRenderer.getNbTriangles();
    rp3d::Array<rp3d::DebugRenderer::DebugTriangle> triangleArray = debugRenderer.getTriangles();
    
    SubMesh subMeshTriangles;
    for (unsigned int i=0; i < numberOfTriangles; i++) {
        
        rp3d::DebugRenderer::DebugTriangle triangle = triangleArray[i];
        
        Vertex vertA;
        vertA.x = triangle.point1.x;
        vertA.y = triangle.point1.y;
        vertA.z = triangle.point1.z;
        vertA.r = 1;
        vertA.g = 1;
        vertA.b = 1;
        vertA.nx = 0;
        vertA.ny = 1;
        vertA.nz = 0;
        vertA.u = 0;
        vertA.v = 0;
        
        Vertex vertB;
        vertB.x = triangle.point2.x;
        vertB.y = triangle.point2.y;
        vertB.z = triangle.point2.z;
        vertB.r = 1;
        vertB.g = 1;
        vertB.b = 1;
        vertB.nx = 0;
        vertB.ny = 1;
        vertB.nz = 0;
        vertB.u = 1;
        vertB.v = 0;
        
        Vertex vertC;
        vertC.x = triangle.point3.x;
        vertC.y = triangle.point3.y;
        vertC.z = triangle.point3.z;
        vertC.r = 1;
        vertC.g = 1;
        vertC.b = 1;
        vertC.nx = 0;
        vertC.ny = 1;
        vertC.nz = 0;
        vertC.u = 1;
        vertC.v = 1;
        
        subMeshTriangles.vertexBuffer.push_back(vertA);
        subMeshTriangles.vertexBuffer.push_back(vertB);
        subMeshTriangles.vertexBuffer.push_back(vertC);
        
        subMeshTriangles.indexBuffer.push_back( (i * 3) + 0 );
        subMeshTriangles.indexBuffer.push_back( (i * 3) + 1 );
        subMeshTriangles.indexBuffer.push_back( (i * 3) + 2 );
        
    }
    
    subMeshTriangles.indexCount  = numberOfTriangles * 3;
    subMeshTriangles.vertexCount = numberOfTriangles * 3;
    
    MeshRenderer* debugMeshRenderer = debugMeshGameObject->GetComponent<MeshRenderer>();
    
    Mesh* debugMesh = debugMeshRenderer->mesh;
    
    debugMesh->ClearSubMeshes();
    
    debugMesh->AddSubMesh(0, 0, 0, subMeshTriangles);
    
    debugMesh->Load();
    
    return;
}

