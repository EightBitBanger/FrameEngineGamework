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






Mesh*     subPart;
Material* material;

rp3d::BoxShape* collider;



void Start(void) {
    
    
    // Start up the physics system
    Physics.Initiate();
    Physics.SetWorldGravity(0, -29, 0);
    
    Physics.world->setIsDebugRenderingEnabled(true);
    
    
    // Load some initial resources
    Resources.Initiate();
    
    Resources.LoadScene("data/main.scene");
    
    //Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    //Resources.LoadTexture("data/barrel/barrel.png", "mat_barrel");
    //Resources.LoadShaderGLSL("data/default.shader", "default");
    
    
    collider = Resources.GetColliderFromTag("coll_barrel");
    assert(collider != nullptr);
    
    // Start the renderer and setup a scene
    Renderer.Initiate();
    
    
    // Main scene
    Scene* currentScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(currentScene);
    
    // Sky background
    Renderer.skyMain = Renderer.CreateSky();
    Renderer.skyMain->background = Colors.Make(0.087, 0.087, 0.087);
    
    // Camera
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->transform.position = glm::vec3(-50, 5, 0);
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Renderer.cameraMain->script = Renderer.CreateScript();
    Renderer.cameraMain->script->OnUpdate = CameraMovementScript;
    
    Entity* entity = Renderer.CreateEntity();
    Mesh* mesh = Renderer.CreateMesh();
    entity->mesh = mesh;
    entity->mesh->SetDefaultAttributes();
    entity->mesh->shader = Renderer.defaultShader;
    
    //defaultShader = Resources.CreateShaderFromTag("default");
    entity->mesh->shader = Renderer.defaultShader;
    if (entity->mesh->shader == nullptr) return;
    
    material = Resources.CreateMaterialFromTag("mat_barrel");
    entity->material = material;
    if (entity->material == nullptr) return;
    
    entity->material->color = Color(0, 0, 0, 1);
    
    subPart = Resources.CreateMeshFromTag("barrel");
    subPart->SetDefaultAttributes();
    
    
    
    currentScene->AddToSceneRoot(entity);
    
    
    // Plain object
    Entity* plain = Renderer.CreateEntity();
    plain->mesh = Renderer.CreateMesh();
    plain->mesh->SetDefaultAttributes();
    
    //plain->mesh->AddPlain(  0, 0, 10, 10, 10, Colors.white);
    //plain->mesh->AddPlain(-10, 0, 10, 10, 10, Colors.white);
    
    plain->mesh->AddPlainSubDivided(0, 0, 0, 10, 10, Colors.blue, 8, 8);
    
    plain->mesh->shader = Renderer.defaultShader;
    plain->material = Resources.CreateMaterialFromTag("mat_plain");
    plain->material->color = Colors.black;
    
    
    rp3d::BoxShape* collPlain = Resources.GetColliderFromTag("coll_plain");
    
    plain->rigidBody = Physics.CreateRigidBody();
    plain->AddCollider(collPlain, 0, -10, 0);
    
    
    
    plain->SetLinearAxisLockFactor(0, 0, 0);
    plain->SetAngularAxisLockFactor(0, 0, 0);
    
    Scene* newScene = Renderer.CreateScene();
    
    newScene->AddToSceneRoot(plain);
    Renderer.AddToRenderQueue(newScene);
    
    return;
}





float spawnRadiusMul  = 0.3;
float forceMul        = 0.01;
float torqueMul       = 0.01;

bool final = false;

Mesh* debugMesh;


void Run(void) {
    
    
    debugMesh = Renderer.CreateMesh();
    debugMesh->SetDefaultAttributes();
    debugMesh->SetPrimitive(GL_LINES);
    
    
    
    rp3d::DebugRenderer& debugRenderer = Physics.world->getDebugRenderer();
    
    //debugRenderer->DebugLine();
    
    debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::CONTACT_POINT, true);
    
    
    unsigned int lineCount = debugRenderer.getNbLines();
    
    const rp3d::DebugRenderer::DebugLine* pointsList = debugRenderer.getLinesArray();
    
    SubMesh subDebug;
    
    /*
    for (unsigned int) {
        
        subDebug.vertexBuffer
        subDebug.vertexBegin
        subDebug.vertexCount
        
        
    }
    */
    
    debugMesh->AddSubMesh(0, 0, 0, subDebug);
    
    
    
    
    
    return;
    
    
    
    
    if (final) return;
    
    Scene* scene = Renderer.GetScene(0);
    if (scene == nullptr) return;
    
    Entity* entity = scene->GetEntity(0);
    if (entity == nullptr) return;
    
    for (int i=0; i < 30; i++) {
        float xx = (Random.Range(1, 100) - Random.Range(1, 100)) * spawnRadiusMul;
        float yy = (Random.Range(1, 100) - Random.Range(1, 100)) * spawnRadiusMul;
        float zz = (Random.Range(1, 100) - Random.Range(1, 100)) * spawnRadiusMul;
        
        float fx = (Random.Range(1, 100) - Random.Range(1, 100)) * forceMul;
        float fy = (Random.Range(1, 100) - Random.Range(1, 100)) * forceMul;
        float fz = (Random.Range(1, 100) - Random.Range(1, 100)) * forceMul;
        
        float tx = (Random.Range(1, 100) - Random.Range(1, 100)) * torqueMul;
        float ty = (Random.Range(1, 100) - Random.Range(1, 100)) * torqueMul;
        float tz = (Random.Range(1, 100) - Random.Range(1, 100)) * torqueMul;
        
        
        Entity* newEntity       = Renderer.CreateEntity();
        newEntity->mesh         = subPart;
        newEntity->material     = material;
        newEntity->mesh->shader = Renderer.defaultShader;
        newEntity->transform.scale = glm::vec3(0.5,0.5,0.5);
        
        newEntity->rigidBody = Physics.CreateRigidBody(xx, yy + 80, zz);
        newEntity->rigidBody->setIsAllowedToSleep(true);
        newEntity->AddCollider(collider, 0, 0, 0);
        
        // Sus
        
        newEntity->rigidBody->setMass(50);
        newEntity->rigidBody->setAngularDamping(0.5);
        newEntity->rigidBody->setLinearDamping(0.7);
        
        
        //newEntity->CalculatePhysics();
        newEntity->rigidBody->updateMassFromColliders();
        //newEntity->rigidBody->updateLocalCenterOfMassFromColliders();
        //newEntity->rigidBody->updateLocalInertiaTensorFromColliders();
        
        
        
        
        
        
        //newEntity->AddForce(fx, fy, fz);
        //newEntity->AddTorque(tx, ty, tz);
        
        scene->AddToSceneRoot(newEntity);
    }
    
    if (scene->GetRenderQueueSize() > 100) {
        
        final = true;
        return;
        
        for (int i=0; i < 5; i++) {
            Entity* oldEntity = scene->entities[0];
            scene->RemoveFromSceneRoot(oldEntity);
            
            Renderer.DestroyEntity(oldEntity);
            
        }
        
    }
    
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







