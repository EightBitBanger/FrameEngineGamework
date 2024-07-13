#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API EngineComponents  Components;
ENGINE_API ColorPreset       Colors;
ENGINE_API NumberGeneration  Random;
ENGINE_API Logger            Log;
ENGINE_API Timer             PhysicsTime;
ENGINE_API Timer             Time;
ENGINE_API ProfilerTimer     Profiler;

ENGINE_API Serialization     Serializer;
ENGINE_API ResourceManager   Resources;
ENGINE_API ScriptSystem      Scripting;
ENGINE_API RenderSystem      Renderer;
ENGINE_API PhysicsSystem     Physics;
ENGINE_API NetworkSystem     Network;
ENGINE_API AudioSystem       Audio;
ENGINE_API InputSystem       Input;
ENGINE_API MathCore          Math;
ENGINE_API ActorSystem       AI;

ENGINE_API StringType        String;
ENGINE_API FloatType         Float;
ENGINE_API DoubleType        Double;
ENGINE_API IntType           Int;
ENGINE_API UintType          Uint;

ENGINE_API FileSystemDir        Directory;
ENGINE_API PlatformLayer        Platform;
ENGINE_API EngineSystemManager  Engine;



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
    debugMesh(nullptr),
    debugLines(nullptr)
{
}


//
// Generation
//

void EngineSystemManager::SetHeightFieldValues(float* heightField, unsigned int width, unsigned int height, float value) {
    
    for (unsigned int x=0; x < width; x ++) {
        
        for (unsigned int z=0; z < height; z ++) {
            
            unsigned int index = z * width + x;
            
            heightField[index] = value;
            
            continue;
        }
        
        continue;
    }
    
    return;
}

void EngineSystemManager::SetColorFieldValues(glm::vec3* colorField, unsigned int width, unsigned int height, Color color) {
    
    for (unsigned int x=0; x < width; x ++) {
        
        for (unsigned int z=0; z < height; z ++) {
            
            unsigned int index = z * width + x;
            
            color.r = Random.Range(0, 100) * 0.0001f;
            color.g = Random.Range(0, 100) * 0.0001f;
            color.b = Random.Range(0, 100) * 0.0001f;
            
            colorField[index] = glm::vec3(color.r, color.g, color.b);
            
            continue;
        }
        
        continue;
    }
    
    return;
}

void EngineSystemManager::AddHeightFieldFromPerlinNoise(float* heightField, unsigned int width, unsigned int height, 
                                                        float noiseWidth, float noiseHeight, 
                                                        float noiseMul, int offsetX, int offsetZ) {
    
    for (unsigned int x=0; x < width; x ++) {
        
        for (unsigned int z=0; z < height; z ++) {
            
            float xCoord = ((float)x + offsetX) * noiseWidth;
            float zCoord = ((float)z + offsetZ) * noiseHeight;
            
            float noise = Random.Perlin(xCoord, 0, zCoord) * noiseMul;
            
            unsigned int index = z * width + x;
            
            heightField[index] += Math.Round( (noise * 10.0) ) * 0.1;
            
            continue;
        }
        
        continue;
    }
    
    return;
}

void EngineSystemManager::AverageHeightFieldFromPerlinNoise(float* heightField, unsigned int width, unsigned int height, 
                                                            float noiseWidth, float noiseHeight, 
                                                            float noiseMul, int offsetX, int offsetZ) {
    
    for (unsigned int x=0; x < width; x ++) {
        
        for (unsigned int z=0; z < height; z ++) {
            
            float xCoord = ((float)x + offsetX) * noiseWidth;
            float zCoord = ((float)z + offsetZ) * noiseHeight;
            
            float noise = Random.Perlin(xCoord, 0, zCoord) * noiseMul;
            
            unsigned int index = z * width + x;
            
            float currentHeight = (heightField[index] + noise) / 2;
            
            heightField[index] = Math.Round( (currentHeight * 10.0) ) * 0.1;
            
            continue;
        }
        
        continue;
    }
    
    return;
}

void EngineSystemManager::GenerateColorFieldFromHeightField(glm::vec3* colorField, float* heightField, 
                                                            unsigned int width, unsigned int height, 
                                                            Color low, Color high, float bias) {
    
    for (unsigned int x=0; x < width; x ++) {
        
        for (unsigned int z=0; z < height; z ++) {
            
            unsigned int index = z * width + x;
            
            if (heightField[index] < 0) heightField[index] *= 0.3;
            
            float heightBias = heightField[index] * bias;
            
            Color color = low;
            
            // Caps
            if (heightBias < 0) heightBias = 0;
            if (heightBias > 1) heightBias = 1;
            
            color = Colors.Lerp(low, high, heightBias);
            
            float uniformVariantR = (Random.Range(0, 100) * 0.00001f) - (Random.Range(0, 10) * 0.00001f);
            float uniformVariantG = uniformVariantR;
            float uniformVariantB = uniformVariantR;
            
            color.r += uniformVariantR;
            color.g += uniformVariantG;
            color.b += uniformVariantB;
            
            // Apply the final color
            colorField[index] = glm::vec3( color.r, color.g, color.b );
            
            continue;
        }
        
        continue;
    }
    
    return;
}

void EngineSystemManager::SetColorFieldFromPerlinNoise(glm::vec3* colorField, unsigned int width, unsigned int height, 
                                                       float noiseWidth, float noiseHeight, float noiseThreshold, 
                                                       Color first, Color second, int offsetX, int offsetZ) {
    
    for (unsigned int x=0; x < width; x ++) {
        
        for (unsigned int z=0; z < height; z ++) {
            
            float xCoord = ((float)x + offsetX) * noiseWidth;
            float zCoord = ((float)z + offsetZ) * noiseHeight;
            
            float noise = Random.Perlin(xCoord, 0, zCoord) + noiseThreshold;
            
            if (noise > 1) noise = 1;
            if (noise < 0) noise = 0;
            
            Color result = Colors.Lerp(first, second, noise);
            
            unsigned int index = z * width + x;
            
            colorField[index] = glm::vec3(result.r, result.g, result.b);
            
            continue;
        }
        
        continue;
    }
    
    return;
}

void EngineSystemManager::AddColorFieldSnowCap(glm::vec3* colorField, float* heightField, 
                                               unsigned int width, unsigned int height, 
                                               Color capColor, float beginHeight, float bias) {
    
    for (unsigned int x=0; x < width; x ++) {
        
        for (unsigned int z=0; z < height; z ++) {
            
            unsigned int index = z * width + x;
            
            float heightBias = heightField[index] * 0.01;
            
            Color color( colorField[index].x, colorField[index].y, colorField[index].z );
            
            // Caps
            if (heightBias < 0) heightBias = 0;
            if (heightBias > 1) heightBias = 1;
            
            int diff = ((beginHeight - (beginHeight - 20)) - (heightField[index] - beginHeight)) * bias;
            
            if (Random.Range(0, 100) > diff) 
                color = Colors.Lerp(color, capColor, heightField[index] * 0.07);
            
            // Apply the final color
            colorField[index] = glm::vec3( color.r, color.g, color.b );
            
            continue;
        }
        
        continue;
    }
    
    return;
}

Mesh* EngineSystemManager::CreateMeshFromHeightField(float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ) {
    
    Mesh* mesh = Create<Mesh>();
    
    AddHeightFieldToMesh(mesh, heightField, colorField, width, height, offsetX, offsetZ);
    
    return mesh;
}

void EngineSystemManager::AddHeightFieldToMesh(Mesh* mesh, 
                                               float* heightField, glm::vec3* colorField, 
                                               unsigned int width, unsigned int height, 
                                               float offsetX, float offsetZ, 
                                               unsigned int subTessX, unsigned int subTessZ) {
    
    unsigned int fieldWidth = (width / subTessX) - 1;
    unsigned int fieldHeight = (height / subTessZ) - 1;
    
    unsigned int ww = width;
    unsigned int hh = height;
    
    float sx = 1.0f;
    float sz = 1.0f;
    
    if (subTessX > 1) 
        sx = subTessX * 4.0f;
    
    if (subTessX > 1) 
        sz = subTessZ * 4.0f;
    
    for (unsigned int x=0; x < fieldWidth; x ++) {
        
        for (unsigned int z=0; z < fieldHeight; z ++) {
            
            unsigned int xa = x * subTessX;
            unsigned int za = z * subTessZ;
            
            // Get height values
            float yyA = heightField[ za    * ww +  xa   ];
            float yyB = heightField[ za    * ww + (xa+1)];
            float yyC = heightField[(za+1) * ww + (xa+1)];
            float yyD = heightField[(za+1) * ww +  xa   ];
            
            //glm::vec3 cA = colorField[ za    * ww +  xa   ];
            //glm::vec3 cB = colorField[ za    * ww + (xa+1)];
            //glm::vec3 cC = colorField[(za+1) * ww + (xa+1)];
            //glm::vec3 cD = colorField[(za+1) * ww +  xa   ];
            
            glm::vec3 cA = colorField[za * ww + xa];
            glm::vec3 cB = colorField[za * ww + xa];
            glm::vec3 cC = colorField[za * ww + xa];
            glm::vec3 cD = colorField[za * ww + xa];
            
            // Calculate chunk position and offset
            float xx = ( ( ( (float)x + offsetX) - (float)ww / 2) / 2) + 0.25;
            float zz = ( ( ( (float)z + offsetZ) - (float)hh / 2) / 2) + 0.25;
            
            // Generate quad
            Vertex vertex[4];
            vertex[0] = Vertex( xx,    yyA, zz,     cA.x, cA.y, cA.z,   0, 1, 0,  0, 0 );
            vertex[1] = Vertex( xx+sx, yyB, zz,     cB.x, cB.y, cB.z,   0, 1, 0,  1, 0 );
            vertex[2] = Vertex( xx+sx, yyC, zz+sz,  cC.x, cC.y, cC.z,   0, 1, 0,  1, 1 );
            vertex[3] = Vertex( xx,    yyD, zz+sz,  cD.x, cD.y, cD.z,   0, 1, 0,  0, 1 );
            
            Vertex vertA = vertex[0];
            Vertex vertB = vertex[1];
            Vertex vertC = vertex[2];
            Vertex vertD = vertex[3];
            
            // Calculate vertex normals
            
            glm::vec3 U;
            glm::vec3 V;
            
            V.x = vertB.x - vertA.x;
            V.y = vertB.y - vertA.y;
            V.z = vertB.z - vertA.z;
            
            U.x = vertC.x - vertA.x;
            U.y = vertC.y - vertA.y;
            U.z = vertC.z - vertA.z;
            
            glm::vec3 normal;
            normal.x = (U.y * V.z) - (U.z * V.y);
            normal.y = (U.z * V.x) - (U.x * V.z);
            normal.z = (U.x * V.y) - (U.y * V.x);
            
            vertA.nx = normal.x;
            vertA.ny = normal.y;
            vertA.nz = normal.z;
            
            vertB.nx = normal.x;
            vertB.ny = normal.y;
            vertB.nz = normal.z;
            
            vertC.nx = normal.x;
            vertC.ny = normal.y;
            vertC.nz = normal.z;
            
            vertD.nx = normal.x;
            vertD.ny = normal.y;
            vertD.nz = normal.z;
            
            vertex[0] = vertA;
            vertex[1] = vertB;
            vertex[2] = vertC;
            vertex[3] = vertD;
            
            
            // Load the sub mesh into the buffer
            SubMesh subBuffer;
            subBuffer.vertexBuffer.push_back(vertex[0]);
            subBuffer.vertexBuffer.push_back(vertex[1]);
            subBuffer.vertexBuffer.push_back(vertex[2]);
            subBuffer.vertexBuffer.push_back(vertex[3]);
            
            subBuffer.indexBuffer.push_back(0);
            subBuffer.indexBuffer.push_back(2);
            subBuffer.indexBuffer.push_back(1);
            
            subBuffer.indexBuffer.push_back(0);
            subBuffer.indexBuffer.push_back(3);
            subBuffer.indexBuffer.push_back(2);
            
            mesh->AddSubMesh(xx, 0, zz, subBuffer.vertexBuffer, subBuffer.indexBuffer, false);
            
            continue;
        }
        
        continue;
    }
    
    return;
}


Button* EngineSystemManager::CreateOverlayButtonCallback(int x, int y, int width, int height, ButtonCallBack callback) {
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

void EngineSystemManager::AddMeshText(GameObject* overlayObject, float xPos, float yPos, float width, float height, std::string text, Color textColor) {
    
    Mesh* meshPtr = overlayObject->GetComponent<MeshRenderer>()->mesh;
    if (meshPtr == nullptr) 
        return;
    
    for (unsigned int i=0; i < text.size(); i++)
        AddMeshSubSprite(overlayObject, xPos + i, yPos, width, height, text[i], textColor);
    
    meshPtr->Load();
    
    return;
}

void EngineSystemManager::AddMeshSubSprite(GameObject* overlayObject, float xPos, float yPos, float width, float height, int index, Color meshColor) {
    
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
        
        sceneOverlay->AddMeshRendererToSceneRoot( meshRenderer, RENDER_QUEUE_OVERLAY );
        
        mConsoleText[i] = mConsoleTextObjects[i]->GetComponent<Text>();
        mConsoleText[i]->canvas.anchorTop = false;
        
        mConsoleText[i]->canvas.x = 0;
        mConsoleText[i]->canvas.y = -(2 * i + 4);
        
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
    
    debugMesh  = Create<Mesh>();
    debugLines = Create<Mesh>();
    
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
    
    
    MeshRenderer* lineRenderer = Renderer.CreateMeshRenderer();
    lineRenderer->mesh     = debugLines;
    lineRenderer->material = debugMaterialLines;
    
    MeshRenderer* triangleRenderer = Renderer.CreateMeshRenderer();
    triangleRenderer->mesh     = debugMesh;
    triangleRenderer->material = debugMaterialMesh;
    
    
    sceneMain->AddMeshRendererToSceneRoot( lineRenderer );
    
    sceneMain->AddMeshRendererToSceneRoot( triangleRenderer );
    
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
    
    debugLines->RemoveSubMesh(0);
    
    debugLines->AddSubMesh(0, 0, 0, subMeshLines, true);
    
    
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
    
    debugMesh->RemoveSubMesh(0);
    
    debugMesh->AddSubMesh(0, 0, 0, subMeshTriangles, true);
    
    return;
}

