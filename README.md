## Features
- OpenGL render system  (in progress..)
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ⁯⁮⁭Audio (Considering openAL)
- Resource management
- Random number generation
- Window input handling

## Core access points
The engine contains various sub systems which can be accessed though the following "singleton" access pointers.

```c++
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
```
## Application entry point
You must provide the framework with the functions "Start()" and "Run()". These functions will act as an entry point for your application.

Start is used to load and initiate resources and to setup the render scene.
```c++
void Start() {
    // Load resources
    Resources.LoadTexture("data/grassy.png", "mat_grassy");
    
    // Camera controller
    Renderer.cameraMain = Renderer.CreateCamera();
    Renderer.cameraMain->transform.position = glm::vec3(0, 10, 0);
    
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    // Create a plain
    Scene* plainScene = Renderer.CreateScene();
    Renderer.AddToRenderQueue(plainScene);
    
    Entity* plain = Renderer.CreateEntity();
    plainScene->AddToSceneRoot(plain);
    
    plain->mesh = Renderer.CreateMesh();
    plain->mesh->AddPlainSubDivided(-100, 0, -150,  10, 10,  Colors.gray,  20, 20);
    
    plain->material = Resources.CreateMaterialFromTag("mat_grassy");
}
```
### Application main loop
The run function serves to update the scene and any objects requiring updating.
```
void Run() {
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
}
```

### Exteral linkage
```
glew32s  glu32  gdi32  user32  kernel32  comctl32  opengl32  reactphysics3d
```

