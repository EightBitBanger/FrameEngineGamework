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



EngineSystemManager::EngineSystemManager(void) : 
    
    mSceneMain(nullptr) 
{
    // Preallocate some buffer space
    mGameObjectActive.reserve(64);
    
    return;
}

GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = mGameObjects.Create();
    // Add object to the active objects list
    mGameObjectActive.push_back(newGameObject);
    return newGameObject;
}

bool EngineSystemManager::DestroyGameObject(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    
    // Remove the game object from the active list
    for (std::vector<GameObject*>::iterator it = mGameObjectActive.begin(); it != mGameObjectActive.end(); ++it) {
        GameObject* thisGameObjectPtr = *it;
        if (gameObjectPtr == thisGameObjectPtr) {
            mGameObjectActive.erase(it);
            break;
        }
    }
    
    // Remove all components
    for (unsigned int i=0; i < gameObjectPtr->GetComponentCount(); i++) {
        
        Component* componentPtr = gameObjectPtr->GetComponent(i);
        DestroyComponent(componentPtr);
        continue;
    }
    
    mGameObjects.Destroy(gameObjectPtr);
    return true;
}

GameObject* EngineSystemManager::CreateCameraController(glm::vec3 position) {
    
    GameObject* cameraController = CreateGameObject();
    cameraController->name = "camera";
    cameraController->transform.position = position;
    
    // Add a camera component
    Component* cameraComponent = CreateComponent(ComponentType::Camera);
    Camera* cameraMain = (Camera*)cameraComponent->GetComponent();
    Renderer.cameraMain = cameraMain;
    cameraMain->EnableMouseLook();
    cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    // Add a rigid body component
    Component* rigidBodyComponent = CreateComponent(ComponentType::RigidBody);
    rp3d::RigidBody* rigidBody = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
    
    rp3d::Vector3 bodyPosition(position.x, position.y, position.z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    
    rp3d::Transform bodyTransform(bodyPosition, quat);
    rigidBody->setTransform(bodyTransform);
    
    // Add a scripting component
    Component* scriptComponent = CreateComponent(ComponentType::Script);
    Script* script = (Script*)scriptComponent->GetComponent();
    script->gameObject = cameraController;
    script->isActive = true;
    
    cameraController->AddComponent(cameraComponent);
    cameraController->AddComponent(rigidBodyComponent);
    cameraController->AddComponent(scriptComponent);
    
    cameraController->EnableGravity(false);
    cameraController->SetAngularAxisLockFactor(0, 0, 0);
    
    return cameraController;
}

Component* EngineSystemManager::CreateComponentMeshRenderer(Mesh* meshPtr, Material* materialPtr) {
    Component* newComponent = CreateComponent(ComponentType::Renderer);
    Entity* entityRenderer = (Entity*)newComponent->GetComponent();
    entityRenderer->AttachMesh(meshPtr);
    entityRenderer->AttachMaterial(materialPtr);
    return newComponent;
}

Component* EngineSystemManager::CreateComponentLight(glm::vec3 position) {
    Component* newComponent = CreateComponent(ComponentType::Light);
    Light* lightPoint = (Light*)newComponent->GetComponent();
    lightPoint->transform.position = position;
    return newComponent;
}

GameObject* EngineSystemManager::GetGameObject(unsigned int index) {
    if (index < mGameObjectActive.size()) 
        return mGameObjectActive[index];
    return nullptr;
}

unsigned int EngineSystemManager::GetGameObjectCount(void) {
    return mGameObjects.Size();
}

unsigned int EngineSystemManager::GetComponentCount(void) {
    return mComponents.Size();
}

void EngineSystemManager::Initiate() {
    mSceneMain = Renderer.CreateScene();
    Renderer.AddSceneToRenderQueue(mSceneMain);
    return;
}

Component* EngineSystemManager::CreateComponent(ComponentType type) {
    void* component_object = nullptr;
    
    switch (type) {
        
        case ComponentType::Renderer: {
            Entity* entityPtr = Renderer.CreateEntity();
            mSceneMain->AddEntityToSceneRoot(entityPtr);
            component_object = (void*)entityPtr;
            break;
        }
        case ComponentType::RigidBody: {
            component_object = (void*)Physics.CreateRigidBody();
            break;
        }
        case ComponentType::Script: {
            component_object = (void*)Scripting.CreateScript();
            break;
        }
        case ComponentType::Camera: {
            component_object = (void*)Renderer.CreateCamera();
            break;
        }
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

bool EngineSystemManager::DestroyComponent(Component* componentPtr) {
    assert(componentPtr != nullptr);
    
    ComponentType componentType = componentPtr->GetType();
    
    switch (componentType) {
        
        case ComponentType::Renderer: {
            Entity* componentEntityRenderer = (Entity*)componentPtr->GetComponent();
            mSceneMain->RemoveEntityFromSceneRoot(componentEntityRenderer);
            Renderer.DestroyEntity(componentEntityRenderer);
            break;
        }
        case ComponentType::RigidBody: {
            rp3d::RigidBody* componentRigidBody = (rp3d::RigidBody*)componentPtr->GetComponent();
            Physics.DestroyRigidBody(componentRigidBody);
            break;
        }
        case ComponentType::Script: {
            Script* componentScript = (Script*)componentPtr->GetComponent();
            Scripting.DestroyScript(componentScript);
            break;
        }
        case ComponentType::Camera: {
            Camera* componentCamera = (Camera*)componentPtr->GetComponent();
            Renderer.DestroyCamera(componentCamera);
            break;
        }
        case ComponentType::Light: {
            Light* componentLight = (Light*)componentPtr->GetComponent();
            mSceneMain->RemoveLightFromSceneRoot(componentLight);
            Renderer.DestroyLight(componentLight);
            break;
        }
        
        default:
            return false;
    }
    mComponents.Destroy(componentPtr);
    return true;
}

void EngineSystemManager::Update(void) {
    
    // Run through the game objects
    for (int i=0; i < mGameObjects.Size(); i++ ) {
        
        GameObject* objectPtr = mGameObjects[i];
        
        if (!objectPtr->isActive) 
            continue;
        
        // Current transform
        Transform currentTransform;
        currentTransform.position    = objectPtr->transform.position;
        currentTransform.orientation = objectPtr->transform.orientation;
        currentTransform.scale       = objectPtr->transform.scale;
        
        // Calculate parent transforms
        if (objectPtr->parent != nullptr) {
            
            GameObject* parent = objectPtr->parent;
            
            // Roll over the parent matrix transform chain
            for (unsigned int i=0; i < 100; i++) {
                
                currentTransform.position  += parent->transform.position;
                currentTransform.scale     *= parent->transform.scale;
                
                // Sus O_o
                if (i % 2 == 0) {
                    currentTransform.orientation *= glm::inverse(parent->transform.orientation);
                } else {
                    currentTransform.orientation *= parent->transform.orientation;
                }
                
                parent = parent->parent;
                if (parent == nullptr) 
                    break;
            }
            
        }
        
        glm::mat4 translation = glm::translate(glm::mat4(1), currentTransform.position);
        glm::mat4 rotation    = glm::toMat4(currentTransform.orientation);
        glm::mat4 scale       = glm::scale(glm::mat4(1), currentTransform.scale);
        
        currentTransform.matrix = translation * rotation * scale;
        
        
        //
        // Sync with the rigid body
        //
        
        rp3d::RigidBody* componentRigidBody = objectPtr->GetCachedRigidBody();
        if (componentRigidBody != nullptr) {
            
            // Use the rigid body as the source transform
            rp3d::Transform bodyTransform = componentRigidBody->getTransform();
            rp3d::Vector3 bodyPosition = bodyTransform.getPosition();
            rp3d::Quaternion quaterion = bodyTransform.getOrientation();
            
            currentTransform.position.x = bodyPosition.x;
            currentTransform.position.y = bodyPosition.y;
            currentTransform.position.z = bodyPosition.z;
            
            currentTransform.orientation.w = quaterion.w;
            currentTransform.orientation.x = quaterion.x;
            currentTransform.orientation.y = quaterion.y;
            currentTransform.orientation.z = quaterion.z;
            
            // Source matrix
            bodyTransform.getOpenGLMatrix(&currentTransform.matrix[0][0]);
            
            // Update the game object transform
            objectPtr->transform.position    = currentTransform.position;
            objectPtr->transform.orientation = currentTransform.orientation;
            
            // Scale the transform
            currentTransform.matrix = glm::scale(currentTransform.matrix, objectPtr->transform.scale);
        }
        
        Entity* componentEntityRenderer = objectPtr->GetCachedEntity();
        if (componentEntityRenderer != nullptr) {
            componentEntityRenderer->transform.position    = currentTransform.position;
            componentEntityRenderer->transform.orientation = currentTransform.orientation;
            componentEntityRenderer->transform.scale       = currentTransform.scale;
            componentEntityRenderer->transform.matrix      = currentTransform.matrix;
        }
        
        Light* componentLight = objectPtr->GetCachedLight();
        if (componentLight != nullptr) {
            componentLight->transform.position    = currentTransform.position;
            componentLight->transform.orientation = currentTransform.orientation;
        }
        
        Camera* componentCamera = objectPtr->GetCachedCamera();
        if (componentCamera != nullptr) {
            componentCamera->transform.position = currentTransform.position;
            if (!componentCamera->useMouseLook) 
                componentCamera->transform.orientation = currentTransform.orientation;
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
