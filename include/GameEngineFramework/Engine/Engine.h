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

#include <GameEngineFramework/Engine/EngineSystems.h>

#define  CONSOLE_NUMBER_OF_ELEMENTS   32
#define  PROFILER_NUMBER_OF_ELEMENTS  24


#ifndef BUILD_CORE
    #include <GameEngineFramework/Engine/EngineSystems.h>
#endif

// UI callback type
typedef void(*ButtonCallBack)(Button*);


#define COMPONENT_STREAM_BUFFER_SIZE   1024 * 64


class ENGINE_API EngineSystemManager {
    
public:
    
    /// Main active rendering scene.
    Scene* sceneMain;
    
    /// Overlay rendering scene. (For UI elements)
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
    
    /// Create an AI actor component and return its pointer.
    GameObject* CreateAIActor(glm::vec3 position);
    
    
    // UI elements
    
    /// Create an empty overlay renderer and return its object.
    GameObject* CreateOverlayRenderer(void);
    
    /// Create a text overlay and return its object.
    GameObject* CreateOverlayTextRenderer(float x, float y, std::string text, float textSize, Color color, std::string materialTag);
    
    /// Create a panel overlay and return its object.
    GameObject* CreateOverlayPanelRenderer(float x, float y, float width, float height, std::string materialTag);
    
    /// Create a button callback with an overlay panel and text.
    Button* CreateButtonUI(float buttonX, float buttonY, float buttonW, float buttonH, float textX, float textY, std::string text, std::string materialName, ButtonCallBack callback);
    
    
    // Mouse button event callback
    
    /// Create a button and return its pointer.
    Button* CreateOverlayButtonCallback(float x, float y, float width, float height, ButtonCallBack callback);
    
    /// Destroy a button 
    bool DestroyOverlayButtonCallback(Button* button);
    
    // Console
    
    /// Enable and render the console text elements.
    void EnableConsole(void);
    
    /// Disable and deactivate the rendering of the console text elements.
    void DisableConsole(void);
    
    /// Return whether the console is currently active
    bool CheckIsConsoleActive(void);
    
    /// Enable console back panel.
    void EnableConsoleBackPanel(void);
    
    /// Disable console back panel.
    void DisableConsoleBackPanel(void);
    
    /// Enable the fade out of text elements after a period of time.
    void EnableConsoleFadeOutTextElements(void);
    
    /// Disable the fade out of text elements after a period of time.
    void DisableConsoleFadeOutTextElements(void);
    
    /// Set the number of frames after which the text element will begin fading out.
    void SetConsoleFadeOutTimer(unsigned int numberOfFrames);
    
    /// Enable automatic close after a command is entered.
    void EnableConsoleCloseOnReturn(void);
    
    /// Disable automatic close after a command is entered.
    void DisableConsoleCloseOnReturn(void);
    
    /// Register a command function into the console to be used at runtime.
    void ConsoleRegisterCommand(std::string name, void(*function)(std::vector<std::string>));
    
    /// Clear the text in the console input string.
    void ConsoleClearInputString(void);
    
    /// Clear the logged messaged above the console input string.
    void ConsoleClearLog(void);
    
    /// Shift the console log text elements up and add the text string into the console log.
    void Print(std::string text, unsigned int fadeTimer = 0);
    
    /// Write a string of text to the dialog text element referenced by index.
    void WriteDialog(unsigned int index, std::string text);
    
    // Profiler
    
    /// Allow the profiler to print the results to the display.
    void EnableProfiler(void);
    
    /// Disable the profiler from printing to the display.
    void DisableProfiler(void);
    
    /// Return whether the profiler is currently active
    bool CheckIsProfilerActive(void);
    
    // Lower level UI rendering
    
    /// Add a string of sprite quads to a mesh.
    void AddMeshText(GameObject* overlayObject, float xPos, float yPos, float width, float height, std::string text, Color textColor);
    
    /// Add a quad to a mesh mapping to a sub sprite from a sprite sheet texture.
    void AddMeshSubSprite(GameObject* overlayObject, float xPos, float yPos, float width, float height, int index, Color meshColor);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    // Height field chunk generation
    
    // Perlin generation
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    EngineSystemManager();
    
    /// Initiate the engine.
    void Initiate(void);
    
    /// Call update on engine components.
    void Update(void);
    
    /// Shutdown the engine.
    void Shutdown(void);
    
    /// Create an object of the type specified.
    template <typename T> T* Create(void) {
        
        extern ActorSystem AI;
        
        // Engine
        if (std::is_same<T, GameObject>::value)    return (T*)CreateGameObject();
        
        // Renderer
        if (std::is_same<T, Mesh>::value)          return (T*)Renderer.CreateMesh();
        if (std::is_same<T, Material>::value)      return (T*)Renderer.CreateMaterial();
        if (std::is_same<T, Shader>::value)        return (T*)Renderer.CreateShader();
        if (std::is_same<T, Scene>::value)         return (T*)Renderer.CreateScene();
        if (std::is_same<T, Camera>::value)        return (T*)Renderer.CreateCamera();
        if (std::is_same<T, MeshRenderer>::value)  return (T*)Renderer.CreateMeshRenderer();
        
        // AI
        if (std::is_same<T, Actor>::value)         return (T*)AI.CreateActor();
        
        return nullptr;
    }
    
    /// Destroy an object of the type specified.
    template <typename T> bool Destroy(T* objectPtr) {
        
        extern ActorSystem AI;
        
        if (std::is_same<T, GameObject>::value)    return DestroyGameObject( (GameObject*)objectPtr );
        if (std::is_same<T, Component>::value)     return DestroyComponent( (Component*)objectPtr );
        
        if (std::is_same<T, Mesh>::value)          return Renderer.DestroyMesh( (Mesh*)objectPtr );
        if (std::is_same<T, Material>::value)      return Renderer.DestroyMaterial( (Material*)objectPtr );
        if (std::is_same<T, Shader>::value)        return Renderer.DestroyShader( (Shader*)objectPtr );
        if (std::is_same<T, Scene>::value)         return Renderer.DestroyScene( (Scene*)objectPtr );
        if (std::is_same<T, Camera>::value)        return Renderer.DestroyCamera( (Camera*)objectPtr );
        if (std::is_same<T, MeshRenderer>::value)  return Renderer.DestroyMeshRenderer( (MeshRenderer*)objectPtr );
        
        if (std::is_same<T, Actor>::value)         return AI.DestroyActor( (Actor*)objectPtr );
        
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
        if (std::is_same<T, Text>::value)         {type = EngineComponents::Text;          componentObjectPtr = (void*)mTextObjects.Create();}
        if (std::is_same<T, Panel>::value)        {type = EngineComponents::Panel;         componentObjectPtr = (void*)mPanelObjects.Create();}
        if (std::is_same<T, Sound>::value)        {type = EngineComponents::Sound;         componentObjectPtr = (void*)Audio.CreateSound();}
        
        // Check bad type
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
            
            case Components.MeshRenderer: {
                MeshRenderer* meshRenderer = (MeshRenderer*)componentPtr->GetComponent();
                Renderer.DestroyMeshRenderer(meshRenderer);
                break;
            }
            
            case Components.RigidBody: {
                rp3d::RigidBody* bodyPtr = (RigidBody*)componentPtr->GetComponent();
                bodyPtr->setIsActive( false );
                Physics.DestroyRigidBody( bodyPtr );
                break;
            }
            
            case Components.Actor: {
                Actor* actorPtr = (Actor*)componentPtr->GetComponent();
                for (unsigned int i=0; i < actorPtr->genetics.mGeneticRenderers.size(); i++) 
                    sceneMain->RemoveMeshRendererFromSceneRoot( actorPtr->genetics.mGeneticRenderers[i], RENDER_QUEUE_GEOMETRY );
                for (unsigned int i=0; i < actorPtr->genetics.mGeneticRenderers.size(); i++) 
                    Destroy<MeshRenderer>( actorPtr->genetics.mGeneticRenderers[i] );
                actorPtr->genetics.mGeneticRenderers.clear();
                AI.DestroyActor( actorPtr );
                break;
            }
            
            case Components.Transform: {mTransforms.Destroy( (Transform*)componentPtr->GetComponent() ); break;}
            case Components.Camera:    {Renderer.DestroyCamera( (Camera*)componentPtr->GetComponent() ); break;}
            case Components.Light:     {Renderer.DestroyLight( (Light*)componentPtr->GetComponent() ); break;}
            case Components.Script:    {Scripting.DestroyScript( (Script*)componentPtr->GetComponent() ); break;}
            case Components.Text:      {mTextObjects.Destroy( (Text*)componentPtr->GetComponent() ); break;}
            case Components.Panel:     {mPanelObjects.Destroy( (Panel*)componentPtr->GetComponent() ); break;}
            case Components.Sound:     {Audio.DestroySound( (Sound*)componentPtr->GetComponent() ); break;}
            
            default: break;
        }
        
        return true;
    }
    
    /// Get the number of component objects.
    unsigned int GetNumberOfComponents(void);
    
    /// Initiate rendering for the physics debugging meshes.
    void EnablePhysicsDebugRenderer(void);
    
    /// Render the physics debug lines / triangles.
    void UpdatePhysicsDebugRenderer(void);
    
    
private:
    
    // Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    
    // Destroy a game object.
    bool DestroyGameObject(GameObject* gameObjectPtr);
    
    
    //
    // Profiler
    
    bool mIsProfilerEnabled;
    
    GameObject* mProfilerTextObjects[PROFILER_NUMBER_OF_ELEMENTS];
    
    Text* mProfilerText[PROFILER_NUMBER_OF_ELEMENTS];
    
    
    //
    // Console
    
    bool mIsConsoleEnabled;
    bool mShowConsoleBackPanel;
    bool mConsoleCloseAfterCommandEntered;
    bool mConsoleDoFadeOutTexts;
    
    unsigned int mConsoleFadeOutTimer;
    
    std::string mConsolePrompt;
    std::string mConsoleString;
    
    Text* mConsoleInput;
    Text* mConsoleText[CONSOLE_NUMBER_OF_ELEMENTS];
    
    GameObject* mConsoleInputObject;
    GameObject* mConsoleTextObjects[CONSOLE_NUMBER_OF_ELEMENTS];
    
    GameObject* mConsolePanelObject;
    
    unsigned int mConsoleTimers[CONSOLE_NUMBER_OF_ELEMENTS];
    
    // Command function routing
    struct ConsoleCommand {
        std::string name;
        void(*function)(std::vector<std::string>);
        ConsoleCommand() : 
            name(""),
            function(nullptr)
        {}
    };
    
    std::vector<ConsoleCommand> mConsoleCommands;
    
    
    // Batch update engine components
    void UpdateTransformationChains(void);
    void UpdateUI(void);
    
    // Console
    void UpdateConsole(void);
    
    // Update component by index
    void UpdateMeshRenderer(unsigned int index);
    void UpdateRigidBody(unsigned int index);
    void UpdatePanelUI(unsigned int index);
    void UpdateTextUI(unsigned int index);
    void UpdateCamera(unsigned int index);
    void UpdateLight(unsigned int index);
    void UpdateAudio(unsigned int index);
    
    // AI update functions
    void UpdateActor(unsigned int index);
    void UpdateActorTargetRotation(unsigned int index);
    void UpdateActorGenetics(unsigned int index);
    void UpdateActorAnimation(unsigned int index);
    void UpdateActorPhysics(unsigned int index);
    
    // Actor genetics update
    void ClearOldGeneticRenderers(unsigned int index);
    void GenerateCollider(unsigned int index);
    MeshRenderer* CreateMeshRendererForGene(unsigned int index, unsigned int geneIndex, Mesh* sourceMesh);
    void ExpressActorGenetics(unsigned int index);
    
    // Actor animation update
    void EnsureNonZeroAnimationState(unsigned int index, unsigned int a);
    void ApplyAnimationRotation(glm::mat4& matrix, unsigned int index, unsigned int a);
    void HandleAnimationSwingBackward(unsigned int index, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange);
    void HandleAnimationSwingForward(unsigned int index, unsigned int a, glm::vec4& animationFactor, float animationMaxSwingRange);
    void UpdateAnimation(glm::mat4& matrix, unsigned int index, unsigned int a);
    void ApplyOffsetFromCenter(glm::mat4& matrix, unsigned int index, unsigned int a);
    void ApplyRotation(glm::mat4& matrix, unsigned int index, unsigned int a);
    void ApplyScaleByAge(glm::mat4& matrix, unsigned int index, unsigned int a);
    
    
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
        Shader*  water = nullptr;
    };
    
    struct DefaultMeshes {
        
        Mesh* grassHorz = nullptr;
        Mesh* grassVert = nullptr;
        
        Mesh* stemHorz = nullptr;
        Mesh* stemVert = nullptr;
        
        Mesh* wallHorizontal = nullptr;
        Mesh* wallVertical = nullptr;
        
        Mesh* log = nullptr;
        
        Mesh* cube = nullptr;
        Mesh* chunk = nullptr;
        Mesh* plain = nullptr;
        Mesh* sphere = nullptr;
        
    };
    
    struct DefaultColliders {
        
        rp3d::BoxShape*  box = nullptr;
        
    };
    
    //
    // Component stream
    //
    
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
        // UI
        Text*          text;
        Panel*         panel;
        // Audio
        Sound*         sound;
    };
    
    ComponentDataStreamBuffer mStreamBuffer[ COMPONENT_STREAM_BUFFER_SIZE ];
    
    // Debug rendering
    bool usePhysicsDebugRenderer;
    
    
public:
    
    GameObject* debugMeshGameObject;
    GameObject* debugLinesGameObject;
    
    /// Default shaders provided by the engine.
    DefaultShaders shaders;
    
    /// Default meshes provided by the engine.
    DefaultMeshes  meshes;
    
    /// Default collision shapes
    DefaultColliders colliders;
    
};


#ifndef BUILD_CORE
    extern EngineSystemManager  Engine;
#endif


#endif
