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
#include <GameEngineFramework/Engine/EngineComponents.h>

#define  CONSOLE_NUMBER_OF_ELEMENTS   32
#define  PROFILER_NUMBER_OF_ELEMENTS  24

#ifndef BUILD_CORE
    #include <GameEngineFramework/Engine/EngineSystems.h>
#endif


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
    
    /// Get the number of elements in the component stream.
    unsigned int GetStreamSize(void);
    
    
    EngineSystemManager();
    
    /// Initiate the engine.
    void Initiate(void);
    /// Call update on engine components.
    void Update(void);
    /// Shutdown the engine.
    void Shutdown(void);
    
    /// Create an object of the specified type.
    template <typename T> T* Create(void) {
        std::lock_guard<std::mutex> lock(mux);
        
        if (std::is_same<T, GameObject>::value) 
            return (T*)CreateGameObject();
        
        ComponentType type = (ComponentType)mObjectRegistry.GetID<T>();
        if (type == -1) {
            Log.Write("!! Creating invalid object (type not registered)");
            return nullptr;
        }
        
        std::unordered_map<ComponentType, void*(*)()>::iterator it = mObjectBuilders.find(type);
        if (it == mObjectBuilders.end() || it->second == nullptr) {
            Log.Write("!! Creating invalid object (no builder registered)");
            return nullptr;
        }
        
        void* objectPtr = (void*)it->second();
        if (objectPtr == nullptr) {
            Log.Write("!! Creating invalid object (builder returned nullptr)");
            return nullptr;
        }
        
        return (T*)objectPtr;
    }
    
    /// Destroy an object of the specified type.
    template <typename T> bool Destroy(T* objectPtr) {
        std::lock_guard<std::mutex> lock(mux);
        
        if (objectPtr == nullptr) {
            Log.Write("!! Destroy called with nullptr");
            return false;
        }
        
        if (std::is_same<T, GameObject>::value) 
            return DestroyGameObject((GameObject*)objectPtr);
        
        if (std::is_same<T, Component>::value) 
            return DestroyComponent((Component*)objectPtr);
        
        // General objects: look up the type in the registry
        ComponentType type = (ComponentType)mObjectRegistry.GetID<T>();
        if (type == -1) {
            Log.Write("!! Destroying invalid object (type not registered)");
            return false;
        }
        
        std::unordered_map<ComponentType, void(*)(void*)>::iterator it = mObjectDestructors.find(type);
        if (it == mObjectDestructors.end() || it->second == nullptr) {
            Log.Write("!! Destroying invalid object (no destructor registered)");
            return false;
        }
        
        it->second((void*)objectPtr);
        return true;
    }
    
    /// Create a component object containing the specified type.
    template <typename T> Component* CreateComponent(void) {
        void* componentObjectPtr = nullptr;
        
        ComponentType type = -1;
        type = (ComponentType)mComponentRegistry.GetID<T>();
        
        void*(*functionPtr)() = mComponentBuilders[type];
        componentObjectPtr = (void*)functionPtr();
        
        if (type == -1 || componentObjectPtr == nullptr) {
            Log.Write("!! Creating invalid component");
            return nullptr;
        }
        
        Component* newComponent = mComponents.Create();
        newComponent->SetComponent(type, componentObjectPtr);
        return newComponent;
    }
    
    /// Create a component object from an existing object.
    template <typename T> Component* CreateComponentFromObject(T* object) {
        ComponentType type = -1;
        type = (ComponentType)mComponentRegistry.GetID<T>();
        
        if (type == -1 || object == nullptr) {
            Log.Write("!! Creating invalid component");
            return nullptr;
        }
        
        Component* newComponent = mComponents.Create();
        newComponent->SetComponent(type, object);
        return newComponent;
    }
    
    /// Destroy a component object.
    bool DestroyComponent(Component* componentPtr) {
        ComponentType componentType = componentPtr->GetType();
        void(*destroyer)(void*) = mComponentDestructors[componentType];
        destroyer( componentPtr->GetComponent() );
        componentPtr->mObject = nullptr;
        return true;
    }
    
    /// Add a new component to the component registry.
    template <typename T> bool RegisterComponent(ComponentType type, std::string name, 
                                                 void*(*CreateFunction)(), 
                                                 void(*DestroyFunction)(void*), 
                                                 void(*UpdateFunction)(unsigned int), 
                                                 ComponentUpdateType updateType) {
        if (!mComponentRegistry.RegisterComponentType<T>(type)) 
            return false;
        
        mComponentNames[type] = name;
        if (CreateFunction  != nullptr) mComponentBuilders[type]    = CreateFunction;
        if (DestroyFunction != nullptr) mComponentDestructors[type] = DestroyFunction;
        if (UpdateFunction  != nullptr) mComponentUpdaters.push_back(UpdateFunction);
        
        if (updateType != ComponentUpdateType::NoUpdate) 
            mComponentUpdateType.push_back(updateType);
        return true;
    }
    
    /// Add a new plug-in component to the component registry.
    template <typename T> bool RegisterPlugin(std::string name, 
                                              void*(*CreateFunction)(), 
                                              void(*DestroyFunction)(void*), 
                                              void(*UpdateFunction)(unsigned int), 
                                              ComponentUpdateType updateType) {
        unsigned int pluginID = mComponentRegistry.GetNumberOfComponents();
        
        if (!mComponentRegistry.RegisterComponentType<T>(pluginID)) 
            return false;
        
        mComponentNames[pluginID] = name;
        if (CreateFunction  != nullptr) mComponentBuilders[pluginID]    = CreateFunction;
        if (DestroyFunction != nullptr) mComponentDestructors[pluginID] = DestroyFunction;
        if (UpdateFunction  != nullptr) mComponentUpdaters.push_back(UpdateFunction);
        
        if (updateType != ComponentUpdateType::NoUpdate) 
            mComponentUpdateType.push_back(updateType);
        return true;
    }
    
    /// Add a new object to the object registry.
    template <typename T> bool RegisterObject(ComponentType type, std::string name, void*(*CreateFunction)(), void(*DestroyFunction)(void*)) {
        if (!mObjectRegistry.RegisterComponentType<T>(type)) 
            return false;
        mObjectBuilders[type] = CreateFunction;
        mObjectDestructors[type] = DestroyFunction;
        return true;
    }
    
    /// Create a transform object and return its pointer.
    Transform* CreateTransform(void);
    
    /// Destroy a transform object.
    bool DestroyTransform(Transform* transform);
    
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
        void ClearInput(void);
        
        void Print(std::string text);
        void WriteDialog(unsigned int index, std::string text);
        
        void ShiftUp(void);
        bool RegisterCommand(std::string name, void(*function_ptr)(std::vector<std::string>));
        void ConsoleReturnCallback(std::string& console_text);
        
        std::vector<Text*> textElements;
        std::vector<Text*> textDialog;
        
    } console;
    
    
    // Update component by index
    void UpdateMeshRenderer(unsigned int index);
    void UpdateRigidBody(unsigned int index);
    void UpdateCamera(unsigned int index);
    void UpdateLight(unsigned int index);
    void UpdateAudio(unsigned int index);
    void UpdateTransforms(unsigned int index);
    void UpdateKinematics(unsigned int index);
    
private:
    
    // Create a game object and return its pointer.
    GameObject* CreateGameObject(void);
    // Destroy a game object.
    bool DestroyGameObject(GameObject* gameObjectPtr);
    
    // List of active game objects
    std::vector<GameObject*>  mGameObjectActive;
    
    // Component allocators
    PoolAllocator<GameObject> mGameObjects;
    PoolAllocator<Component>  mComponents;
    PoolAllocator<Transform>  mTransforms;
    
    // Component type registry
    ComponentTypeRegistry mComponentRegistry;
    std::unordered_map<ComponentType, void*(*)()> mComponentBuilders;
    std::unordered_map<ComponentType, void(*)(void*)> mComponentDestructors;
    std::unordered_map<ComponentType, std::string> mComponentNames;
    std::vector<void(*)(unsigned int)> mComponentUpdaters;
    std::vector<ComponentUpdateType> mComponentUpdateType;
    
    // General object registry
    ComponentTypeRegistry mObjectRegistry;
    std::unordered_map<ComponentType, void*(*)()> mObjectBuilders;
    std::unordered_map<ComponentType, void(*)(void*)> mObjectDestructors;
    
    // Component stream
    void UpdateComponentStream(void);
    
    unsigned int mDataStreamIndex;
    unsigned int mObjectIndex;
    unsigned int mStreamSize;
    
    struct ComponentDataStreamBuffer {
        GameObject* gameObject;
        uint64_t componentMask;
        void* components[EngineComponent::NumberOfComponents];
    };
    
    std::vector<ComponentDataStreamBuffer> mStreamBuffer;
    
    // Debug rendering
    bool usePhysicsDebugRenderer;
    
    std::mutex mux;
    
public:
    
    GameObject* debugMeshGameObject;
    GameObject* debugLinesGameObject;
};


#ifndef BUILD_CORE
    extern EngineSystemManager  Engine;
#endif

#endif
