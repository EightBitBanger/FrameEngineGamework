## Features
- OpenGL render system  (in progress..)
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ⁯⁮⁭Audio (Considering openAL)
- Resource management
- Random number generation
- Window input handling


<br><br/>
###  Building the project
Exteral build linkage
```
glew32s  glu32  gdi32  user32  kernel32  comctl32  opengl32  reactphysics3d
```


## Singleton access points
The framework contains many sub systems which can be accessed though the following singletons.

```c++
#include "application/application.h"

extern RandomGen         Random;
extern ColorPreset       Colors;
extern Timer             Time;
extern Logger            Log;

extern ResourceManager   Resources;
extern RenderSystem      Renderer;
extern PhysicsSystem     Physics;
extern InputSystem       Input;

extern ApplicationLayer  Application;
```

## Application entry point
You must provide the framework with the functions `Start()` and `Run()`. These functions will act as an entry point for your application.

The function `Start()` will be called once during application initiation.

```c++
//
// This example will setup a basic scene.

void Start() {
    // Load resources
    Resources.LoadTexture("data/grassy.png", "mat_grassy");
    
    // Camera controller
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    Renderer.cameraMain->rigidBody = Physics.CreateRigidBody(0,  // Position
                                                             10,
                                                             0);
    Renderer.cameraMain->rigidBody->enableGravity(false);
    Renderer.cameraMain->SetLinearDamping(4);
    
    // Create a sub divided plain mesh
    Scene* sceneMain = Renderer.CreateScene();
    Renderer.AddToRenderQueue(sceneMain);
    
    Entity* plain = Renderer.CreateEntity();
    sceneMain->AddToSceneRoot(plain);
    
    plain->mesh = Renderer.CreateMesh();
    plain->material = Resources.CreateMaterialFromTag("mat_grassy");
    
    plain->mesh->AddPlainSubDivided(-100,  0, -150,  // Position
                                      10, 10,        // Scale
                                      Colors.gray,   // Vertex color
                                      20, 20 );      // Number of sub divisions
}
```

## Main loop
The framework will continue to call `Run()` once per frame.

```c++
//
// This example will move the camera with the WSAD directional keys and space / shift for elevation.

void Run() {
    
    float cameraSpeed = 700;
    
    glm::vec3 force(0);
    if (Input.CheckKeyCurrent(VK_W)) {force += Renderer.cameraMain->forward;}
    if (Input.CheckKeyCurrent(VK_S)) {force -= Renderer.cameraMain->forward;}
    
    if (Input.CheckKeyCurrent(VK_A)) {force += Renderer.cameraMain->right;}
    if (Input.CheckKeyCurrent(VK_D)) {force -= Renderer.cameraMain->right;}
    
    if (Input.CheckKeyCurrent(VK_SPACE)) {force += Renderer.cameraMain->up;}
    if (Input.CheckKeyCurrent(VK_SHIFT)) {force -= Renderer.cameraMain->up;}
    
    // Apply camera speed multiplier
    force *= cameraSpeed;
    
    Renderer.cameraMain->AddForce(force.x, force.y, force.z);
}
```

