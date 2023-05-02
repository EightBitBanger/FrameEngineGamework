#include "Engine/Engine.h"

extern RandomGen         Random;
extern ColorPreset       Colors;
extern Timer             Time;
extern Timer             PhysicsTime;
extern Logger            Log;

extern EngineSystemManager  Engine;
extern ApplicationLayer     Application;
extern ResourceManager      Resources;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;
extern ScriptSystem         Scripting;



// User scripts
void ScriptCameraController(void* gameObject);



// Cached resource pointers
Scene*    objectScene;

Mesh*     barrelMesh;
Material* barrelMaterial;




GameObject* cameraController = nullptr;


//
// Application entry point
//

void Framework::Start() {
    
    Resources.LoadScene("data/main.scene");
    
    Physics.SetWorldGravity(0, -90, 0);
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->SetColor( Colors.Make(0.087, 0.087, 0.087) );
    
    
    // Camera controller
    cameraController = Engine.CreateCameraController(-50, 0, 0);
    cameraController->GetAttachedScript()->OnUpdate = ScriptCameraController;
    
    
    
    // Create objects from resource tags
    barrelMesh = Resources.CreateMeshFromTag("barrel");
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    barrelMesh->ChangeSubMeshColor(0, Colors.yellow);
    
    
    
    
    GameObject* plain = Engine.CreateGameObject();
    Entity* renderer = Engine.CreateEntityRenderer(barrelMesh, barrelMaterial);
    plain->AttachEntity(renderer);
    
    rp3d::RigidBody* body = Engine.CreateRigidBody(0, 0, 0);
    //plain->AttachRidigBody(body);
    
    
    
    
    return;
}






float forceMul  = 50;
float torqueMul = 5;


//
// Application main loop
//

void Framework::Run() {
    
    
    
    //
    // Escape key pause
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        //Application.isActive = false;
        
        Application.Pause();
        
        if (Application.isPaused) {
            
            if (Renderer.cameraMain != nullptr) 
                Renderer.cameraMain->DisableMouseLook();
            
            Input.ClearKeys();
        } else {
            
            if (Renderer.cameraMain != nullptr) {
                Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
                Renderer.cameraMain->EnableMouseLook();
            }
            
            Time.Update();
            PhysicsTime.Update();
        }
        
    }
    
    
    return;
}




void Framework::Shutdown(void) {
    
    return;
}





void ScriptCameraController(void* gameObjectPtr) {
    
    GameObject* gameObject = (GameObject*)gameObjectPtr;
    
    float cameraSpeed = 1000;
    
    glm::vec3 force(0);
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    force *= cameraSpeed;
    
    gameObject->AddForce(force.x, force.y, force.z);
    
    return;
};








