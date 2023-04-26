// Standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <thread>
#include <map>


#define GLEW_STATIC
#include <gl/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "../std/poolallocator.h"
#include "../std/timer.h"
#include "../std/strings.h"
#include "../std/types.h"
#include "../std/logging.h"
#include "../std/random.h"
#include "../std/fileloader.h"
extern Logger Log;

#include "enumerators.h"

#include "types/bufferlayout.h"
#include "types/transform.h"
#include "types/color.h"
#include "types/viewport.h"

#include "components/shader.h"
#include "components/material.h"
#include "components/mesh.h"

#include "components/script.h"
#include "components/camera.h"
#include "components/entity.h"

#include "components/sky.h"

#include "components/scene.h"




class RenderSystem {
    
    HWND  windowHandle;
    HDC   deviceContext;
    HGLRC renderContext;
    
    std::vector<Scene*>  renderQueue;
    
    Mesh*      currentMesh;
    Material*  currentMaterial;
    Shader*    currentShader;
    
    PoolAllocator<Entity>    entity;
    PoolAllocator<Mesh>      mesh;
    PoolAllocator<Shader>    shader;
    PoolAllocator<Camera>    camera;
    PoolAllocator<Material>  material;
    PoolAllocator<Sky>       sky;
    PoolAllocator<Scene>     scene;
    PoolAllocator<Script>    script;
    
public:
    
    Scene* operator[] (unsigned int const i) {return renderQueue[i];}
    
    Material* defaultMaterial;
    Shader*   defaultShader;
    
    Sky*      skyMain;
    Camera*   cameraMain;
    
    Viewport  viewport;
    
    glm::vec2 displaySize;
    glm::vec2 displayCenter;
    
    
    RenderSystem();
    
    
    Entity* CreateEntity(void);
    bool DestroyEntity(Entity* entityPtr);
    
    Mesh* CreateMesh(void);
    bool DestroyMesh(Mesh* meshPtr);
    
    Shader* CreateShader(void);
    bool DestroyShader(Shader* shaderPtr);
    
    Camera* CreateCamera(void);
    bool DestroyCamera(Camera* cameraPtr);
    
    Material* CreateMaterial(void);
    bool DestroyMaterial(Material* materialPtr);
    
    Sky* CreateSky(void);
    bool DestroySky(Sky* skyPtr);
    
    Scene* CreateScene(void);
    Scene* GetScene(unsigned int index);
    unsigned int GetSceneCount(void);
    bool DestroyScene(Scene* scenePtr);
    
    Script* CreateScript(void);
    bool DestroyScript(Script* scriptPtr);
    
    
    void Initiate(void);
    
    GLenum  SetRenderTarget(HWND wHndl);
    void    ReleaseRenderTarget(void);
    
    void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
    
    void RenderFrame(float deltaTime);
    
    void AddToRenderQueue(Scene* scene);
    bool RemoveFromRenderQueue(Scene* scene);
    unsigned int GetRenderQueueSize(void);
    Scene* GetRenderQueueScene(unsigned int index);
    
    glm::mat4 CalculateModelMatrix(Transform& modelTransform);
    
    std::vector<std::string> GetGLErrorCodes(std::string errorLocationString);
};
