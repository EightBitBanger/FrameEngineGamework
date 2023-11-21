#ifndef ENGINE_CORE
#define ENGINE_CORE

#include "components/gameobject.h"

#include "UI/canvas.h"
#include "UI/text.h"

#include "../application/applicationlayer.h"
#include "../../tests/framework.h"

#include "../Math/Math.h"
#include "../Renderer/components/meshrenderer.h"
#include "../Physics/PhysicsSystem.h"
#include "../Scripting/components/script.h"
#include "../Renderer/components/camera.h"

#include "../AI/ActorSystem.h"
#include "../AI/components/actor.h"
#include "../AI/Genetics.h"

#include "../../vendor/CodeBaseLibrary/directorysearch.h"
#include "../../vendor/CodeBaseLibrary/logging.h"
#include "../../vendor/CodeBaseLibrary/random.h"
#include "../../vendor/CodeBaseLibrary/timer.h"

extern PhysicsSystem  Physics;


class __declspec(dllexport) EngineSystemManager {
    
public:
    
    // Game objects
    
    /// Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    
    /// Destroy a game object.
    bool DestroyGameObject(GameObject* gameObjectPtr);
    
    /// Get the number of game objects.
    unsigned int GetGameObjectCount(void);
    
    /// Return a pointer to a game object at the index position.
    GameObject* GetGameObject(unsigned int index);
    
    
    // Components
    
    /// Create a component object with initial type information and return its pointer.
    Component* CreateComponent(ComponentType type);
    
    /// Destroy a component object.z
    bool DestroyComponent(Component* componentPtr);
    
    /// Get the number of component objects.
    unsigned int GetComponentCount(void);
    
    
    // Special components and game objects
    
    /// Create a camera controller game object and return its pointer.
    GameObject* CreateCameraController(glm::vec3 position, glm::vec3 scale);
    
    /// Generate a sky object and return its pointer.
    GameObject* CreateSky(std::string meshTagName, Color colorLow, Color colorHigh, float biasMul);
    
    /// Create a mesh renderer component and return its pointer.
    Component* CreateComponentMeshRenderer(Mesh* meshPtr, Material* materialPtr);
    
    /// Create a point light component and return its pointer.
    Component* CreateComponentLight(glm::vec3 position);
    
    /// Create an AI actor component and return its pointer.
    GameObject* CreateAIActor(glm::vec3 position);
    
    
    // UI elements
    
    /// Create an empty overlay renderer object and return its pointer.
    GameObject* CreateOverlayRenderer(void);
    
    /// Create a text overlay renderer object and return its pointer.
    GameObject* CreateOverlayTextRenderer(std::string text, unsigned int textSize, Color color, std::string materialTag);
    
    
    /// Add a string of sprite quads to a mesh.
    void AddMeshText(GameObject* overlayObject, float xPos, float yPos, std::string text, Color textColor);
    
    /// Add a quad to a mesh mapping to a sub sprite from a sprite sheet texture.
    void AddMeshSubSprite(GameObject* overlayObject, float xPos, float yPos, int index, Color meshColor);
    
    
    EngineSystemManager();
    
    /// Initiate the engine.
    void Initiate(void);
    
    /// Call update on engine components.
    void Update(void);
    
    /// Shutdown the engine.
    void Shutdown(void);
    
    /// Create an object of the type specified.
    template <typename T> T* Create(void) {
        // Engine
        if (std::is_same<T, GameObject>::value)    return (T*)CreateGameObject();
        if (std::is_same<T, Component>::value)     return (T*)CreateComponent( COMPONENT_TYPE_UNDEFINED );
        // Render system
        if (std::is_same<T, Mesh>::value)          return (T*)Renderer.CreateMesh();
        if (std::is_same<T, Material>::value)      return (T*)Renderer.CreateMaterial();
        if (std::is_same<T, Shader>::value)        return (T*)Renderer.CreateShader();
        if (std::is_same<T, Scene>::value)         return (T*)Renderer.CreateScene();
        if (std::is_same<T, Camera>::value)        return (T*)Renderer.CreateCamera();
        if (std::is_same<T, MeshRenderer>::value)  return (T*)Renderer.CreateMeshRenderer();
        
        return nullptr;
    }
    
    /// Destroy an object of the type specified.
    template <typename T> bool Destroy(T* objectPtr) {
        // Engine
        if (std::is_same<T, GameObject>::value)    return DestroyGameObject( (GameObject*)objectPtr );
        if (std::is_same<T, Component>::value)     return DestroyComponent( (Component*)objectPtr );
        // Render system
        if (std::is_same<T, Mesh>::value)          return Renderer.DestroyMesh( (Mesh*)objectPtr );
        if (std::is_same<T, Material>::value)      return Renderer.DestroyMaterial( (Material*)objectPtr );
        if (std::is_same<T, Shader>::value)        return Renderer.DestroyShader( (Shader*)objectPtr );
        if (std::is_same<T, Scene>::value)         return Renderer.DestroyScene( (Scene*)objectPtr );
        if (std::is_same<T, Camera>::value)        return Renderer.DestroyCamera( (Camera*)objectPtr );
        if (std::is_same<T, MeshRenderer>::value)  return Renderer.DestroyMeshRenderer( (MeshRenderer*)objectPtr );
        
        return false;
    }
    
    /// Create a component object containing the type specified.
    template <typename T> Component* CreateComponent(void) {
        if (std::is_same<T, Camera>::value)    return CreateComponent(Components.Camera);
        if (std::is_same<T, Light>::value)     return CreateComponent(Components.Light);
        if (std::is_same<T, Script>::value)    return CreateComponent(Components.Script);
        if (std::is_same<T, RigidBody>::value) return CreateComponent(Components.RigidBody);
        if (std::is_same<T, Actor>::value)     return CreateComponent(Components.Actor);
        if (std::is_same<T, Text>::value)      return CreateComponent(Components.Text);
        return nullptr;
    }
    
    /// Create a mesh renderer component object.
    template <typename T> Component* CreateComponent(Mesh* meshPtr, Material* materialPtr) {
        if (!std::is_same<T, MeshRenderer>::value) 
            return nullptr;
        Component* rendererComponent = CreateComponent(Components.MeshRenderer);
        MeshRenderer* entityRenderer = (MeshRenderer*)rendererComponent->GetComponent();
        entityRenderer->mesh = meshPtr;
        entityRenderer->material = materialPtr;
        return rendererComponent;
    }
    
    
private:
    
    // List of active game objects
    std::vector<GameObject*> mGameObjectActive;
    
    // Engine allocators
    PoolAllocator<GameObject> mGameObjects;
    PoolAllocator<Component>  mComponents;
    PoolAllocator<Text>       mTextObjects;
    
    // Default shaders
    struct DefaultShaders {
        Shader*  texture;
        Shader*  textureUnlit;
        Shader*  color;
        Shader*  UI;
    };
    
    
public:
    
    DefaultShaders shaders;
    
};



#endif
