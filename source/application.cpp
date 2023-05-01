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
void ScriptCameraController(void);


// Cached resource pointers
Scene*    objectScene;

Mesh*     barrelMesh;
Material* barrelMaterial;



Scene* plainScene;


//
// Application entry point
//

void Framework::Start() {
    
    Resources.LoadScene("data/main.scene");
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->SetColor( Colors.Make(0.087, 0.087, 0.087) );
    
    // Camera
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->transform.position = glm::vec3(-50, 50, 0);
    
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    
    
    
    // Create objects from resource tags
    barrelMesh = Resources.CreateMeshFromTag("barrel");
    
    barrelMesh->ChangeSubMeshColor(0, Colors.ltgray);
    
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    
    
    
    
    plainScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(plainScene);
    
    
    
    // Create a plain
    /*
    
    Scene* plainScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(plainScene);
    
    Entity* plain = Renderer.CreateEntity();
    plainScene->AddToSceneRoot(plain);
    
    Mesh* meshPlain = Renderer.CreateMesh();
    plain->AttachMesh(meshPlain);
    meshPlain->AddPlain(0, 0, 0,  100, 100,  Colors.gray);
    
    Material* materialPtr = Resources.CreateMaterialFromTag("mat_plain");
    plain->AttachMaterial(materialPtr);
    */
    
    //rp3d::RigidBody* rigidBodyPtr = Physics.CreateRigidBody(0, 0, 0);
    //plain->AttachRidigBody(rigidBodyPtr);
    //plain->SetRigidBodyStatic();
    
    //plain->AddCollider( Resources.FindColliderTag("coll_plain"), 0, -10.0, 0);
    
    
    
    //rp3d::Vector3 lockfactor(0, 0, 0);
    //rigidBody->setLinearLockAxisFactor(lockfactor);
    //rigidBody->setAngularLockAxisFactor(lockfactor);
    //rigidBody->setType(rp3d::BodyType::STATIC);
    
    
    
    
    
    
    
    return;
}






float spreadMul     = 0.7;
unsigned int focus  = 50;
float count         = 200;

int counter=0;




//
// Application main loop
//

void Framework::Run() {
    
    GameObject* gameObject = Engine.CreateGameObject();
    
    rp3d::RigidBody* rigidBody = Engine.CreateRigidBody();
    gameObject->AttachRidigBody(rigidBody);
    
    Entity* entityRenderer = Engine.CreateEntityRenderer(barrelMesh, barrelMaterial);
    gameObject->AttachEntity(entityRenderer);
    
    
    
    unsigned int size = Engine.GetGameObjectCount();
    
    if (size > 100) {
        
        GameObject* oldObject = Engine.GetGameObject(0);
        
        rp3d::RigidBody* body = oldObject->GetAttachedRidigBody();
        
        Engine.DestroyRigidBody(body);
        Engine.DestroyGameObject(oldObject);
        
    }
    
    
    
    
    
    /*
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
        
        
        barrel->AttachMesh(barrelMesh);
        barrel->AttachMaterial(barrelMaterial);
        
        barrel->AttachRidigBody( Physics.CreateRigidBody(xx, yy, zz) );
        
        ColliderTag* colliderTag = Resources.FindColliderTag("coll_barrel");
        barrel->AddCollider(colliderTag, 0, 0, 0);
        
    }
    */
    return;
}




void Framework::Shutdown(void) {
    
    return;
}







void ScriptCameraController(void) {
    
    
    float cameraSpeed = 1000;
    
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







