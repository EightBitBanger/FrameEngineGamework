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


// Cached resources
Mesh*     barrelMesh;
Mesh*     projectileMesh;
Material* barrelMaterial;






//
// Application entry point
//

GameObject* cameraController;

void Framework::Start() {
    
    // Load some external files
    Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    Resources.LoadTexture("data/barrel/barrel.png", "mat_barrel");
    
    // Set the gravity vector for the simulation
    Physics.SetWorldGravity(0, -0, 0);
    
    // Create objects from resource tags
    Material* barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    Mesh*     barrelMesh     = Resources.CreateMeshFromTag("barrel");
    
    
    // Create a game object
    GameObject* barrel = Engine.CreateGameObject();
    
    // Add a render component
    Component* entityRenderer = Engine.CreateComponentEntityRenderer(barrelMesh, barrelMaterial);
    barrel->AddComponent(entityRenderer);
    
    // Add a physics component
    Component* rigidBodyComponent = Engine.CreateComponent(COMPONENT_TYPE_RIGIDBODY);
    barrel->AddComponent(rigidBodyComponent);
    
    
    // Create a camera controller
    cameraController = Engine.CreateCameraController(-50, 0, 0);
    
    
    return;
    
    
    
    /*
    
    //
    // Stress test
    //
    
    //Physics.SetWorldGravity(0, -0, -700);
    Physics.SetWorldGravity(0, -0, 0);
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->SetColor( Colors.Make(0.087, 0.087, 0.087) );
    
    
    // Create objects from resource tags
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    // Load a barrel mesh
    barrelMesh = Resources.CreateMeshFromTag("barrel");
    barrelMesh->ChangeSubMeshColor(0, Colors.gray);
    
    // Load a mesh to serve as a projectile
    projectileMesh = Resources.CreateMeshFromTag("barrel");
    projectileMesh->ChangeSubMeshColor(0, Colors.green);
    
    
    
    //
    // Create a camera controller
    
    cameraController = Engine.CreateCameraController(-200, 0, 0);
    cameraController->name = "camera";
    Component* scriptComponent = cameraController->FindComponent(COMPONENT_TYPE_SCRIPT);
    
    Script* cameraScript = (Script*)scriptComponent->GetComponent();
    cameraScript->OnUpdate = ScriptCameraController;
    
    */
    
    
    
    
    return;
}






float spreadMul    = 0.1;
float spawnHeight  = 70;

float forceMul   = 10;
float torqueMul  = 1;




//
// Application main loop
//

void Framework::Run() {
    
    glm::vec3 force(0);
    
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    // Camera speed
    force *= 1000;
    
    cameraController->AddForce(force.x, force.y, force.z);
    
    
    
    
    
    return;
    
    //
    // Create some barrel objects in random positions
    
    for (int i=0; i < 50; i++) {
        
        // Create a game object
        GameObject* particle = Engine.CreateGameObject();
        particle->name = "particle";
        
        // Add a renderer component
        particle->AddComponent( Engine.CreateComponentEntityRenderer(barrelMesh, barrelMaterial) );
        
        // Add a physics component
        particle->AddComponent( Engine.CreateComponent(COMPONENT_TYPE_RIGIDBODY) );
        
        
        // Apply some physical forces
        
        float startx = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        float starty = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        float startz = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        
        particle->SetPosition(startx, starty + spawnHeight, startz);
        
        float forcex = (Random.Range(0, 100) - Random.Range(0, 100)) * forceMul;
        float forcey = (Random.Range(0, 100) - Random.Range(0, 100)) * forceMul;
        float forcez = (Random.Range(0, 100) - Random.Range(0, 100)) * forceMul;
        
        particle->AddForce(forcex, forcey, forcez);
        
        float torquex = (Random.Range(0, 100) - Random.Range(0, 100)) * torqueMul;
        float torquey = (Random.Range(0, 100) - Random.Range(0, 100)) * torqueMul;
        float torquez = (Random.Range(0, 100) - Random.Range(0, 100)) * torqueMul;
        
        particle->AddTorque(torquex, torquey, torquez);
        
        continue;
    }
    
    unsigned int index=0;
    
    while (Engine.GetGameObjectCount() > 1000) {
        
        
        GameObject* gameObject = Engine.GetGameObject(index);
        index++;
        
        // Ignore cameras and projectiles
        if (gameObject->name == "projectile") 
            continue;
        if (gameObject->name == "camera") 
            continue;
        
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





float cameraSpeed = 1500;

void ScriptCameraController(void* gameObjectPtr) {
    GameObject* gameObject = (GameObject*)gameObjectPtr;
    
    glm::vec3 force(0);
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    //
    // Shoot object from camera
    
    if (Input.CheckMouseLeftPressed()) {
        Input.SetMouseLeftPressed(false);
        
        GameObject* projectile = Engine.CreateGameObject();
        projectile->name = "projectile";
        
        // Add a render component
        Component* entityRenderer = Engine.CreateComponent(COMPONENT_TYPE_RENDERER);
        projectile->AddComponent(entityRenderer);
        
        Entity* entity = (Entity*)entityRenderer->GetComponent();
        entity->AttachMesh(projectileMesh);
        entity->AttachMaterial(barrelMaterial);
        
        
        // Add a physics component
        Component* rigidBodyComponent = Engine.CreateComponent(COMPONENT_TYPE_RIGIDBODY);
        projectile->AddComponent(rigidBodyComponent);
        rp3d::RigidBody* body = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
        
        
        //
        // Calculate projectile force
        
        glm::vec3 fwd = Renderer.cameraMain->forward;
        glm::vec3 fwdAngle = Renderer.cameraMain->forward;
        fwd *= 3; // Start offset from camera
        
        glm::vec3 pos = Renderer.cameraMain->transform.position;
        pos += fwd;
        fwd *= 4500; // Total forward force + camera offset
        
        
        // Transform the rigid body
        rp3d::Transform newTransform;
        newTransform.setPosition(rp3d::Vector3(pos.x, pos.y, pos.z));
        
        rp3d::Quaternion quat;
        quat.setAllValues(fwdAngle.x, fwdAngle.y, fwdAngle.z, 0);
        
        newTransform.setOrientation(quat);
        
        body->setTransform(newTransform);
        
        projectile->AddForce(fwd.x, fwd.y, fwd.z);
        projectile->EnableGravity(false);
    }
    
    force *= cameraSpeed;
    
    gameObject->AddForce(force.x, force.y, force.z);
    
    return;
};








