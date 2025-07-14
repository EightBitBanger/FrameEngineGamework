#include <GameEngineFramework/Engine/Engine.h>


EngineSystemManager::EngineSystemManager(void) : 
    sceneMain(nullptr),
    sceneOverlay(nullptr),
    cameraController(nullptr),
    
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

GameObject* EngineSystemManager::GetGameObject(unsigned int index) {
    if (index < mGameObjects.Size()) 
        return mGameObjects[index];
    return nullptr;
}

rp3d::BoxShape* EngineSystemManager::GetColliderBox(glm::vec3 extents) {
    for (unsigned int i=0; i < mBoxCollider.size(); i++) {
        rp3d::BoxShape* colliderShape = mBoxCollider[i];
        rp3d::Vector3 extentItem = colliderShape->getHalfExtents();
        if (extentItem.x == extents.x && 
            extentItem.y == extents.y && 
            extentItem.z == extents.z) 
            return colliderShape;
    }
    rp3d::Vector3 newExtent(extents.x, extents.y, extents.z);
    rp3d::BoxShape* colliderShape = Physics.common.createBoxShape(newExtent);
    
    mBoxCollider.push_back( colliderShape );
    return colliderShape;
}


void EngineSystemManager::Initiate() {
    
    // Initiate console
    console.input = UI.CreateTextField();
    console.input->doStayInFocus = true;
    
    // Initiate console 
    for (unsigned int i=0; i < 32; i++) {
        Text* text = UI.CreateText();
        text->x = 5;
        text->y = Platform.windowArea.h - (90 + (16 * i));
        
        text->text = "";
        text->color.a = 3.0f;
        text->doFadeout = true;
        
        console.textElements.push_back(text);
        
    }
    
    // Initiate dialog
    for (unsigned int i=0; i < 32; i++) {
        Text* text = UI.CreateText();
        text->x = 5;
        text->y = 16 * i;
        
        text->text = "";
        text->color.a = 3.0f;
        text->doFadeout = true;
        
        console.textDialog.push_back(text);
    }
    
    // Return callback
    extern void ConsoleReturnCallback(std::string& console_text);
    console.input->callback = &ConsoleReturnCallback;
    console.doCloseConsoleAfterCommand = true;
    console.Disable();
    
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
    meshes.grassHorz->isShared         = true;
    meshes.grassVert->isShared         = true;
    meshes.stemHorz->isShared          = true;
    meshes.stemVert->isShared          = true;
    meshes.wallHorizontal->isShared    = true;
    meshes.wallVertical->isShared      = true;
    meshes.log->isShared               = true;
    
    meshes.cube->isShared              = true;
    meshes.chunk->isShared             = true;
    meshes.plain->isShared             = true;
    meshes.sphere->isShared            = true;
    
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

