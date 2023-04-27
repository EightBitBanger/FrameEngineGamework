### Features
- OpenGL render system  (in progress..)
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ⁯⁮⁭Audio (Considering openAL)
- Resource management
- Random number generation
- Window input handling
<a>
⁪
</a>

### Application entry point 
The engine contains various sub systems which can be accessed though the following "singleton" access points.

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
You must provide the framework with the functions Start() and Run(). These functions will act as an entry point for your application.

```c++
Start() {
  
}

Run() {
  
}
```

### Exteral linkage
```
glew32s  glu32  gdi32  user32  kernel32  comctl32  opengl32  reactphysics3d
```

