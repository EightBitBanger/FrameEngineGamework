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
    
    // Destroy rigid body
    Entity* entityRenderer = gameObjectPtr->GetAttachedEntity();
    if (entityRenderer != nullptr) 
        DestroyEntityRenderer(entityRenderer);
    
    // Destroy entity renderer
    rp3d::RigidBody* rigidBody = gameObjectPtr->GetAttachedRidigBody();
    if (rigidBody != nullptr) 
        DestroyRigidBody(rigidBody);
    
    gameObject.Destroy(gameObjectPtr);
    return;
}






GameObject* EngineSystemManager::CreateCameraController(float x, float y, float z) {
    
    GameObject* cameraController = CreateGameObject();
    cameraController->name = "camera";
    
    // Basic camera
    Renderer.cameraMain = Renderer.CreateCamera();
    cameraController->AttachCamera(Renderer.cameraMain);
    Renderer.cameraMain->EnableMouseLook();
    Renderer.cameraMain->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    // Physical movement
    rp3d::RigidBody* cameraBody = CreateRigidBody(x, y, z);
    cameraController->AttachRidigBody(cameraBody);
    cameraController->SetLinearDamping(3);
    cameraController->EnableGravity(false);
    
    // Default camera movement
    Script* scriptPtr = Scripting.CreateScript();
    cameraController->AttachScript(scriptPtr);
    
    return cameraController;
}

void EngineSystemManager::DestroyCameraController(GameObject* cameraControllerPtr) {
    
    Camera* attachedCamera = cameraControllerPtr->GetAttachedCamera();
    if (attachedCamera != nullptr) {
        if (Renderer.cameraMain == attachedCamera) {
            Renderer.cameraMain = nullptr;
        }
        cameraControllerPtr->DetachCamera();
        Renderer.DestroyCamera(attachedCamera);
    }
    
    Script* attachedScript = cameraControllerPtr->GetAttachedScript();
    if (attachedScript != nullptr) {
        cameraControllerPtr->DetachScript();
        Scripting.DestroyScript(attachedScript);
    }
    
    rp3d::RigidBody* attachedBody = cameraControllerPtr->GetAttachedRidigBody();
    if (attachedBody != nullptr) {
        cameraControllerPtr->DetachRidigBody();
        DestroyRigidBody(attachedBody);
    }
    
    DestroyGameObject(cameraControllerPtr);
}


Entity* EngineSystemManager::CreateEntityRenderer(Mesh* meshPtr, Material* materialPtr) {
    Entity* entityPtr = Renderer.CreateEntity();
    
    assert(meshPtr != nullptr);
    assert(materialPtr != nullptr);
    
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


rp3d::RigidBody* EngineSystemManager::CreateRigidBody(float x, float y, float z) {
    rp3d::RigidBody* rigidBody = Physics.CreateRigidBody(x, y, z);
    
    return rigidBody;
}

void EngineSystemManager::DestroyRigidBody(rp3d::RigidBody* rigidBodyPtr) {
    assert(rigidBodyPtr != nullptr);
    Physics.DestroyRigidBody(rigidBodyPtr);
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
    assert(index <= gameObjectActive.size());
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


void EngineSystemManager::Update(void) {
    
    for (int i=0; i < gameObject.Size()-1; i++ ) {
        GameObject* objectPtr = gameObject[i];
        
        if (!objectPtr->isActive) 
            continue;
        
        // Sync with the rigid body
        rp3d::RigidBody* rigidBodyPtr = objectPtr->GetAttachedRidigBody();
        if (rigidBodyPtr == nullptr) 
            continue;
        
        Entity* entitytPtr = objectPtr->GetAttachedEntity();
        if (entitytPtr != nullptr) {
            rp3d::Transform bodyTransform = rigidBodyPtr->getTransform();
            bodyTransform.getOpenGLMatrix(&entitytPtr->transform.matrix[0][0]);
            
            // Translation
            rp3d::Vector3 bodyPos = bodyTransform.getPosition();
            entitytPtr->transform.position.x = bodyPos.x;
            entitytPtr->transform.position.y = bodyPos.y;
            entitytPtr->transform.position.z = bodyPos.z;
            
            // Orientation
            rp3d::Quaternion quaterion = bodyTransform.getOrientation();
            entitytPtr->transform.rotation.x = quaterion.x;
            entitytPtr->transform.rotation.y = quaterion.y;
            entitytPtr->transform.rotation.z = quaterion.z;
            entitytPtr->transform.rotation.w = quaterion.w;
            
        }
        
        Camera* CameraPtr = objectPtr->GetAttachedCamera();
        if (CameraPtr != nullptr) {
            rp3d::Transform bodyTransform = rigidBodyPtr->getTransform();
            rigidBodyPtr->getTransform().getOpenGLMatrix(&CameraPtr->transform.matrix[0][0]);
            
            // Translation
            rp3d::Vector3 bodyPos = bodyTransform.getPosition();
            CameraPtr->transform.position.x = bodyPos.x;
            CameraPtr->transform.position.y = bodyPos.y;
            CameraPtr->transform.position.z = bodyPos.z;
            
            // Orientation
            if (!CameraPtr->useMouseLook) {
                rp3d::Quaternion quaterion = bodyTransform.getOrientation();
                CameraPtr->transform.rotation.x = quaterion.x;
                CameraPtr->transform.rotation.y = quaterion.y;
                CameraPtr->transform.rotation.z = quaterion.z;
                CameraPtr->transform.rotation.w = quaterion.w;
            }
            
        }
        
        continue;
    }
    return;
}




