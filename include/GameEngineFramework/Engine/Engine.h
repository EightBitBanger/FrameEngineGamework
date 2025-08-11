#ifndef ENGINE_CORE
#define ENGINE_CORE

#include <GameEngineFramework/Engine/components/gameobject.h>
#include <GameEngineFramework/Engine/components/component.h>

#include <GameEngineFramework/application/Platform.h>
#include <GameEngineFramework/UI/UserInterfaceSystem.h>

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

#include <GameEngineFramework/Engine/EngineSystems.h>

#define  CONSOLE_NUMBER_OF_ELEMENTS   32
#define  PROFILER_NUMBER_OF_ELEMENTS  24


#ifndef BUILD_CORE
    #include <GameEngineFramework/Engine/EngineSystems.h>
#endif


#define COMPONENT_STREAM_BUFFER_SIZE   1024 * 1024   // One magabyte


class ENGINE_API EngineSystemManager {
    
public:
    
    /// Main active rendering scene.
    Scene* sceneMain;
    /// Overlay rendering scene.
    Scene* sceneOverlay;
    /// Assigned game object acting as a camera controller object.
    GameObject* cameraController;
    
    // Game objects
    
    /// Get the number of game objects.
    unsigned int GetNumberOfGameObjects(void);
    /// Return a pointer to a game object at the index position.
    GameObject* GetGameObject(unsigned int index);
    
    // Special components and game objects
    
    /// Create a camera controller game object and return its pointer.
    GameObject* CreateCameraController(glm::vec3 position);
    /// Generate a sky object and return its pointer.
    GameObject* CreateSky(std::string meshTagName, Color colorLow, Color colorHigh, float biasMul);
    
    // Lower level UI sprite rendering
    
    /// Add a string of sprite quads to a mesh.
    void AddMeshText(GameObject* overlayObject, float xPos, float yPos, float width, float height, std::string text, Color textColor);
    /// Add a quad to a mesh mapping to a sub sprite from a sprite sheet texture.
    void AddMeshSubSprite(GameObject* overlayObject, float xPos, float yPos, float width, float height, int index, Color meshColor);
    
    /// Get a collider from the collider list. If one does not exist it will be generated.
    rp3d::BoxShape* GetColliderBox(glm::vec3 extents);
    
    
    
    EngineSystemManager();
    
    /// Initiate the engine.
    void Initiate(void);
    /// Call update on engine components.
    void Update(void);
    /// Shutdown the engine.
    void Shutdown(void);
    
    /// Create an object of the type specified.
    template <typename T> T* Create(void) {
        std::lock_guard<std::mutex> lock(mux);
        extern ActorSystem AI;
        
        if (std::is_same<T, GameObject>::value)    return (T*)CreateGameObject();
        
        if (std::is_same<T, Mesh>::value)          return (T*)Renderer.CreateMesh();
        if (std::is_same<T, Material>::value)      return (T*)Renderer.CreateMaterial();
        if (std::is_same<T, Shader>::value)        return (T*)Renderer.CreateShader();
        if (std::is_same<T, Scene>::value)         return (T*)Renderer.CreateScene();
        if (std::is_same<T, Camera>::value)        return (T*)Renderer.CreateCamera();
        if (std::is_same<T, Light>::value)         return (T*)Renderer.CreateLight();
        if (std::is_same<T, MeshRenderer>::value)  return (T*)Renderer.CreateMeshRenderer();
        
        if (std::is_same<T, Actor>::value)         return (T*)AI.CreateActor();
        
        Log.Write("!! Creating invalid object");
        return nullptr;
    }
    
    /// Destroy an object of the type specified.
    template <typename T> bool Destroy(T* objectPtr) {
        std::lock_guard<std::mutex> lock(mux);
        extern ActorSystem AI;
        
        if (std::is_same<T, GameObject>::value)    return DestroyGameObject( (GameObject*)objectPtr );
        if (std::is_same<T, Component>::value)     return DestroyComponent( (Component*)objectPtr );
        
        if (std::is_same<T, Mesh>::value)          return Renderer.DestroyMesh( (Mesh*)objectPtr );
        if (std::is_same<T, Material>::value)      return Renderer.DestroyMaterial( (Material*)objectPtr );
        if (std::is_same<T, Shader>::value)        return Renderer.DestroyShader( (Shader*)objectPtr );
        if (std::is_same<T, Scene>::value)         return Renderer.DestroyScene( (Scene*)objectPtr );
        if (std::is_same<T, Camera>::value)        return Renderer.DestroyCamera( (Camera*)objectPtr );
        if (std::is_same<T, Light>::value)         return Renderer.DestroyLight( (Light*)objectPtr );
        if (std::is_same<T, MeshRenderer>::value)  return Renderer.DestroyMeshRenderer( (MeshRenderer*)objectPtr );
        
        if (std::is_same<T, Script>::value)        return Scripting.DestroyScript((Script*)objectPtr);
        if (std::is_same<T, RigidBody>::value)     return Physics.DestroyRigidBody((RigidBody*)objectPtr);
        if (std::is_same<T, Actor>::value)         return AI.DestroyActor( (Actor*)objectPtr );
        if (std::is_same<T, Sound>::value)         return Audio.DestroySound((Sound*)objectPtr);
        
        Log.Write("!! Destroying invalid object");
        return false;
    }
    
    /// Create a component object containing the type specified.
    template <typename T> Component* CreateComponent(void) {
        void* componentObjectPtr = nullptr;
        ComponentType type = EngineComponents::Undefined;
        
        if (std::is_same<T, Transform>::value)    {type = EngineComponents::Transform;     componentObjectPtr = (void*)mTransforms.Create();}
        if (std::is_same<T, MeshRenderer>::value) {type = EngineComponents::MeshRenderer;  componentObjectPtr = (void*)Renderer.CreateMeshRenderer();}
        if (std::is_same<T, Camera>::value)       {type = EngineComponents::Camera;        componentObjectPtr = (void*)Renderer.CreateCamera();}
        if (std::is_same<T, Light>::value)        {type = EngineComponents::Light;         componentObjectPtr = (void*)Renderer.CreateLight();}
        if (std::is_same<T, Script>::value)       {type = EngineComponents::Script;        componentObjectPtr = (void*)Scripting.CreateScript();}
        if (std::is_same<T, RigidBody>::value)    {type = EngineComponents::RigidBody;     componentObjectPtr = (void*)Physics.CreateRigidBody();}
        if (std::is_same<T, Actor>::value)        {type = EngineComponents::Actor;         componentObjectPtr = (void*)AI.CreateActor();}
        if (std::is_same<T, Sound>::value)        {type = EngineComponents::Sound;         componentObjectPtr = (void*)Audio.CreateSound();}
        
        if (type == EngineComponents::Undefined || componentObjectPtr == nullptr) {
            Log.Write("!! Created invalid component");
            return nullptr;
        }
        
        // Assemble the component
        Component* newComponent = mComponents.Create();
        newComponent->SetComponent(type, componentObjectPtr);
        return newComponent;
    }
    
    
    /// Destroy a component object.
    bool DestroyComponent(Component* componentPtr) {
        ComponentType componentType = componentPtr->GetType();
        switch (componentType) {
            case Components.RigidBody:    {Physics.DestroyRigidBody( (RigidBody*)componentPtr->GetComponent() ); break;}
            case Components.Actor:        {((Actor*)componentPtr->GetComponent())->isGarbage = true; break;}
            case Components.Transform:    {mTransforms.Destroy( (Transform*)componentPtr->GetComponent() ); break;}
            case Components.Camera:       {Renderer.DestroyCamera( (Camera*)componentPtr->GetComponent() ); break;}
            case Components.Light:        {Renderer.DestroyLight( (Light*)componentPtr->GetComponent() ); break;}
            case Components.MeshRenderer: {Renderer.DestroyMeshRenderer((MeshRenderer*)componentPtr->GetComponent()); break;}
            case Components.Script:       {Scripting.DestroyScript( (Script*)componentPtr->GetComponent() ); break;}
            case Components.Sound:        {Audio.DestroySound( (Sound*)componentPtr->GetComponent() ); break;}
            
        default: 
            Log.Write("!! Destroying invalid component");
            return false;
            break;
        }
        return true;
    }
    
    /// Get the number of component objects.
    unsigned int GetNumberOfComponents(void);
    
    /// Initiate rendering for the physics debugging meshes.
    void EnablePhysicsDebugRenderer(void);
    
    /// Render the physics debug lines / triangles.
    void UpdatePhysicsDebugRenderer(void);
    
    // Command console
    
    class ENGINE_API CommandConsole {
        
    public:
        
        bool doCloseConsoleAfterCommand;
        
        TextField* input;
        
        void Enable(void);
        void Disable(void);
        void Clear(void);
        
        void Print(std::string text);
        void WriteDialog(unsigned int index, std::string text);
        
        void ShiftUp(void);
        bool RegisterCommand(std::string name, void(*function_ptr)(std::vector<std::string>));
        void ConsoleReturnCallback(std::string& console_text);
        
        std::vector<Text*> textElements;
        std::vector<Text*> textDialog;
        
    } console;
    
    
    // List of box colliders
    std::vector<rp3d::BoxShape*>  mBoxCollider;
    
private:
    
    
    // Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    
    // Destroy a game object.
    bool DestroyGameObject(GameObject* gameObjectPtr);
    
    // Batch update engine components
    void UpdateTransformationChains(void);
    
    // Update component by index
    void UpdateMeshRenderer(unsigned int index);
    void UpdateRigidBody(unsigned int index);
    void UpdateCamera(unsigned int index);
    void UpdateLight(unsigned int index);
    void UpdateAudio(unsigned int index);
    
    // Update actor kinematic motion
    void UpdateKinematics(void);
    
    // Actor genetics update
    void ClearOldGeneticRenderers(unsigned int index);
    void GenerateCollider(Actor* actor);
    MeshRenderer* CreateMeshRendererForGene(unsigned int index, unsigned int geneIndex, Mesh* sourceMesh);
    void ExpressActorGenetics(unsigned int index);
    
    // List of active game objects
    std::vector<GameObject*>  mGameObjectActive;
    
    // Component allocators
    PoolAllocator<GameObject> mGameObjects;
    PoolAllocator<Component>  mComponents;
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
        Shader*  water = nullptr;
    };
    
    struct DefaultMeshes {
        Mesh* grass = nullptr;
        Mesh* grassHorz = nullptr;
        Mesh* grassVert = nullptr;
        Mesh* stemHorz = nullptr;
        Mesh* stemVert = nullptr;
        Mesh* wallHorizontal = nullptr;
        Mesh* wallVertical = nullptr;
        Mesh* log = nullptr;
        Mesh* leaf = nullptr;
        
        Mesh* cube = nullptr;
        Mesh* chunk = nullptr;
        Mesh* plain = nullptr;
        Mesh* sphere = nullptr;
    };
    
    void UpdateComponentStream(void);
    
    unsigned int mDataStreamIndex;
    unsigned int mObjectIndex;
    unsigned int mStreamSize;
    
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
        // Audio
        Sound*         sound;
    };
    
    ComponentDataStreamBuffer mStreamBuffer[ COMPONENT_STREAM_BUFFER_SIZE ];
    
    // Debug rendering
    bool usePhysicsDebugRenderer;
    
    std::mutex mux;
    
public:
    
    GameObject* debugMeshGameObject;
    GameObject* debugLinesGameObject;
    
    /// Default shaders provided by the engine.
    DefaultShaders shaders;
    
    /// Default meshes provided by the engine.
    DefaultMeshes  meshes;
    
};


#ifndef BUILD_CORE
    extern EngineSystemManager  Engine;
#endif


#endif
