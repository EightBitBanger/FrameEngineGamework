#include <GameEngineFramework/Engine/Engine.h>

ENGINE_API extern EngineComponents  Components;
ENGINE_API extern ColorPreset       Colors;
ENGINE_API extern NumberGeneration  Random;
ENGINE_API extern StringType        String;
ENGINE_API extern Logger            Log;
ENGINE_API extern Timer             PhysicsTime;
ENGINE_API extern Timer             Time;

ENGINE_API extern Serialization     Serializer;
ENGINE_API extern ResourceManager   Resources;
ENGINE_API extern ScriptSystem      Scripting;
ENGINE_API extern RenderSystem      Renderer;
ENGINE_API extern PhysicsSystem     Physics;
ENGINE_API extern AudioSystem       Audio;
ENGINE_API extern InputSystem       Input;
ENGINE_API extern MathCore          Math;
ENGINE_API extern ActorSystem       AI;

ENGINE_API extern PlatformLayer     Platform;



Component* EngineSystemManager::CreateComponent(ComponentType type) {
    void* component_object = nullptr;
    
    switch (type) {
        
        case Components.Transform:       component_object = (void*)mTransforms.Create(); break;
        case Components.MeshRenderer:    component_object = (void*)Renderer.CreateMeshRenderer(); break;
        case Components.Camera:          component_object = (void*)Renderer.CreateCamera(); break;
        case Components.Light:           component_object = (void*)Renderer.CreateLight(); break;
        case Components.Script:          component_object = (void*)Scripting.CreateScript(); break;
        case Components.RigidBody:       component_object = (void*)Physics.CreateRigidBody(); break;
        case Components.Actor:           component_object = (void*)AI.CreateActor(); break;
        case Components.Text:            component_object = (void*)mTextObjects.Create(); break;
        case Components.Panel:           component_object = (void*)mPanelObjects.Create(); break;
        
        default:
            return nullptr;
    }
    
    Component* newComponent = mComponents.Create();
    newComponent->SetComponent(type, component_object);
    
    return newComponent;
}

bool EngineSystemManager::DestroyComponent(Component* componentPtr) {
    
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
        
        default: break;
    }
    
    return true;
}

unsigned int EngineSystemManager::GetNumberOfComponents(void) {
    return mComponents.Size();
}

Component* EngineSystemManager::CreateComponentMeshRenderer(Mesh* meshPtr, Material* materialPtr) {
    Component* rendererComponent = CreateComponent(Components.MeshRenderer);
    MeshRenderer* meshRenderer = (MeshRenderer*)rendererComponent->mObject;
    
    meshRenderer->mesh = meshPtr;
    meshRenderer->material = materialPtr;
    
    return rendererComponent;
}

Component* EngineSystemManager::CreateComponentLight(glm::vec3 position) {
    Component* lightComponent = CreateComponent(Components.Light);
    Light* lightPoint = (Light*)lightComponent->mObject;
    
    lightPoint->position = position;
    
    return lightComponent;
}
