// Engine component related functions
//

#include <GameEngineFramework/Engine/Engine.h>

__declspec(dllexport) extern EngineComponents  Components;
__declspec(dllexport) extern ColorPreset       Colors;
__declspec(dllexport) extern RandomGen         Random;
__declspec(dllexport) extern Logger            Log;
__declspec(dllexport) extern Timer             PhysicsTime;
__declspec(dllexport) extern Timer             Time;

__declspec(dllexport) extern Serialization     Serializer;
__declspec(dllexport) extern ResourceManager   Resources;
__declspec(dllexport) extern ScriptSystem      Scripting;
__declspec(dllexport) extern RenderSystem      Renderer;
__declspec(dllexport) extern PhysicsSystem     Physics;
__declspec(dllexport) extern AudioSystem       Audio;
__declspec(dllexport) extern InputSystem       Input;
__declspec(dllexport) extern MathCore          Math;
__declspec(dllexport) extern ActorSystem       AI;

__declspec(dllexport) extern ApplicationLayer      Application;
__declspec(dllexport) extern EngineSystemManager   Engine;



Component* EngineSystemManager::CreateComponent(ComponentType type) {
    void* component_object = nullptr;
    
    switch (type) {
        
        case COMPONENT_TYPE_MESH_RENDERER: {
            component_object = (void*)Renderer.CreateMeshRenderer();
            break;
        }
        case COMPONENT_TYPE_CAMERA: {
            component_object = (void*)Renderer.CreateCamera();
            break;
        }
        case COMPONENT_TYPE_LIGHT: {
            component_object = (void*)Renderer.CreateLight();
            break;
        }
        case COMPONENT_TYPE_SCRIPT: {
            component_object = (void*)Scripting.CreateScript();
            break;
        }
        case COMPONENT_TYPE_RIGID_BODY: {
            component_object = (void*)Physics.CreateRigidBody();
            break;
        }
        case COMPONENT_TYPE_ACTOR: {
            component_object = (void*)AI.CreateActor();
            break;
        }
        case COMPONENT_TYPE_TEXT: {
            component_object = (void*)mTextObjects.Create();
            break;
        }
        case COMPONENT_TYPE_PANEL: {
            component_object = (void*)mPanelObjects.Create();
            break;
        }
        
        default:
            return nullptr;
    }
    
    Component* newComponent = mComponents.Create();
    newComponent->SetComponent(type, component_object);
    
    doUpdateDataStream = true;
    return newComponent;
}

bool EngineSystemManager::DestroyComponent(Component* componentPtr) {
    assert(componentPtr != nullptr);
    
    ComponentType componentType = componentPtr->GetType();
    
    switch (componentType) {
        
        case COMPONENT_TYPE_MESH_RENDERER: {
            MeshRenderer* componentEntityRenderer = (MeshRenderer*)componentPtr->GetComponent();
            Renderer.DestroyMeshRenderer(componentEntityRenderer);
            break;
        }
        case COMPONENT_TYPE_CAMERA: {
            Camera* componentCamera = (Camera*)componentPtr->GetComponent();
            Renderer.DestroyCamera(componentCamera);
            break;
        }
        case COMPONENT_TYPE_LIGHT: {
            Light* componentLight = (Light*)componentPtr->GetComponent();
            Renderer.DestroyLight(componentLight);
            break;
        }
        case COMPONENT_TYPE_SCRIPT: {
            Script* componentScript = (Script*)componentPtr->GetComponent();
            Scripting.DestroyScript(componentScript);
            break;
        }
        case COMPONENT_TYPE_RIGID_BODY: {
            RigidBody* componentRigidBody = (RigidBody*)componentPtr->GetComponent();
            Physics.DestroyRigidBody(componentRigidBody);
            break;
        }
        case COMPONENT_TYPE_ACTOR: {
            Actor* actorObject = (Actor*)componentPtr->GetComponent();
            AI.DestroyActor(actorObject);
            break;
        }
        case COMPONENT_TYPE_TEXT: {
            Text* textObject = (Text*)componentPtr->GetComponent();
            mTextObjects.Destroy(textObject);
            break;
        }
        case COMPONENT_TYPE_PANEL: {
            Panel* panelObject = (Panel*)componentPtr->GetComponent();
            mPanelObjects.Destroy(panelObject);
            break;
        }
        
        default:
            return false;
    }
    mComponents.Destroy(componentPtr);
    
    doUpdateDataStream = true;
    return true;
}

Component* EngineSystemManager::CreateComponentMeshRenderer(Mesh* meshPtr, Material* materialPtr) {
    Component* rendererComponent = CreateComponent(Components.MeshRenderer);
    MeshRenderer* meshRenderer = (MeshRenderer*)rendererComponent->mObject;
    
    meshRenderer->mesh = meshPtr;
    meshRenderer->material = materialPtr;
    
    if (sceneMain != nullptr) 
        sceneMain->AddMeshRendererToSceneRoot( meshRenderer );
    
    doUpdateDataStream = true;
    return rendererComponent;
}

Component* EngineSystemManager::CreateComponentLight(glm::vec3 position) {
    Component* lightComponent = CreateComponent(Components.Light);
    Light* lightPoint = (Light*)lightComponent->mObject;
    
    lightPoint->position = position;
    
    if (sceneMain != nullptr) 
        sceneMain->AddLightToSceneRoot( lightPoint );
    
    doUpdateDataStream = true;
    return lightComponent;
}
