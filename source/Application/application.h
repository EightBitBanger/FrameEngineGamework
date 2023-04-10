



void CameraMovementScript(void) {
    
    float cameraSpeed = 100;
    
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






void Start(void) {
    
    Resources.LoadScene("data/main.scene");
    
    
    
    // Main scene
    currentScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(currentScene);
    
    
    
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->background = Colors.black;
    
    
    
    // Camera
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->transform.position = glm::vec3(-20, 1, 0);
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Renderer.cameraMain->script = Renderer.CreateScript();
    Renderer.cameraMain->script->OnUpdate = CameraMovementScript;
    
    
    
    
    
    // Mesh
    mesh = Renderer.CreateMesh();
    mesh->SetDefaultAttributes();
    mesh->shader = Resources.CreateShaderFromTag("default");
    if (mesh->shader == nullptr) return;
    
    meshSource = Resources.CreateMeshFromTag("Barrel");
    if (meshSource == nullptr) return;
    
    SubMesh subPart;
    meshSource->CopySubMesh(0, subPart);
    
    for (int i=0; i < 100; i++) {
        float xx = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        float yy = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        float zz = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        
        mesh->AddSubMesh(xx, yy, zz, subPart.vertexBuffer, subPart.indexBuffer);
    }
    
    
    
    Entity* entity = Renderer.CreateEntity();
    entity->material = Resources.CreateMaterialFromTag("matBarrel");
    if (entity->material == nullptr) return;
    
    entity->mesh = mesh;
    entity->material->color = Color(0, 0, 0, 1);
    currentScene->AddToSceneRoot(entity);
    
    
}


void Run(void) {
    
    {
        float xx = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        float yy = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        float zz = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        
        unsigned int randMesh = Random.Range(0, mesh->GetSubMeshCount());
        mesh->ChangeSubMeshPosition(randMesh, xx, yy, zz);
        
        randMesh = Random.Range(0, mesh->GetSubMeshCount());
        mesh->ChangeSubMeshColor(randMesh, Colors.MakeRandom());
        
    }
    
    
    
    
    {
        float xx = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        float yy = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        float zz = (Random.Range(0, 100) - Random.Range(0, 100)) * 0.7;
        
        mesh->AddSubMesh(xx, yy, zz, meshSource->vertexBuffer, meshSource->indexBuffer);
        
        if (mesh->GetSubMeshCount() > 200) {
            for (int i=0; i < 10; i++) {
                unsigned int randMesh = Random.Range(0, mesh->GetSubMeshCount());
                mesh->RemoveSubMesh(randMesh);
            }
        }
        
    }
    
    return;
}


void Shutdown(void) {
    
    return;
}


