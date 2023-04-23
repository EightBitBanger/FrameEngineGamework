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









void Start(void) {
    
    Physics.Initiate();
    
    Renderer.Initiate();
    
    Resources.Initiate();
    
    Engine.Initiate();
    
    
    
    //
    // Start up the physics system and setup the simulation
    
    Physics.SetWorldGravity(0, -9.98, 0);
    
    //
    // Load some initial resources
    
    Resources.LoadScene("data/main.scene");
    
    //Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    //Resources.LoadTexture("data/barrel/barrel.png", "mat_barrel");
    //Resources.LoadTexture("data/grassy.png", "mat_plain");
    //Resources.LoadShaderGLSL("data/default.shader", "default");
    
    
    //rp3d::BoxShape* collider = Resources.GetColliderFromTag("coll_barrel");
    //assert(collider != nullptr);
    
    
    
    
    
    //
    // Start the renderer and setup a scene
    
    
    // Main scene
    Scene* currentScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(currentScene);
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->background = Colors.Make(0.087, 0.087, 0.087);
    
    // Camera
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->transform.position = glm::vec3(-50, 10, 0);
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Renderer.cameraMain->script = Renderer.CreateScript();
    Renderer.cameraMain->script->OnUpdate = CameraMovementScript;
    
    
    
    
    // Plain object
    Entity* plain = Renderer.CreateEntity();
    plain->mesh = Renderer.CreateMesh();
    plain->mesh->SetDefaultAttributes();
    
    
    plain->mesh->AddPlainSubDivided(-5, 0, -5, 10, 10, Colors.gray, 3, 3);
    
    //plain->mesh->AddPlain(0, 0, 0, 100, 100, Colors.gray);
    
    //plain->mesh->AddWallSubDivided(-5, -5, 0, 10, 10, Colors.white, 3, 3);
    
    plain->mesh->shader = Renderer.defaultShader;
    plain->material = Resources.CreateMaterialFromTag("mat_plain");
    plain->material->color = Colors.white;
    
    rp3d::BoxShape* collPlain = Physics.CreateColliderBox(100, 100, 100);
    
    plain->rigidBody = Physics.CreateRigidBody();
    plain->AddCollider(collPlain, 0, -100, 0);
    
    
    plain->SetMass(0);
    
    plain->SetLinearAxisLockFactor(0, 0, 0);
    plain->SetAngularAxisLockFactor(0, 0, 0);
    
    
    currentScene->AddToSceneRoot(plain);
    
    
    
    
    return;
}





void Run(void) {
    
    if (!Input.CheckKeyPressed(VK_RETURN)) 
        return;
    
    Entity* barrel1 = Engine.CreateGameObject(0, 50, 0, "barrel", "mat_barrel", "coll_barrel");
    
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







