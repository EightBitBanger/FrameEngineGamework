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

#define  CONSOLE_NUMBER_OF_ELEMENTS   32
#define  PROFILER_NUMBER_OF_ELEMENTS  24


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
    void ConsoleShiftUp(std::string text);
    
    
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
    
    /// Initiates a height field grid array of points and set them to zero.
    void SetHeightFieldValues(float* heightField, unsigned int width, unsigned int height, float value);
    
    /// Add a layer of perlin noise into a height field.
    void AddHeightFieldFromPerlinNoise(float* heightField, unsigned int width, unsigned int height, float noiseWidth, float noiseHeight, float noiseMul, int offsetX, int offsetZ);
    
    /// Average a layer of perlin noise into a height field.
    void AverageHeightFieldFromPerlinNoise(float* heightField, unsigned int width, unsigned int height, float noiseWidth, float noiseHeight, float noiseMul, int offsetX, int offsetZ);
    
    // Color field
    
    /// Initiates a color field grid array of colors and set them to zero.
    void SetColorFieldValues(glm::vec3* colorField, unsigned int width, unsigned int height, Color color);
    
    /// Generate a color field containing a color range from from low to high. The bias will determine the fade 
    /// from the low color to the high color based on the height field values.
    void GenerateColorFieldFromHeightField(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color low, Color high, float bias);
    
    /// Set a layer of perlin noise into a color field. The perlin noise is used to fade from the first color to the second.
    void SetColorFieldFromPerlinNoise(glm::vec3* colorField, unsigned int width, unsigned int height, float noiseWidth, float noiseHeight, float noiseThreshold, Color first, Color second, int offsetX, int offsetZ);
    
    /// Generate a snow cap effect of a given color capColor and starting at the height beginHeight.
    /// The bias will determine how much snow will be added.
    void AddColorFieldSnowCap(glm::vec3* colorField, float* heightField, unsigned int width, unsigned int height, Color capColor, float beginHeight, float bias);
    
    // Mapping to a mesh
    
    /// Apply the height field values to a mesh.
    void AddHeightFieldToMesh(Mesh* mesh, float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ, unsigned int subTessX=1.0f, unsigned int subTessZ=1.0f);
    
    /// Generate a height field mesh from perlin noise.
    Mesh* CreateMeshFromHeightField(float* heightField, glm::vec3* colorField, unsigned int width, unsigned int height, float offsetX, float offsetZ);
    
    
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
        if (std::is_same<T, Component>::value)     return (T*)CreateComponent( COMPONENT_TYPE_UNDEFINED );
        
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
        
        // Engine
        if (std::is_same<T, GameObject>::value)    {GameObject* ptr = (GameObject*)objectPtr;
                                                    ptr->mIsGarbage = true;
                                                    return true;}
        if (std::is_same<T, Component>::value)     return DestroyComponent( (Component*)objectPtr );
        
        // Renderer
        if (std::is_same<T, Mesh>::value)          return Renderer.DestroyMesh( (Mesh*)objectPtr );
        if (std::is_same<T, Material>::value)      return Renderer.DestroyMaterial( (Material*)objectPtr );
        if (std::is_same<T, Shader>::value)        return Renderer.DestroyShader( (Shader*)objectPtr );
        if (std::is_same<T, Scene>::value)         return Renderer.DestroyScene( (Scene*)objectPtr );
        if (std::is_same<T, Camera>::value)        return Renderer.DestroyCamera( (Camera*)objectPtr );
        if (std::is_same<T, MeshRenderer>::value)  return Renderer.DestroyMeshRenderer( (MeshRenderer*)objectPtr );
        
        // AI
        if (std::is_same<T, Actor>::value)         return AI.DestroyActor( (Actor*)objectPtr );
        
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
    
    
    /// Destroy a component object.
    bool DestroyComponent(Component* componentPtr);
    
    /// Get the number of component objects.
    unsigned int GetNumberOfComponents(void);
    
    /// Initiate rendering for the physics debugging meshes.
    void EnablePhysicsDebugRenderer(void);
    
    /// Render the physics debug lines / triangles.
    void UpdatePhysicsDebugRenderer(void);
    
    
private:
    
    // Create a component object with initial type information and return its pointer.
    Component* CreateComponent(ComponentType type);
    
    // Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    
    // Destroy a game object.
    bool DestroyGameObject(GameObject* gameObjectPtr);
    
    // Process the objects marked as garbage
    void ProcessDeferredDeletion(void);
    
    
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
    
    // AI update functions
    void UpdateActor(unsigned int index);
    void UpdateActorTargetRotation(unsigned int index);
    void UpdateActorGenetics(unsigned int index);
    void UpdateActorAnimation(unsigned int index);
    
    // List of active game objects
    std::vector<GameObject*>  mGameObjectActive;
    
    // Garbage collection
    
    // Garbage game objects
    std::vector<GameObject*>  mGarbageGameObjects;
    
    // Garbage rigid bodies
    std::vector<RigidBody*>  mGarbageRigidBodies;
    
    // Clean rigid bodies
    std::vector<RigidBody*>  mFreeRigidBodies;
    
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
    };
    
    ComponentDataStreamBuffer mStreamBuffer[ COMPONENT_STREAM_BUFFER_SIZE ];
    
    // Debug rendering
    bool usePhysicsDebugRenderer;
    
    Mesh* debugMesh;
    Mesh* debugLines;
    
    
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
