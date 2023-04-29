#include "application/application.h"

extern RandomGen         Random;
extern ColorPreset       Colors;
extern Timer             Time;
extern Timer             PhysicsTime;
extern Logger            Log;

extern ResourceManager   Resources;
extern RenderSystem      Renderer;
extern PhysicsSystem     Physics;
extern InputSystem       Input;

extern ApplicationLayer  Application;


// User scripts
void ScriptCameraController(void);


// Cached resource pointers
Scene*    objectScene;

Mesh*     barrelMesh;
Material* barrelMaterial;





//
// Application entry point
//

void Start() {
    
    Resources.LoadScene("data/main.scene");
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->SetColor( Colors.Make(0.087, 0.087, 0.087) );
    
    // Camera
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Renderer.cameraMain->rigidBody = Physics.CreateRigidBody(-100, 30, 0);
    //Renderer.cameraMain->AddCollider( Resources.FindColliderTag("coll_player"), 0, -10, 0 );
    Renderer.cameraMain->SetMass(2);
    Renderer.cameraMain->SetLinearDamping(4);
    Renderer.cameraMain->SetAngularAxisLockFactor(0, 0, 0);
    Renderer.cameraMain->rigidBody->setIsAllowedToSleep(false);
    Renderer.cameraMain->rigidBody->enableGravity(false);
    
    
    
    // Assign the camera controller native script
    Renderer.cameraMain->script = Renderer.CreateScript();
    Renderer.cameraMain->script->OnUpdate = ScriptCameraController;
    
    
    // Create objects from resource tags
    barrelMesh = Resources.CreateMeshFromTag("barrel");
    
    barrelMesh->ChangeSubMeshColor(0, Colors.ltgray);
    
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    
    
    // Create a plain
    
    Scene* plainScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(plainScene);
    
    Entity* plain = Renderer.CreateEntity();
    plainScene->AddToSceneRoot(plain);
    
    plain->mesh = Renderer.CreateMesh();
    plain->mesh->AddPlainSubDivided(-50, 0, -50,  10, 10,  Colors.gray,  10, 10);
    
    plain->material = Resources.CreateMaterialFromTag("mat_plain");
    plain->rigidBody = Physics.CreateRigidBody(0, 0, 0);
    plain->SetRigidBodyStatic();
    
    plain->AddCollider( Resources.FindColliderTag("coll_plain"), 0, -10.0, 0);
    
    
    
    // Object container scene
    objectScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(objectScene);
    
    return;
}






float spreadMul     = 0.7;
unsigned int focus  = 50;
float count         = 200;

int counter=0;


//
// Application main loop
//

void Run() {
    
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
                Renderer.cameraMain->EnableMouseLook();
                Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
            }
            
            Time.Update();
            PhysicsTime.Update();
        }
    }
    
    // Enter key trigger spawn reset
    if (Input.CheckKeyCurrent(VK_RETURN)) counter = 0;
    
    
    //
    // Spawn a bunch of random objects
    
    if (counter > count) return;
    if (Random.Range(1, 2) == 1) {
        
        counter++;
        
        float xx = (Random.Range(0, focus) - Random.Range(0, focus)) * spreadMul;
        float yy = 100;
        float zz = (Random.Range(0, focus) - Random.Range(0, focus)) * spreadMul;
        
        // Create a barrel object
        Entity* barrel = Renderer.CreateEntity();
        objectScene->AddToSceneRoot(barrel);
        
        barrel->mesh = barrelMesh;
        barrel->material = barrelMaterial;
        
        barrel->rigidBody = Physics.CreateRigidBody(xx, yy, zz);
        
        ColliderTag* colliderTag = Resources.FindColliderTag("coll_barrel");
        barrel->AddCollider(colliderTag, 0, 0, 0);
        
    }
    
    return;
}




void Shutdown(void) {
    
    return;
}







void ScriptCameraController(void) {
    
    float cameraSpeed = 700;
    
    glm::vec3 force(0);
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    force *= cameraSpeed;
    Renderer.cameraMain->AddForce(force.x, force.y, force.z);
    
    return;
};







