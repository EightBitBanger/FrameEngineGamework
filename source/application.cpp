#include "application/application.h"

#include "Renderer/RenderSystem.h"
#include "Input/InputSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Resources/ResourceManager.h"

extern RandomGen         Random;
extern ColorPreset       Colors;
extern Timer             Time;
extern Logger            Log;

extern ResourceManager   Resources;
extern RenderSystem      Renderer;
extern PhysicsSystem     Physics;
extern InputSystem       Input;



// User native scripts
void CameraMovementScript(void);








void Start(void) {
    
    Resources.Initiate();
    
    //Resources.LoadScene("data/main.scene");
    
    Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    
    Resources.LoadTexture("data/barrel/barrel.png", "matBarrel");
    
    Resources.LoadShaderGLSL("data/default.shader", "default");
    
    
    
    
    // Main scene
    Scene* currentScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(currentScene);
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->background = Colors.Make(0.087, 0.087, 0.087);
    
    // Camera
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->transform.position = glm::vec3(-40, 2, 0);
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Renderer.cameraMain->script = Renderer.CreateScript();
    Renderer.cameraMain->script->OnUpdate = CameraMovementScript;
    
    
    
    Entity* entity = Renderer.CreateEntity();
    
    Mesh* mesh = Renderer.CreateMesh();
    entity->mesh = mesh;
    entity->mesh->SetDefaultAttributes();
    
    entity->mesh->shader = Resources.CreateShaderFromTag("default");
    if (entity->mesh->shader == nullptr) return;
    
    entity->material = Resources.CreateMaterialFromTag("matBarrel");
    if (entity->material == nullptr) return;
    
    entity->material->color = Color(0, 0, 0, 1);
    
    
    currentScene->AddToSceneRoot(entity);
    
    
    return;
}





unsigned int gridSize    = 8;
unsigned int gridSpacing = 10;

bool endRoutine = false;
unsigned int i=0;
unsigned int a=0;


void Run(void) {
    
    
    Scene* scene = Renderer.GetScene(0);
    if (scene == nullptr) return;
    
    Entity* entity = scene->GetEntity(0);
    if (entity == nullptr) return;
    
    Mesh* mesh = entity->mesh;
    
    unsigned int index = Random.Range( 0, mesh->GetSubMeshCount() );
    mesh->ChangeSubMeshColor(index, Colors.MakeRandom() );
    
    if (endRoutine) return;
    
    // Get the loaded model as a sub mesh and
    // create a grid of sub meshes in the mesh
    SubMesh subPartSource = Resources.GetSubMeshFromTag("barrel");
    
    if (i > gridSize) {
        i=0; a++;
        if (a > gridSize) {
            endRoutine = true;
            return;
        }
    }i++;
    
    mesh->AddSubMesh(gridSpacing * a, 0, gridSpacing * i, subPartSource);
    
    
    
    
    
    
    
    
    
    
    
    
    
    //index = Random.Range( 0, mesh->GetSubMeshCount() );
    //float xx = (Random.Range(1, 100) - Random.Range(1, 100)) * 1;
    //float yy = (Random.Range(1, 100) - Random.Range(1, 100)) * 1;
    //float zz = (Random.Range(1, 100) - Random.Range(1, 100)) * 1;
    
    //mesh->ChangeSubMeshPosition(index, xx, yy, zz);
    
    return;
}




void Shutdown(void) {
    
    
    Resources.DestroyAssets();
    
    return;
}







void CameraMovementScript(void) {
    
    float cameraSpeed = 100;
    
    if (Input.CheckKeyCurrent(VK_W)) Renderer.cameraMain->transform.position += Renderer.cameraMain->forward * cameraSpeed * Time.delta * glm::vec3(1, 0.2, 1);
    if (Input.CheckKeyCurrent(VK_S)) Renderer.cameraMain->transform.position -= Renderer.cameraMain->forward * cameraSpeed * Time.delta * glm::vec3(1, 0.2, 1);
    if (Input.CheckKeyCurrent(VK_A)) Renderer.cameraMain->transform.position += Renderer.cameraMain->right * cameraSpeed * Time.delta;
    if (Input.CheckKeyCurrent(VK_D)) Renderer.cameraMain->transform.position -= Renderer.cameraMain->right * cameraSpeed * Time.delta;
    
    if (Input.CheckKeyCurrent(VK_SPACE)) Renderer.cameraMain->transform.position += Renderer.cameraMain->up * cameraSpeed * Time.delta;
    if (Input.CheckKeyCurrent(VK_SHIFT)) Renderer.cameraMain->transform.position -= Renderer.cameraMain->up * cameraSpeed * Time.delta;
    
    return;
};







