#include "Engine/Engine.h"

extern RandomGen         Random;
extern ColorPreset       Colors;
extern Timer             Time;
extern Timer             PhysicsTime;
extern Logger            Log;

extern EngineSystemManager  Engine;
extern ApplicationLayer     Application;
extern ResourceManager      Resources;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;



// User scripts
void ScriptCameraController(void* gameObject);


// Cached resource pointers
Mesh*     barrelMesh;
Material* barrelMaterial;

GameObject* cameraController = nullptr;





//
// Application entry point
//

void Framework::Start() {
    
    Resources.LoadScene("data/main.scene");
    
    Physics.SetWorldGravity(0, -0, -1000);
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->SetColor( Colors.Make(0.087, 0.087, 0.087) );
    
    
    // Create objects from resource tags
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    barrelMesh = Resources.CreateMeshFromTag("barrel");
    barrelMesh->ChangeSubMeshColor(0, Colors.gray);
    
    
    
    //
    // Create a camera controller
    
    cameraController = Engine.CreateCameraController(-200, 0, 0);
    Component* scriptComponent = cameraController->FindComponent(COMPONENT_TYPE_SCRIPT);
    
    Script* cameraScript = (Script*)scriptComponent->GetComponent();
    cameraScript->OnUpdate = ScriptCameraController;
    
    
    
    
    return;
}






float spreadMul    = 0.3;
float spawnHeight  = 100;

float forceMul   = 100;
float torqueMul  = 3;




//
// Application main loop
//

void Framework::Run() {
    
    for (int i=0; i < 100; i++) {
        
        //
        // Create some barrel objects in random positions
        
        GameObject* barrel = Engine.CreateGameObject();
        
        
        // Add a renderer component
        Component* entityRenderer = Engine.CreateComponent(COMPONENT_TYPE_RENDERER);
        barrel->AddComponent(entityRenderer);
        
        Entity* entity = (Entity*)entityRenderer->GetComponent();
        entity->AttachMesh(barrelMesh);
        entity->AttachMaterial(barrelMaterial);
        
        
        // Add a physics component
        Component* rigidBodyComponent = Engine.CreateComponent(COMPONENT_TYPE_RIGIDBODY);
        barrel->AddComponent(rigidBodyComponent);
        
        //rp3d::RigidBody* body = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
        
        float startx = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        float starty = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        float startz = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        
        float forcex = (Random.Range(0, 100) - Random.Range(0, 100)) * forceMul;
        float forcey = (Random.Range(0, 100) - Random.Range(0, 100)) * forceMul;
        float forcez = (Random.Range(0, 100) - Random.Range(0, 100)) * forceMul;
        
        float torquex = (Random.Range(0, 100) - Random.Range(0, 100)) * torqueMul;
        float torquey = (Random.Range(0, 100) - Random.Range(0, 100)) * torqueMul;
        float torquez = (Random.Range(0, 100) - Random.Range(0, 100)) * torqueMul;
        
        barrel->SetPosition(startx, starty + spawnHeight, startz);
        barrel->AddForce(forcex, forcey, forcez);
        barrel->AddTorque(torquex, torquey, torquez);
        
        continue;
    }
    
    
    while (Engine.GetGameObjectCount() > 1000) {
        
        GameObject* gameObject = Engine.GetGameObject(0);
        Component* cameraComponent = gameObject->FindComponent(COMPONENT_TYPE_CAMERA);
        if (cameraComponent != nullptr) 
            gameObject = Engine.GetGameObject(1);
        
        Engine.DestroyGameObject(gameObject);
        
    }
    
    
    
    
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








