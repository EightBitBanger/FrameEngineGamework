#include "application/application.h"

#include "Renderer/RenderSystem.h"
#include "Input/InputSystem.h"
#include "Physics/PhysicsSystem.h"
#include "Resources/ResourceManager.h"
#include "Engine/EngineMain.h"

extern RandomGen         Random;
extern ColorPreset       Colors;
extern Timer             Time;
extern Logger            Log;

extern ResourceManager   Resources;
extern RenderSystem      Renderer;
extern PhysicsSystem     Physics;
extern InputSystem       Input;

extern EngineMainAPI     Engine;



// User native scripts
void CameraMovementScript(void);




Mesh*     barrelMesh;
Material* barrelMaterial;

Scene* objectsScene;


void Start(void) {
    
    //
    // Load some initial resources
    
    Resources.LoadScene("data/main.scene");
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->SetColor( Colors.Make(0.087, 0.087, 0.087) );
    
    // Camera
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->transform.position = glm::vec3(-120, 20, 0);
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Renderer.cameraMain->script = Renderer.CreateScript();
    Renderer.cameraMain->script->OnUpdate = CameraMovementScript;
    
    
    barrelMesh = Resources.CreateMeshFromTag("barrel");
    barrelMesh->SetDefaultAttributes();
    barrelMesh->shader = Renderer.defaultShader;
    barrelMesh->ChangeSubMeshColor(0, Colors.ltgray);
    
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    
    // Plain scene
    Scene* plainScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(plainScene);
    
    
    // Create a plain
    Entity* plain = Renderer.CreateEntity();
    plainScene->AddToSceneRoot(plain);
    
    plain->mesh = Renderer.CreateMesh();
    plain->mesh->SetDefaultAttributes();
    plain->mesh->shader = Renderer.defaultShader;
    plain->mesh->AddPlain(0, 0, 0, 100, 100, Colors.gray);
    
    plain->material = Resources.CreateMaterialFromTag("mat_plain");
    plain->rigidBody = Physics.CreateRigidBody(0, 0, 0);
    plain->SetRigidBodyStatic();
    
    plain->AddCollider( Resources.FindColliderTag("coll_plain"), 0, -10.0, 0);
    
    
    // Objects scene
    objectsScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(objectsScene);
    
    return;
}

float spreadMul   = 1.0;
int counter=0;

void Run(void) {
    if (counter == 100) return;
    
    //if (!Input.CheckKeyCurrent(VK_RETURN)) 
    //    return;
    
    if (Random.Range(1, 2) == 1) {
        
        counter++;
        if (counter > 100) 
            counter = 100;
        
        float xx = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        float yy = (Random.Range(0, 10)  - Random.Range(0, 10))  * spreadMul;
        float zz = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        
        // Create a barrel object
        Entity* barrel = Renderer.CreateEntity();
        objectsScene->AddToSceneRoot(barrel);
        
        barrel->mesh = barrelMesh;
        
        barrel->material = barrelMaterial;
        barrel->rigidBody = Physics.CreateRigidBody(xx, yy + 100, zz);
        barrel->SetRigidBodyStatic();
        
        barrel->AddCollider( Resources.FindColliderTag("coll_barrel"), 0, 0, 0);
        
        
    }
    
    return;
}




void Shutdown(void) {
    
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







