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


unsigned int EngineSystemManager::GetNumberOfComponents(void) {
    return mComponents.Size();
}







//
// Generation
//


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

