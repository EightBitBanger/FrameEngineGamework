#ifndef ENGINE_CORE
#define ENGINE_CORE

#include <GameEngineFramework/Engine/components/gameobject.h>
#include <GameEngineFramework/Engine/components/component.h>

#include <GameEngineFramework/Engine/UI/canvas.h>
#include <GameEngineFramework/Engine/UI/panel.h>
#include <GameEngineFramework/Engine/UI/sprite.h>
#include <GameEngineFramework/Engine/UI/text.h>
#include <GameEngineFramework/Engine/UI/button.h>

#include <GameEngineFramework/application/Platform.h>

#include <GameEngineFramework/Resources/ResourceManager.h>
#include <GameEngineFramework/Physics/PhysicsSystem.h>
#include <GameEngineFramework/Profiler/Profiler.h>
#include <GameEngineFramework/Scripting/components/script.h>

#include <GameEngineFramework/Renderer/components/meshrenderer.h>
#include <GameEngineFramework/Renderer/components/camera.h>

#include <GameEngineFramework/ActorAI/ActorSystem.h>
#include <GameEngineFramework/ActorAI/components/actor.h>
#include <GameEngineFramework/ActorAI/Genetics/Gene.h>

#include <GameEngineFramework/Types/types.h>
#include <GameEngineFramework/Logging/Logging.h>
#include <GameEngineFramework/Timer/timer.h>

#include <GameEngineFramework/Math/Math.h>
#include <GameEngineFramework/Math/Random.h>

#include <GameEngineFramework/Networking/NetworkSystem.h>


#ifndef BUILD_CORE
    extern EngineComponents     Components;
    extern ColorPreset          Colors;
    extern NumberGeneration     Random;
    extern Logger               Log;
    extern ProfilerTimer        Profiler;
    extern Timer                PhysicsTime;
    extern Timer                Time;
    
    extern PlatformLayer        Platform;
    extern ResourceManager      Resources;
    extern Serialization        Serializer;
    extern ScriptSystem         Scripting;
    extern RenderSystem         Renderer;
    extern PhysicsSystem        Physics;
    extern NetworkSystem        Network;
    extern AudioSystem          Audio;
    extern InputSystem          Input;
    extern MathCore             Math;
    extern ActorSystem          AI;
    
    extern StringType  String;
    extern FloatType   Float;
    extern DoubleType  Double;
    extern IntType     Int;
    extern UintType    Uint;
#endif

// UI callback type
typedef void(*ButtonCallBack)();


#define COMPONENT_STREAM_BUFFER_SIZE   1024 * 32

struct ComponentDataStreamBuffer {
    
    // Base object
    GameObject*    gameObject;
    
    // Components
    Transform*     transform;
    
    // Rendering
    Light*         light;
    Actor*         actor;
    Camera*        camera;
    RigidBody*     rigidBody;
    MeshRenderer*  meshRenderer;
    
    // UI
    Text*          text;
    Panel*         panel;
    
};



class ENGINE_API EngineSystemManager {
    
public:
    
    /// Main active rendering scene.
    Scene* sceneMain;
    
    // Game objects
    
    /// Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    
    /// Destroy a game object.
    bool DestroyGameObject(GameObject* gameObjectPtr);
    
    /// Get the number of game objects.
    unsigned int GetGameObjectCount(void);
    
    /// Return a pointer to a game object at the index position.
    GameObject* GetGameObject(unsigned int index);
    
    
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
    
    /// Create an empty overlay renderer and return its object.
    GameObject* CreateOverlayRenderer(void);
    
    /// Create a text overlay and return its object.
    GameObject* CreateOverlayTextRenderer(int x, int y, std::string text, unsigned int textSize, Color color, std::string materialTag);
    
    /// Create a panel overlay and return its object.
    GameObject* CreateOverlayPanelRenderer(int x, int y, int width, int height, std::string materialTag);
    
    
    // Mouse button event callback
    
    /// Create a button and return its pointer.
    Button* CreateOverlayButtonCallback(int x, int y, int width, int height, ButtonCallBack callback);
    
    /// Destroy a button 
    bool DestroyOverlayButtonCallback(Button* button);
    
    
    //
    // Lower level UI rendering
    //
    
    /// Add a string of sprite quads to a mesh.
    void AddMeshText(GameObject* overlayObject, float xPos, float yPos, float width, float height, std::string text, Color textColor);
    
    /// Add a quad to a mesh mapping to a sub sprite from a sprite sheet texture.
    void AddMeshSubSprite(GameObject* overlayObject, float xPos, float yPos, float width, float height, int index, Color meshColor);
    
    
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
        
        // Renderer
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
        // Engine
        if (std::is_same<T, Transform>::value)    return CreateComponent(Components.Transform);
        // Renderer
        if (std::is_same<T, MeshRenderer>::value) return CreateComponent(Components.MeshRenderer);
        if (std::is_same<T, Camera>::value)       return CreateComponent(Components.Camera);
        if (std::is_same<T, Light>::value)        return CreateComponent(Components.Light);
        if (std::is_same<T, Script>::value)       return CreateComponent(Components.Script);
        // Physics
        if (std::is_same<T, RigidBody>::value)    return CreateComponent(Components.RigidBody);
        // AI
        if (std::is_same<T, Actor>::value)        return CreateComponent(Components.Actor);
        // UI
        if (std::is_same<T, Text>::value)         return CreateComponent(Components.Text);
        if (std::is_same<T, Panel>::value)        return CreateComponent(Components.Panel);
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
    
    /// Destroy a component object.
    bool DestroyComponent(Component* componentPtr);
    
    /// Get the number of component objects.
    unsigned int GetComponentCount(void);
    
    
private:
    
    // Create a component object with initial type information and return its pointer.
    Component* CreateComponent(ComponentType type);
    
    
    // Batch update engine components
    void UpdateTransformationChains(void);
    void UpdateUI(void);
    
    // Update component by index
    void UpdateMeshRenderer(unsigned int index);
    void UpdateRigidBody(unsigned int index);
    void UpdatePanelUI(unsigned int index);
    void UpdateTextUI(unsigned int index);
    void UpdateCamera(unsigned int index);
    void UpdateLight(unsigned int index);
    
    // AI update functions
    void UpdateActor(unsigned int index);
    void UpdateActorTargetRotation(unsigned int index);
    void UpdateActorGenetics(unsigned int index);
    void UpdateActorAnimation(unsigned int index);
    
    // List of active game objects
    std::vector<GameObject*>  mGameObjectActive;
    
    // Component allocators
    PoolAllocator<GameObject> mGameObjects;
    PoolAllocator<Component>  mComponents;
    
    PoolAllocator<Text>       mTextObjects;
    PoolAllocator<Panel>      mPanelObjects;
    PoolAllocator<Button>     mButtons;
    
    PoolAllocator<Transform>  mTransforms;
    
    // Default assets
    
    struct DefaultShaders {
        Shader*  texture = nullptr;
        Shader*  textureUnlit = nullptr;
        Shader*  color = nullptr;
        Shader*  colorUnlit = nullptr;
        Shader*  UI = nullptr;
        Shader*  shadowCaster = nullptr;
        Shader*  sky = nullptr;
    };
    
    struct DefaultMeshes {
        Mesh* cube = nullptr;
        Mesh* chunk = nullptr;
        Mesh* plain = nullptr;
        Mesh* sphere = nullptr;
        Mesh* wallHorizontal = nullptr;
        Mesh* wallVertical = nullptr;
    };
    
    
    // Component data streaming
    unsigned int mDataStreamIndex;
    unsigned int mObjectIndex;
    unsigned int mStreamSize;
    
    ComponentDataStreamBuffer mStreamBuffer[ COMPONENT_STREAM_BUFFER_SIZE ];
    
public:
    
    /// Default shaders provided by the engine.
    DefaultShaders shaders;
    
    /// Default meshes provided by the engine.
    DefaultMeshes  meshes;
    
};


#ifndef BUILD_CORE
    extern EngineSystemManager  Engine;
#endif


#endif
