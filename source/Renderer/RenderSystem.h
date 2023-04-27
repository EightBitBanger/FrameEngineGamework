#ifndef OPENGL_RENDERER_SYSTEM
#define OPENGL_RENDERER_SYSTEM

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
    
public:
    
    Scene* operator[] (unsigned int const i) {return renderQueue[i];}
    
    /// Default objects
    Material* defaultMaterial;
    Shader*   defaultShader;
    
    /// Current sky object used for rendering
    Sky*      skyMain;
    /// Current camera object used for rendering
    Camera*   cameraMain;
    
    /// Current rendering view port
    Viewport  viewport;
    /// Current screen size and center
    glm::vec2 displaySize;
    glm::vec2 displayCenter;
    
    
    RenderSystem();
    
    
    /// Create an entity object and return its pointer
    Entity* CreateEntity(void);
    /// Destroy an entity object and return true on success
    bool DestroyEntity(Entity* entityPtr);
    
    /// Create a mesh object and return its pointer
    Mesh* CreateMesh(void);
    /// Destroy a mesh object and return true on success
    bool DestroyMesh(Mesh* meshPtr);
    
    /// Create a shader object and return its pointer
    Shader* CreateShader(void);
    /// Destroy a shader object and return true on success
    bool DestroyShader(Shader* shaderPtr);
    
    /// Create a camera object and return its pointer
    Camera* CreateCamera(void);
    /// Destroy a camera object and return true on success
    bool DestroyCamera(Camera* cameraPtr);
    
    /// Create a material object and return its pointer
    Material* CreateMaterial(void);
    /// Destroy a material object and return true on success
    bool DestroyMaterial(Material* materialPtr);
    
    /// Create a sky object and return its pointer
    Sky* CreateSky(void);
    /// Destroy a sky object and return true on success
    bool DestroySky(Sky* skyPtr);
    
    /// Create a scene object and return its pointer
    Scene* CreateScene(void);
    /// Get a scene object currently in the render queue
    Scene* GetScene(unsigned int index);
    /// Get the number of scenes in the render queue
    unsigned int GetSceneCount(void);
    /// Destroy a scene object and return true on success
    bool DestroyScene(Scene* scenePtr);
    
    /// Create a script object and return its pointer
    Script* CreateScript(void);
    /// Destroy a script object and return true on success
    bool DestroyScript(Script* scriptPtr);
    
    
    /// Start the render system
    void Initiate(void);
    
    /// Set the target render context
    GLenum  SetRenderTarget(HWND wHndl);
    /// Free the target render context
    void    ReleaseRenderTarget(void);
    
    /// Set the render area within the display
    void SetViewport(unsigned int x, unsigned int y, unsigned int w, unsigned int h);
    /// Draw the current frame as it stands
    void RenderFrame(float deltaTime);
    
    /// Add a scene to the render queue for drawing
    void AddToRenderQueue(Scene* scene);
    /// Remove a scene from the render queue
    bool RemoveFromRenderQueue(Scene* scene);
    /// Get the number of scenes in the render queue
    unsigned int GetRenderQueueSize(void);
    /// Return a scene pointer at the index position within the render queue
    Scene* GetRenderQueueScene(unsigned int index);
    
    /// Calculate the model matrix from a given transform
    glm::mat4 CalculateModelMatrix(Transform& modelTransform);
    
    /// Return a list of any and all openGL error codes
    std::vector<std::string> GetGLErrorCodes(std::string errorLocationString);
    
    
private:
    
    // Device render context
    HWND  windowHandle;
    HDC   deviceContext;
    HGLRC renderContext;
    
    // Render queue group
    std::vector<Scene*>  renderQueue;
    
    // Current asset bindings
    Mesh*      currentMesh;
    Material*  currentMaterial;
    Shader*    currentShader;
    
    // Allocators
    PoolAllocator<Entity>    entity;
    PoolAllocator<Mesh>      mesh;
    PoolAllocator<Shader>    shader;
    PoolAllocator<Camera>    camera;
    PoolAllocator<Material>  material;
    PoolAllocator<Sky>       sky;
    PoolAllocator<Scene>     scene;
    PoolAllocator<Script>    script;
    
};


#endif
