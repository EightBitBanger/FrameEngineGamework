#include "engine.h"

Timer        PhysicsTime;
Timer        Time;
Logger       Log;
RandomGen    Random;
ColorPreset  Colors;

InputSystem       Input;
PhysicsSystem     Physics;
RenderSystem      Renderer;
ScriptSystem      Scripting;
ResourceManager   Resources;

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
    gameObject.Destroy(gameObjectPtr);
    return;
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
    
    for (int i=0; i < gameObject.Size(); i++ ) {
        GameObject* objectPtr = gameObject[i];
        
        
        if (!objectPtr->isActive) 
            continue;
        
        Entity* entitytPtr = objectPtr->GetAttachedEntity();
        if (entitytPtr == nullptr) 
            continue;
        
        rp3d::RigidBody* rigidBodyPtr = objectPtr->GetAttachedRidigBody();
        if (rigidBodyPtr == nullptr) 
            continue;
        
        rigidBodyPtr->getTransform().getOpenGLMatrix(&entitytPtr->transform.modelMatrix[0][0]);
        
        
        continue;
    }
    return;
}










