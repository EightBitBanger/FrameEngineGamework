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



Scene* plainScene;

GameObject* cameraController = nullptr;


//
// Application entry point
//

void Framework::Start() {
    
    Resources.LoadScene("data/main.scene");
    
    Physics.SetWorldGravity(0, 0, 0);
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->SetColor( Colors.Make(0.087, 0.087, 0.087) );
    
    
    // Camera controller
    cameraController = Engine.CreateCameraController(-50, 0, 0);
    cameraController->GetAttachedScript()->OnUpdate = ScriptCameraController;
    
    
    
    // Create objects from resource tags
    barrelMesh = Resources.CreateMeshFromTag("barrel");
    
    barrelMesh->ChangeSubMeshColor(0, Colors.ltgray);
    
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    
    
    
    
    plainScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(plainScene);
    
    
    return;
}






float forceMul  = 50;
float torqueMul = 5;


//
// Application main loop
//

void Framework::Run() {
    
    
    for (unsigned int i=0; i < 10; i++) {
        
        GameObject* gameObject = Engine.CreateGameObject();
        
        rp3d::RigidBody* rigidBody = Engine.CreateRigidBody();
        gameObject->AttachRidigBody(rigidBody);
        
        Entity* entityRenderer = Engine.CreateEntityRenderer(barrelMesh, barrelMaterial);
        gameObject->AttachEntity(entityRenderer);
        
        float forcex = (Random.Range(0, 100) - Random.Range(0, 100)) * forceMul;
        float forcey = (Random.Range(0, 100) - Random.Range(0, 100)) * forceMul;
        float forcez = (Random.Range(0, 100) - Random.Range(0, 100)) * forceMul;
        
        float torquex = (Random.Range(0, 100) - Random.Range(0, 100)) * torqueMul;
        float torquey = (Random.Range(0, 100) - Random.Range(0, 100)) * torqueMul;
        float torquez = (Random.Range(0, 100) - Random.Range(0, 100)) * torqueMul;
        
        gameObject->AddForce(forcex, forcey, forcez);
        gameObject->AddTorque(torquex, torquey, torquez);
    }
    
    
    
    while (Engine.GetGameObjectCount() > 2000) {
        
        GameObject* oldObject = Engine.GetGameObject(0);
        
        if (oldObject->name == "camera") 
            oldObject = Engine.GetGameObject(1);
        
        rp3d::RigidBody* body   = oldObject->GetAttachedRidigBody();
        Entity*          entity = oldObject->GetAttachedEntity();
        
        Engine.DestroyGameObject(oldObject);
        
    }
    
    
    
    
    
    
    
    //
    // Escape key pause
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
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








