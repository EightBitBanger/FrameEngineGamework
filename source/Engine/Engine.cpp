#include "engine.h"

ColorPreset       Colors;
RandomGen         Random;
Logger            Log;
Timer             PhysicsTime;
Timer             Time;

ResourceManager   Resources;
PhysicsSystem     Physics;
RenderSystem      Renderer;
ScriptSystem      Scripting;
InputSystem       Input;

ApplicationLayer      Application;
EngineSystemManager   Engine;




EngineSystemManager::EngineSystemManager(void) {
    
    // Preallocate some buffer space
    gameObjectActive.reserve(512);
    rigidBodyFreeList.reserve(512);
    
    return;
}


GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = gameObject.Create();
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
    
    gameObject.Destroy(gameObjectPtr);
    return;
}






GameObject* EngineSystemManager::CreateCameraController(float x, float y, float z) {
    
    GameObject* cameraController = CreateGameObject();
    cameraController->name = "camera";
    
    // Camera component
    Component* cameraComponent = CreateComponent(COMPONENT_TYPE_CAMERA);
    Camera* cameraMain = (Camera*)cameraComponent->GetComponent();
    Renderer.cameraMain = cameraMain;
    cameraMain->EnableMouseLook();
    cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    // Rigid body component
    Component* rigidBodyComponent = CreateComponent(COMPONENT_TYPE_RIGIDBODY);
    rp3d::RigidBody* rigidBody = (rp3d::RigidBody*)rigidBodyComponent->GetComponent();
    rigidBody->setLinearDamping(4);
    rigidBody->enableGravity(false);
    
    rp3d::Vector3 position(x, y, z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    
    rp3d::Transform bodyTransform(position, quat);
    rigidBody->setTransform(bodyTransform);
    
    // Script component
    Component* scriptComponent = CreateComponent(COMPONENT_TYPE_SCRIPT);
    Script* script = (Script*)scriptComponent->GetComponent();
    script->gameObject = cameraController;
    script->isActive = true;
    
    cameraController->AddComponent(cameraComponent);
    cameraController->AddComponent(rigidBodyComponent);
    cameraController->AddComponent(scriptComponent);
    
    return cameraController;
}


Entity* EngineSystemManager::CreateEntityRenderer(Mesh* meshPtr, Material* materialPtr) {
    Entity* entityPtr = Renderer.CreateEntity();
    entityPtr->AttachMesh( meshPtr );
    entityPtr->AttachMaterial( materialPtr );
    sceneMain->AddToSceneRoot(entityPtr);
    return entityPtr;
}

void EngineSystemManager::DestroyEntityRenderer(Entity* entityPtr) {
    assert(entityPtr != nullptr);
    sceneMain->RemoveFromSceneRoot(entityPtr);
    Renderer.DestroyEntity(entityPtr);
    return;
}




void EngineSystemManager::AddGameObjectToActiveList(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    gameObjectActive.push_back( gameObjectPtr );
    return;
}

bool EngineSystemManager::RemoveGameObjectFromActiveList(GameObject* gameObjectPtr) {
    assert(gameObjectPtr != nullptr);
    for (std::vector<GameObject*>::iterator it = gameObjectActive.begin(); it != gameObjectActive.end(); ++it) {
        GameObject* thisGameObjectPtr = *it;
        if (gameObjectPtr == thisGameObjectPtr) {
            gameObjectActive.erase(it);
            return true;
        }
    }
    return false;
}

GameObject* EngineSystemManager::GetGameObject(unsigned int index) {
    assert(index < gameObjectActive.size());
    return gameObjectActive[index];
}

unsigned int EngineSystemManager::GetGameObjectCount(void) {
    return gameObjectActive.size();
}

void EngineSystemManager::Initiate() {
    sceneMain = Renderer.CreateScene();
    Renderer.AddToRenderQueue(sceneMain);
    return;
}



Component* EngineSystemManager::CreateComponent(unsigned int component_type) {
    void* component_object = nullptr;
    
    switch (component_type) {
        
        case COMPONENT_TYPE_RENDERER:
            component_object = (void*)this->CreateEntityRenderer(nullptr, nullptr);
            break;
            
        case COMPONENT_TYPE_RIGIDBODY: {
            if (rigidBodyFreeList.size() > 0) {
                component_object = (void*)RemoveRigidBodyFromFreeList();
                break;
            }
            }
            component_object = (void*)Physics.CreateRigidBody();
            break;
            
        case COMPONENT_TYPE_SCRIPT:
            component_object = (void*)Scripting.CreateScript();
            break;
            
        case COMPONENT_TYPE_CAMERA:
            component_object = (void*)Renderer.CreateCamera();
            break;
        
    }
    
    Component* newComponent = components.Create();
    newComponent->SetComponent(component_type, component_object);
    return newComponent;
}

void EngineSystemManager::DestroyComponent(Component* componentPtr) {
    assert(componentPtr != nullptr);
    
    // Destroy attached component object
    Entity* componentEntityRenderer;
    rp3d::RigidBody* componentRigidBody;
    Script* componentScript;
    Camera* componentCamera;
    
    unsigned int componentType = componentPtr->GetType();
    
    switch (componentType) {
        
        case COMPONENT_TYPE_RENDERER:
            componentEntityRenderer = (Entity*)componentPtr->GetComponent();
            DestroyEntityRenderer(componentEntityRenderer);
            break;
            
        case COMPONENT_TYPE_RIGIDBODY:
            componentRigidBody = (rp3d::RigidBody*)componentPtr->GetComponent();
            //Physics.DestroyRigidBody(componentRigidBody);
            AddRigidBodyToFreeList(componentRigidBody);
            break;
            
        case COMPONENT_TYPE_SCRIPT: 
            componentScript = (Script*)componentPtr->GetComponent();
            Scripting.DestroyScript(componentScript);
            break;
            
        case COMPONENT_TYPE_CAMERA: 
            componentCamera = (Camera*)componentPtr->GetComponent();
            Renderer.DestroyCamera(componentCamera);
            break;
            
    }
    components.Destroy(componentPtr);
    return;
}




void EngineSystemManager::AddRigidBodyToFreeList(rp3d::RigidBody* rigidBodyPtr) {
    rigidBodyFreeList.push_back(rigidBodyPtr);
    rigidBodyPtr->setIsActive(false);
    return;
}

rp3d::RigidBody* EngineSystemManager::RemoveRigidBodyFromFreeList(void) {
    rp3d::RigidBody* bodyPtr = nullptr;
    for (std::vector<rp3d::RigidBody*>::iterator it = rigidBodyFreeList.begin(); it != rigidBodyFreeList.end(); ++it) {
        rp3d::RigidBody* thisBodyPtr = *it;
        rigidBodyFreeList.erase(it);
        bodyPtr = thisBodyPtr;
        bodyPtr->setIsActive(true);
        break;
    }
    if (bodyPtr == nullptr) {
        return Physics.CreateRigidBody(0, 0, 0);
    }
    
    return bodyPtr;
}



void EngineSystemManager::Update(void) {
    
    // Run through the game objects
    for (int i=0; i < gameObject.Size(); i++ ) {
        
        // Component references
        Camera*           componentCamera = nullptr;
        //Script*           componentScript = nullptr;
        rp3d::RigidBody*  componentRigidBody = nullptr;
        Entity*           componentEntityRenderer = nullptr;
        
        GameObject* objectPtr = gameObject[i];
        
        if (!objectPtr->isActive) 
            continue;
        
        // Run through the components
        for (unsigned int c=0; c < objectPtr->GetComponentCount(); c++) {
            
            Component* componentPtr = objectPtr->GetComponent(c);
            
            switch (componentPtr->GetType()) {
                
                case COMPONENT_TYPE_RENDERER:
                    componentEntityRenderer = (Entity*)componentPtr->GetComponent();
                    break;
                    
                case COMPONENT_TYPE_RIGIDBODY:
                    componentRigidBody = (rp3d::RigidBody*)componentPtr->GetComponent();
                    break;
                    
                //case COMPONENT_TYPE_SCRIPT: 
                //    componentScript = (Script*)componentPtr->GetComponent();
                //    break;
                    
                case COMPONENT_TYPE_CAMERA: 
                    componentCamera = (Camera*)componentPtr->GetComponent();
                    break;
                    
            }
            
            continue;
        }
        
        // No rigid body just a renderer, update the matrix from position and rotation
        if (componentEntityRenderer != nullptr) {
            glm::mat4 modleMatrix = Renderer.CalculateModelMatrix(componentEntityRenderer->transform);
            componentEntityRenderer->transform.matrix = modleMatrix;
        }
        
        // Nothing further to update
        if (componentRigidBody == nullptr) 
            continue;
        
        // Sync with the rigid body
        rp3d::Transform bodyTransform = componentRigidBody->getTransform();
        
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
        
        if (componentCamera != nullptr) {
            bodyTransform.getOpenGLMatrix(&componentCamera->transform.matrix[0][0]);
            
            // Translation
            rp3d::Vector3 bodyPos = bodyTransform.getPosition();
            componentCamera->transform.position.x = bodyPos.x;
            componentCamera->transform.position.y = bodyPos.y;
            componentCamera->transform.position.z = bodyPos.z;
            
            // Orientation if NOT mouse looking
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




