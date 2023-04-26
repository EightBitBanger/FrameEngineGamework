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
    Renderer.cameraMain->rigidBody = Physics.CreateRigidBody(-130, 50, 0);
    Renderer.cameraMain->AddCollider( Resources.FindColliderTag("coll_player"), 0, -10, 0 );
    Renderer.cameraMain->SetMass(10);
    Renderer.cameraMain->SetLinearDamping(0.4);
    Renderer.cameraMain->SetAngularAxisLockFactor(0, 0, 0);
    Renderer.cameraMain->rigidBody->setIsAllowedToSleep(false);
    Renderer.cameraMain->rigidBody->enableGravity(false);
    
    
    //Renderer.cameraMain->transform.position = glm::vec3(-120, 20, 0);
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Renderer.cameraMain->script = Renderer.CreateScript();
    Renderer.cameraMain->script->OnUpdate = CameraMovementScript;
    
    
    barrelMesh = Resources.CreateMeshFromTag("barrel");
    
    barrelMesh->ChangeSubMeshColor(0, Colors.ltgray);
    
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    
    // Plain scene
    Scene* plainScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(plainScene);
    
    
    // Create a plain
    Entity* plain = Renderer.CreateEntity();
    plainScene->AddToSceneRoot(plain);
    
    plain->mesh = Renderer.CreateMesh();
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
    
    if (Input.CheckKeyCurrent(VK_RETURN)) counter = 0;
    
    if (counter > 100) return;
    
    
    if (Random.Range(1, 2) == 1) {
        
        counter++;
        
        float xx = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        float yy = 100;
        float zz = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
        
        // Create a barrel object
        Entity* barrel = Renderer.CreateEntity();
        objectsScene->AddToSceneRoot(barrel);
        
        barrel->mesh = barrelMesh;
        barrel->material = barrelMaterial;
        
        barrel->rigidBody = Physics.CreateRigidBody(xx, yy, zz);
        //barrel->SetRigidBodyDynamic();
        
        barrel->AddCollider( Resources.FindColliderTag("coll_barrel"), 0, 0, 0);
        
    }
    
    return;
}




void Shutdown(void) {
    
    return;
}







void CameraMovementScript(void) {
    
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







