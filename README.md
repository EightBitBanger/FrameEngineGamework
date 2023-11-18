## Features
- <a href="https://github.com/RetroBytes32/GameEngineFramework/wiki/Render%E2%80%89System">Render system</a>⁭
- Physics support by <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
- ⁯⁮⁭Audio (Considering openAL)
- <a href="https://github.com/RetroBytes32/GameEngineFramework/wiki/Resource%E2%80%89Management">Resource management</a>⁭
- <a href="https://github.com/RetroBytes32/GameEngineFramework/wiki/Input%E2%80%89System">Input handling</a>⁭
- <a href="https://github.com/RetroBytes32/GameEngineFramework/wiki/Serialization%E2%80%89System">Serialization</a>⁭



<br><br/>
## Screenshots
You can find more screenshots [here](https://github.com/RetroBytes32/GameEngineFramework/wiki/Screenshots).

### Vertex lighting demo
<img src="https://github.com/RetroBytes32/GameEngineFramework/blob/main/demo/vertex_lighting_demo_2.jpg"  width="800" height="600">


<br><br/>
## Platform
While this framework currently only supports windows, I look forward to porting to linux in the future.
- Windows


<br><br/>
##  Building the project
First, clone the repository with git. You can then use cmake to generate the project files of your choice.

```
git clone https://github.com/RetroBytes32/FrameEngineGamework
```


### Dependencies
Building this repository will require the following libraries to be installed.

* <a href="https://github.com/DanielChappuis/reactphysics3d">ReactPhysics3D</a>⁭
* <a href="https://github.com/nothings/stb">STB-Image</a>⁭
* <a href="https://github.com/Bly7/OBJ-Loader">Bly7-OBJ-loader</a>⁭
* <a href="https://github.com/Groovounet/glm">glm</a>⁭
* <a href="https://github.com/nigels-com/glew">glew</a>⁭



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
The source file "Application.cpp" is the application layer entry point and the starting point for your application code.
The framework will provide you with the functions `Start()` and `Run()`. These functions will serve as an entry point for your application.
The `Start()` function will be called once during application initiation while `Run()` will be called once per frame.

