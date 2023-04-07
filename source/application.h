//
// Application entry point


struct MyApplication : ApplicationLayer {
    void Start(void);
    void Run(void);
    void Shutdown(void);
};
MyApplication Application;



void CameraMovement(void) {
    
    float cameraSpeed = 80;
    
    if (Input.CheckKeyCurrent(VK_W)) Renderer.cameraMain->transform.position += Renderer.cameraMain->forward * cameraSpeed * Time.delta * glm::vec3(1, 0.2, 1);
    if (Input.CheckKeyCurrent(VK_S)) Renderer.cameraMain->transform.position -= Renderer.cameraMain->forward * cameraSpeed * Time.delta * glm::vec3(1, 0.2, 1);
    if (Input.CheckKeyCurrent(VK_A)) Renderer.cameraMain->transform.position += Renderer.cameraMain->right * cameraSpeed * Time.delta;
    if (Input.CheckKeyCurrent(VK_D)) Renderer.cameraMain->transform.position -= Renderer.cameraMain->right * cameraSpeed * Time.delta;
    
    if (Input.CheckKeyCurrent(VK_SPACE)) Renderer.cameraMain->transform.position += Renderer.cameraMain->up * cameraSpeed * Time.delta;
    if (Input.CheckKeyCurrent(VK_SHIFT)) Renderer.cameraMain->transform.position -= Renderer.cameraMain->up * cameraSpeed * Time.delta;
    
};





Mesh*      mesh;
Mesh*      meshSource;

Material*  material;
Scene*     currentScene;






void MyApplication::Start(void) {
    
    
    Resources.LoadShaderGLSL("data/default.shader", "default");
    Resources.LoadWaveFront("data/barrel.obj", "Barrel");
    
    Resources.LoadTexture("data/barrel.png", "matBarrel");
    Resources.LoadTexture("data/grassy.png", "matGround");
    
    
    
    // Main scene
    currentScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(currentScene);
    
    
    Physics.SetGravity(0, 0, 0);
    
    
    Renderer.Blending.Enable();
    Renderer.Blending.SetFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Renderer.DepthTest.Enable();
    
    
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->background = Colors.black;
    
    
    
    // Camera
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->transform.position = glm::vec3(-20, 1, 0);
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Renderer.cameraMain->script = Renderer.CreateScript();
    Renderer.cameraMain->script->OnUpdate = CameraMovement;
    
    
    
    
    
    // Mesh
    mesh = Renderer.CreateMesh();
    mesh->SetDefaultAttributes();
    mesh->shader = Resources.CreateShaderFromTag("default");
    if (mesh->shader == nullptr) return;
    
    meshSource = Resources.CreateMeshFromTag("Barrel");
    if (meshSource == nullptr) return;
    
    std::vector<Vertex> vertexBuff;
    std::vector<Index>  indexBuff;
    meshSource->CopySubMesh(0, vertexBuff, indexBuff);
    
    mesh->AddSubMesh(0, 0, -10, vertexBuff, indexBuff);
    mesh->AddSubMesh(0, 0,   0, vertexBuff, indexBuff);
    mesh->AddSubMesh(0, 0,  10, vertexBuff, indexBuff);
    
    mesh->UpdateMesh();
    
    
    
    
    Entity* entity = Renderer.CreateEntity();
    entity->material = Resources.CreateMaterialFromTag("matBarrel");
    if (entity->material == nullptr) return;
    
    entity->mesh = mesh;
    entity->material->color = Color(0, 0, 0, 1);
    currentScene->AddToSceneRoot(entity);
    
    return;
}




void MyApplication::Run(void) {
    
    if (meshSource == nullptr) return;
    if (mesh == nullptr) return;
    
    
    if (Input.CheckKeyCurrent(VK_I)) {
        
        
        float x = (Random.Range(1, 100) * 0.8) - (Random.Range(1, 100) * 0.8);
        float y = (Random.Range(1, 100) * 0.8) - (Random.Range(1, 100) * 0.8);
        float z = (Random.Range(1, 100) * 0.8) - (Random.Range(1, 100) * 0.8);
        
        SubMesh newMesh;
        
        meshSource->CopySubMesh(0, newMesh);
        
        mesh->AddSubMesh(x, y, z, newMesh.vertexBuffer, newMesh.indexBuffer);
    }
    
    
    
    
    
    
    if (Input.CheckKeyCurrent(VK_K)) {
        
        unsigned int randMesh = Random.Range(0, mesh->GetSubMeshCount());
        
        mesh->RemoveSubMesh(randMesh);
        
    }
    
    
    
    
    if (Input.CheckKeyCurrent(VK_C)) {
        
        unsigned int randMesh = Random.Range(0, mesh->GetSubMeshCount());
        
        float xx = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        float yy = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        float zz = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        
        mesh->ChangeSubMeshPosition(randMesh, xx, yy, zz);
        
    }
    
    if (Input.CheckKeyCurrent(VK_V)) {
        
        unsigned int randMesh = Random.Range(0, mesh->GetSubMeshCount());
        
        mesh->ChangeSubMeshColor(randMesh, Colors.MakeRandom());
        
    }
    
    
    
    if (Input.CheckKeyPressed(VK_P)) {
        
        mesh->AddPlane(0, -20, 0, 100, 100, Colors.white);
        mesh->UpdateMesh();
    }
    
    
    
    return;
    
    
    
    
    
    
    int   spawnCount   = 3;
    int   spawnMax     = 3000;
    
    float spawnScale   = 100;
    
    float forceMul     = 100;
    float torqueMul    = 30;
    
    
    for (int i=0; i < spawnCount; i++) {
        
        Entity* entity = Renderer.CreateEntity();
        entity->mesh = mesh;
        
        float xscale = (Random.Range(1.0, spawnScale) - Random.Range(1.0, spawnScale)) * 0.001;
        float yscale = (Random.Range(1.0, spawnScale) - Random.Range(1.0, spawnScale)) * 0.001;
        float zscale = (Random.Range(1.0, spawnScale) - Random.Range(1.0, spawnScale)) * 0.001;
        entity->transform.scale    = glm::vec3(xscale, yscale, zscale);
        
        entity->material = Renderer.CreateMaterial();
        entity->material->color = Colors.ltgray;
        
        entity->rigidBody = Physics.CreateRigidBody(0, 0, 0);
        
        
        
        
        float xforce = Random.Range(1.0, forceMul) - Random.Range(1.0, forceMul);
        float yforce = Random.Range(1.0, forceMul) - Random.Range(1.0, forceMul);
        float zforce = Random.Range(1.0, forceMul) - Random.Range(1.0, forceMul);
        
        float xtorque = Random.Range(1.0, torqueMul) - Random.Range(1.0, torqueMul);
        float ytorque = Random.Range(1.0, torqueMul) - Random.Range(1.0, torqueMul);
        float ztorque = Random.Range(1.0, torqueMul) - Random.Range(1.0, torqueMul);
        
        
        entity->AddForce(xforce, yforce, zforce);
        entity->AddTorque(xtorque, ytorque, ztorque);
        
        currentScene->AddToSceneRoot(entity);
    }
    
    
    
    
    
    if (currentScene->GetRenderQueueSize() > (unsigned int)spawnMax) {
        
        for (int i=0; i < spawnCount*2; i++) {
            Entity* entity = currentScene->entityQueue[0];
            
            currentScene->RemoveFromSceneRoot(entity);
            
            Renderer.DestroyMaterial(entity->material);
            
            Renderer.DestroyEntity(entity);
        }
        
    }
    
    
    
    
    
    
    
    
    
    if (Renderer.cameraMain == nullptr) return;
    
    
    
}




void MyApplication::Shutdown(void) {
    
    
    
}








