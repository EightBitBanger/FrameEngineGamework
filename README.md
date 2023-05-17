## Introduction

This game engine framework aims to provide simpler interfaces with more complex systems. The application layer contains an entry point function and numerious APIs for rendering, physics, resource management, scene management, input, timing and random number generation.


<br><br/>
### Core Features
- OpenGL render system  (in progress..)
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ⁯⁮⁭Audio (Considering openAL)
- Resource management
- Window input handling

<br><br/>
## Platform
While this framework currently only supports windows, I look forward to porting to linux in the future.
- Windows


<br><br/>
##  Building the project
First, download the repository with git. You can then use cmake to generate the project files of your choice.

```
git pull https://github.com/RetroBytes32/GameEngineFramework
```


## Dependencies
Building this repository will require the following libraries to be installed.

* <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
* <a href="https://github.com/nothings/stb">STB-Image</a>⁭
* <a href="https://github.com/Bly7/OBJ-Loader">Bly7-OBJ-loader</a>⁭
* <a href="https://github.com/Groovounet/glm">glm</a>⁭
* <a href="https://github.com/nigels-com/glew">glew</a>⁭

### Required exteral linkage.
```
             Windows                    OpenGL           Physics
 _______________________________    _______________    ____________
gdi32  user32  kernel32  comctl32  glew32s  opengl32  reactphysics3d
```


<br><br/>
## Documentation
You can find the API documentation <a href="https://github.com/RetroBytes32/GameEngine-Framework/wiki">here</a>⁭.


<br><br/>
## Singleton access pointers
The framework contains many sub systems which can be accessed though the following singletons.

```c++
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
```


<br><br/>
## Application entry point
> The source file "Application.cpp" is the application layer entry point and the starting point for your application code.
> The framework will provide you with the functions `Start()` and `Run()`. These functions will serve as an entry point for your application.
> The `Start()` function will be called once during application initiation.

```c++
// This example will load required resources and initiate the scene

// Global resources that run() must know about
Mesh*       projectileMesh;
Material*   barrelMaterial;
GameObject* cameraController;
rp3d::BoxShape* projectileCollider;

void Framework::Start() {
    
    // Load some external files
    Resources.LoadWaveFront("data/barrel/barrel.obj", "barrel");
    Resources.LoadTexture("data/barrel/barrel.png", "mat_barrel");
    Resources.LoadTexture("data/grassy.png", "mat_grassy");
    
    // Set the gravity vector for the simulation
    Physics.SetWorldGravity(0, -90, 0);
    
    // Create objects from resource tags
    Material* groundMaterial = Resources.CreateMaterialFromTag("mat_grassy");
    barrelMaterial = Resources.CreateMaterialFromTag("mat_barrel");
    
    Mesh* barrelMesh = Resources.CreateMeshFromTag("barrel");
    barrelMesh->ChangeSubMeshColor(0, Colors.white);
    
    projectileMesh = Resources.CreateMeshFromTag("barrel");
    projectileMesh->ChangeSubMeshColor(0, Colors.white);
    
    
    
    // Create a ground plain mesh
    Mesh* groundMesh = Renderer.CreateMesh();
    groundMesh->AddPlainSubDivided(-50, 0, -50, 10, 10, Colors.white, 10, 10);
    
    // Create a game object named world
    GameObject* ground = Engine.CreateGameObject();
    ground->name = "world";
    
    // Add a render component
    Component* groundRenderer = Engine.CreateComponentEntityRenderer(groundMesh, groundMaterial);
    ground->AddComponent(groundRenderer);
    
    // Add a physics component
    Component* groundRigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
    ground->AddComponent(groundRigidBodyComponent);
    
    // Lock the ground plain in place
    ground->SetRigidBodyStatic();
    ground->SetLinearAxisLockFactor(0, 0, 0);
    ground->SetAngularAxisLockFactor(0, 0, 0);
    
    // Ground collider
    rp3d::BoxShape* groundCollider = Physics.CreateColliderBox(100, 100, 100);
    ground->AddColliderBox(groundCollider, 0, -100, 0);
    
    
    // Create a game object with a camera and a rigid body component
    cameraController = Engine.CreateCameraController(0, 30, 0);
    
    // Create a collider for the projectile objects
    projectileCollider = Physics.CreateColliderBox(1.45, 2.1, 1.45);
    
}
```

<br><br/>
### Main loop
> The `Run()` function will be called once per frame.

```c++
// This example will apply force to the camera`s rigid body on key presses
// and allow you to throw objects along the direction angle of the camera.

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
    force *= 700;
    
    cameraController->AddForce(force.x, force.y, force.z);
    
    
    
    // Shoot objects from camera
    
    if (Input.CheckMouseLeftPressed()) {
        
        // Spread offset effect on projectile angle
        float spreadMul = 0.0007;
        
        for (int i=0; i < 3; i++) {
            
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
            
            // Add a physics component
            Component* rigidBodyComponent = Engine.CreateComponent(ComponentType::RigidBody);
            projectile->AddComponent(rigidBodyComponent);
            rp3d::RigidBody* body = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
            
            // Projectile collider
            projectile->AddColliderBox(projectileCollider, 0, 0, 0);
            
            projectile->CalculatePhysics();
            
            projectile->SetLinearAxisLockFactor(1, 1, 1);
            projectile->SetAngularAxisLockFactor(1, 1, 1);
            projectile->SetMass(1);
            
            //
            // Calculate projectile force from camera forward angle
            
            glm::vec3 fwd = Renderer.cameraMain->forward;
            glm::vec3 fwdAngle = Renderer.cameraMain->forward;
            
            // Offset starting distance from camera
            fwd *= 8;
            
            glm::vec3 pos = Renderer.cameraMain->transform.position;
            pos += fwd;
            
            // Total forward force + camera offset distance
            fwd *= 7000;
            
            float startx = pos.x + (offsetx * 0);
            float starty = pos.y + (offsety * 0) - 7;
            float startz = pos.z + (offsetz * 0);
            
            // Transform the rigid body
            rp3d::Transform newTransform;
            newTransform.setPosition(rp3d::Vector3(startx, starty, startz));
            
            rp3d::Quaternion quat;
            quat.setAllValues(fwdAngle.x + offsetx, 
                              fwdAngle.y + offsety, 
                              fwdAngle.z + offsetz, 0);
            
            newTransform.setOrientation(quat);
            
            body->setTransform(newTransform);
            
            projectile->AddForce(fwd.x, fwd.y, fwd.z);
            
            continue;
        }
        
    }
    
    
    
    // Purge extra objects
    
    unsigned int index=0;
    while (Engine.GetGameObjectCount() > 700) {
        
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
        
        Application.Pause();
        
        if (Application.isPaused) {
            
            if (Renderer.cameraMain != nullptr) 
                Renderer.cameraMain->DisableMouseLook();
            
            Input.ClearKeys();
        } else {
            
            if (Renderer.cameraMain != nullptr) {
                // Reset the camera`s mouse reset position
                Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x,
                                                    Renderer.displayCenter.y);
                Renderer.cameraMain->EnableMouseLook();
            }
            
            Time.Update();
            PhysicsTime.Update();
        }
    }
    
}
```
