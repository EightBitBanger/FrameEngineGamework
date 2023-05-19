#include "engine.h"

ColorPreset       Colors;
RandomGen         Random;
Logger            Log;
Timer             PhysicsTime;
Timer             Time;

ResourceManager   Resources;
PhysicsSystem     Physics;
RenderSystem      Renderer;
AudioSystem       Audio;
ScriptSystem      Scripting;
InputSystem       Input;

ApplicationLayer      Application;
EngineSystemManager   Engine;



EngineSystemManager::EngineSystemManager(void) {
    
    // Preallocate some buffer space
    mGameObjectActive.reserve(512);
    
    return;
}

GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = mGameObject.Create();
    AddGameObjectToActiveList(newGameObject);
    return newGameObject;
}

void EngineSystemManager::DestroyGameObject(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    RemoveGameObjectFromActiveList(gameObjectPtr);
    
    // Remove all components
    for (unsigned int i=0; i < gameObjectPtr->GetComponentCount(); i++) {
        
        Component* componentPtr = gameObjectPtr->GetComponent(i);
        DestroyComponent(componentPtr);
        continue;
    }
    
    mGameObject.Destroy(gameObjectPtr);
    return;
}

GameObject* EngineSystemManager::CreateCameraController(float x, float y, float z) {
    
    GameObject* cameraController = CreateGameObject();
    cameraController->name = "camera";
    
    // Add a camera component
    Component* cameraComponent = CreateComponent(ComponentType::Camera);
    Camera* cameraMain = (Camera*)cameraComponent->GetComponent();
    Renderer.cameraMain = cameraMain;
    cameraMain->EnableMouseLook();
    cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    // Add a rigid body component
    Component* rigidBodyComponent = CreateComponent(ComponentType::RigidBody);
    rp3d::RigidBody* rigidBody = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
    rigidBody->setLinearDamping(4);
    rigidBody->enableGravity(false);
    
    rp3d::Vector3 position(x, y, z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    
    rp3d::Transform bodyTransform(position, quat);
    rigidBody->setTransform(bodyTransform);
    
    // Add a scripting component
    Component* scriptComponent = CreateComponent(ComponentType::Script);
    Script* script = (Script*)scriptComponent->GetComponent();
    script->gameObject = cameraController;
    script->isActive = true;
    
    cameraController->AddComponent(cameraComponent);
    cameraController->AddComponent(rigidBodyComponent);
    cameraController->AddComponent(scriptComponent);
    
    return cameraController;
}

Component* EngineSystemManager::CreateComponentEntityRenderer(Mesh* meshPtr, Material* materialPtr) {
    Component* newComponent = CreateComponent(ComponentType::Renderer);
    Entity* entityRenderer = (Entity*)newComponent->GetComponent();
    entityRenderer->AttachMesh(meshPtr);
    entityRenderer->AttachMaterial(materialPtr);
    
    
    return newComponent;
}

Entity* EngineSystemManager::CreateEntityRenderer(Mesh* meshPtr, Material* materialPtr) {
    Entity* entityPtr = Renderer.CreateEntity();
    entityPtr->AttachMesh( meshPtr );
    entityPtr->AttachMaterial( materialPtr );
    mSceneMain->AddEntityToSceneRoot(entityPtr);
    return entityPtr;
}

void EngineSystemManager::DestroyEntityRenderer(Entity* entityPtr) {
    assert(entityPtr != nullptr);
    mSceneMain->RemoveEntityFromSceneRoot(entityPtr);
    Renderer.DestroyEntity(entityPtr);
    return;
}

void EngineSystemManager::AddGameObjectToActiveList(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    mGameObjectActive.push_back( gameObjectPtr );
    return;
}

bool EngineSystemManager::RemoveGameObjectFromActiveList(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    for (std::vector<GameObject*>::iterator it = mGameObjectActive.begin(); it != mGameObjectActive.end(); ++it) {
        GameObject* thisGameObjectPtr = *it;
        if (gameObjectPtr == thisGameObjectPtr) {
            mGameObjectActive.erase(it);
            return true;
        }
    }
    return false;
}

GameObject* EngineSystemManager::GetGameObject(unsigned int index) {
    if (index <= mGameObjectActive.size()) 
        return mGameObjectActive[index];
    return nullptr;
}

unsigned int EngineSystemManager::GetGameObjectCount(void) {
    return mGameObjectActive.size();
}

void EngineSystemManager::Initiate() {
    mSceneMain = Renderer.CreateScene();
    Renderer.AddToRenderQueue(mSceneMain);
    return;
}

Component* EngineSystemManager::CreateComponent(ComponentType type) {
    void* component_object = nullptr;
    
    // Attach a component of a defined type
    switch (type) {
        
        case ComponentType::Renderer:
            component_object = (void*)this->CreateEntityRenderer(nullptr, nullptr);
            break;
            
        case ComponentType::RigidBody: {
            component_object = (void*)Physics.CreateRigidBody();
            break;
        }
        case ComponentType::Script:
            component_object = (void*)Scripting.CreateScript();
            break;
            
        case ComponentType::Camera:
            component_object = (void*)Renderer.CreateCamera();
            break;
        
        case ComponentType::Light: {
            Light* lightPtr = Renderer.CreateLight();
            mSceneMain->AddLightToSceneRoot(lightPtr);
            component_object = (void*)lightPtr;
            break;
        }
        
        default:
            break;
    }
    
    Component* newComponent = mComponents.Create();
    newComponent->SetComponent(type, component_object);
    return newComponent;
}

void EngineSystemManager::DestroyComponent(Component* componentPtr) {
    assert(componentPtr != nullptr);
    
    Entity* componentEntityRenderer;
    rp3d::RigidBody* componentRigidBody;
    Script* componentScript;
    Camera* componentCamera;
    Light* componentLight;
    
    ComponentType componentType = componentPtr->GetType();
    
    // Destroy attached component type
    switch (componentType) {
        
        case ComponentType::Renderer:
            componentEntityRenderer = (Entity*)componentPtr->GetComponent();
            DestroyEntityRenderer(componentEntityRenderer);
            break;
          
        case ComponentType::RigidBody:
            componentRigidBody = (rp3d::RigidBody*)componentPtr->GetComponent();
            Physics.DestroyRigidBody(componentRigidBody);
            break;
            
        case ComponentType::Script: 
            componentScript = (Script*)componentPtr->GetComponent();
            Scripting.DestroyScript(componentScript);
            break;
            
        case ComponentType::Camera: 
            componentCamera = (Camera*)componentPtr->GetComponent();
            Renderer.DestroyCamera(componentCamera);
            break;
        
        case ComponentType::Light: {
            componentLight = (Light*)componentPtr->GetComponent();
            mSceneMain->RemoveLightFromSceneRoot(componentLight);
            Renderer.DestroyLight(componentLight);
            break;
        }
        
        default:
            break;
    }
    mComponents.Destroy(componentPtr);
    return;
}

void EngineSystemManager::Update(void) {
    
    // Run through the game objects
    for (int i=0; i < mGameObject.Size(); i++ ) {
        
        GameObject* objectPtr = mGameObject[i];
        
        if (!objectPtr->isActive) 
            continue;
        
        // Component references
        Camera*           componentCamera         = objectPtr->GetCachedCamera();
        rp3d::RigidBody*  componentRigidBody      = objectPtr->GetCachedRigidBody();
        Entity*           componentEntityRenderer = objectPtr->GetCachedEntity();
        
        //
        // Nothing to update, no attached rigid body
        
        if (componentRigidBody == nullptr) {
            
            //
            // Update the entity`s transform matrix from the entity`s position and rotation
            
            if (componentEntityRenderer != nullptr) {
                
                glm::mat4 modleMatrix = Renderer.CalculateModelMatrix(componentEntityRenderer->transform);
                
                componentEntityRenderer->transform.matrix = modleMatrix;
            }
            
            continue;
        }
        
        
        rp3d::Transform bodyTransform = componentRigidBody->getTransform();
        
        //
        // Sync the position of the entity renderer
        //
        
        if (componentEntityRenderer != nullptr) {
            bodyTransform.getOpenGLMatrix(&componentEntityRenderer->transform.matrix[0][0]);
            
            // Translation
            rp3d::Vector3 bodyPos = bodyTransform.getPosition();
            componentEntityRenderer->transform.position.x = bodyPos.x;
            componentEntityRenderer->transform.position.y = bodyPos.y;
            componentEntityRenderer->transform.position.z = bodyPos.z;
            
            // Orientation
            rp3d::Quaternion quaterion = bodyTransform.getOrientation();
            componentEntityRenderer->transform.rotation.x = quaterion.x;
            componentEntityRenderer->transform.rotation.y = quaterion.y;
            componentEntityRenderer->transform.rotation.z = quaterion.z;
            componentEntityRenderer->transform.rotation.w = quaterion.w;
            
        }
        
        //
        // Sync the position of the camera
        //
        
        if (componentCamera != nullptr) {
            bodyTransform.getOpenGLMatrix(&componentCamera->transform.matrix[0][0]);
            
            // Translation
            rp3d::Vector3 bodyPos = bodyTransform.getPosition();
            componentCamera->transform.position.x = bodyPos.x;
            componentCamera->transform.position.y = bodyPos.y;
            componentCamera->transform.position.z = bodyPos.z;
            
            // Set the orientation if NOT mouse looking
            if (!componentCamera->useMouseLook) {
                rp3d::Quaternion quaterion = bodyTransform.getOrientation();
                componentCamera->transform.rotation.x = quaterion.x;
                componentCamera->transform.rotation.y = quaterion.y;
                componentCamera->transform.rotation.z = quaterion.z;
                componentCamera->transform.rotation.w = quaterion.w;
            }
            
        }
        
        continue;
    }
    
    return;
}

void EngineSystemManager::Shutdown(void) {
    
    while (GetGameObjectCount() > 0) {
        DestroyGameObject( GetGameObject(0) );
    }
    
    assert(GetGameObjectCount() == 0);
    assert(mComponents.Size() == 0);
    
    return;
}
