#include "Engine/Engine.h"

extern RandomGen            Random;
extern ColorPreset          Colors;
extern Timer                Time;
extern Timer                PhysicsTime;
extern Logger               Log;

extern EngineSystemManager  Engine;
extern ApplicationLayer     Application;
extern ResourceManager      Resources;
extern ScriptSystem         Scripting;
extern RenderSystem         Renderer;
extern PhysicsSystem        Physics;
extern InputSystem          Input;







// Global resource pointers
Mesh*       projectileMesh;
Material*   barrelMaterial;
GameObject* cameraController;
rp3d::BoxShape* projectileCollider;





//
// Application entry point
//

void Framework::Start() {
    
    // Load some external files
    Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    Resources.LoadTexture("data/barrel/barrel.png", "mat_barrel");
    Resources.LoadTexture("data/grassy.png", "mat_grassy");
    
    // Set the gravity vector for the simulation
    Physics.SetWorldGravity(0, -9.81, 0);
    
    // Create objects from resource tags
    Material* groundMaterial = Resources.CreateMaterialFromTag("mat_grassy");
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    Mesh* barrelMesh = Resources.CreateMeshFromTag("barrel");
    barrelMesh->ChangeSubMeshColor(0, Colors.white);
    
    projectileMesh = Resources.CreateMeshFromTag("barrel");
    projectileMesh->ChangeSubMeshColor(0, Colors.white);
    
    
    
    // Create a ground plain
    Mesh* groundMesh = Renderer.CreateMesh();
    groundMesh->AddPlainSubDivided(-50, 0, -50, 10, 10, Colors.white, 10, 10);
    
    GameObject* ground = Engine.CreateGameObject();
    ground->name = "world";
    
    
    // Add a light component
    Component* lightComponent = Engine.CreateComponent(ComponentType::Light);
    ground->AddComponent(lightComponent);
    
    
    // Add a render component
    Component* groundRenderer = Engine.CreateComponentEntityRenderer(groundMesh, groundMaterial);
    ground->AddComponent(groundRenderer);
    
    // Add a physics component
    Component* groundRigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
    ground->AddComponent(groundRigidBodyComponent);
    ground->SetStatic();
    
    // Lock the ground plain in place
    ground->SetLinearAxisLockFactor(0, 0, 0);
    ground->SetAngularAxisLockFactor(0, 0, 0);
    
    // Ground collider
    rp3d::BoxShape* groundCollider = Physics.CreateColliderBox(1000, 10, 1000);
    ground->AddColliderBox(groundCollider, 0, -10, 0);
    
    
    
    // Create a camera controller
    cameraController = Engine.CreateCameraController(-50, 20, 0);
    cameraController->SetLinearDamping(2);
    cameraController->SetMass(100);
    
    cameraController->CalculatePhysics();
    cameraController->EnableGravity(true);
    
    
    
    rp3d::BoxShape* boxShape = Physics.CreateColliderBox(1, 8, 1);
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    cameraController->CalculatePhysics();
    
    
    // Create a projectile collider
    projectileCollider = Physics.CreateColliderBox(2, 4, 2);
    
    return;
}







//
// Application main loop
//

void Framework::Run() {
    
    glm::vec3 force(0);
    
    // Keyboard movement, WASD keys
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    // Space and shift for elevation
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    // Camera speed multiplier
    force *= 5000;
    
    cameraController->AddForce(force.x, force.y, force.z);
    
    
    
    // Shoot object from camera
    
    //if (Input.CheckMouseLeftPressed()) {
    
    if (Random.Range(0, 10) > 8) {
        // Spread offset effect on projectile angle
        float spreadMul = 0.1;
        
        //for (int i=0; i < 300; i++) {
            
            // Apply some random physical forces
            float offsetx = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
            float offsety = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
            float offsetz = (Random.Range(0, 100) - Random.Range(0, 100)) * spreadMul;
            
            GameObject* projectile = Engine.CreateGameObject();
            projectile->name = "projectile";
            
            // Add a render component
            Component* entityRenderer = Engine.CreateComponent(ComponentType::Renderer);
            projectile->AddComponent(entityRenderer);
            
            Entity* entity = (Entity*)entityRenderer->GetComponent();
            entity->AttachMesh(projectileMesh);
            entity->AttachMaterial(barrelMaterial);
            
            projectile->SetPosition(offsetx, offsety, offsetz);
            
            // Add a physics component
            Component* rigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
            projectile->AddComponent(rigidBodyComponent);
            rp3d::RigidBody* body = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
            
            // Projectile collider
            projectile->AddColliderBox(projectileCollider, 0, 0, 0);
            
            projectile->CalculatePhysics();
            
            projectile->SetMass(200);
            
            //
            // Calculate projectile force from camera forward angle
            
            glm::vec3 fwd = Renderer.cameraMain->forward;
            glm::vec3 fwdAngle = Renderer.cameraMain->forward;
            
            // Offset starting distance from camera
            fwd *= 8;
            
            //glm::vec3 pos = Renderer.cameraMain->transform.position;
            glm::vec3 pos(0);
            //pos += fwd;
            
            // Total forward force + camera offset distance
            //fwd *= 7000;
            
            float startx = pos.x + (offsetx * 0);
            float starty = pos.y + (offsety * 0) + 50;
            float startz = pos.z + (offsetz * 0);
            
            // Transform the rigid body
            rp3d::Transform newTransform;
            newTransform.setPosition(rp3d::Vector3(startx, starty, startz));
            
            rp3d::Quaternion quat;
            quat.setAllValues(fwdAngle.x + offsetx, fwdAngle.y + offsety, fwdAngle.z + offsetz, 0);
            
            newTransform.setOrientation(quat);
            
            body->setTransform(newTransform);
            
            projectile->AddForce(fwd.x, fwd.y, fwd.z);
            
            //continue;
        //}
        
    }
    
    
    
    // Purge extra objects
    unsigned int index=0;
    while (Engine.GetGameObjectCount() > 200) {
        
        GameObject* gameObject = Engine.GetGameObject(index);
        index++;
        
        // Ignore cameras and world objects
        if ((gameObject->name == "world") | (gameObject->name == "camera")) 
            continue;
        
        Engine.DestroyGameObject(gameObject);
        index = 0;
    }
    
    
    
    // Escape key pause
    
    if (Input.CheckKeyPressed(VK_ESCAPE)) {
        // Uncomment to make the escape key close the application
        //Application.isActive = false;
        //return;
        
        Application.Pause();
        
        if (Application.isPaused) {
            
            if (Renderer.cameraMain != nullptr) 
                Renderer.cameraMain->DisableMouseLook();
            
            Input.ClearKeys();
        } else {
            
            if (Renderer.cameraMain != nullptr) {
                // Reset the camera`s mouse reset position
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





