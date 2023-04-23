#include "../Renderer/RenderSystem.h"
extern RenderSystem Renderer;
extern ColorPreset  Colors;
#include "../Physics/PhysicsSystem.h"
extern PhysicsSystem Physics;
#include "../Resources/ResourceManager.h"
extern ResourceManager Resources;

#include "EngineMain.h"


void EngineMainAPI::Initiate(void) {
    
    sceneMain = Renderer.CreateScene();
    Renderer.AddToRenderQueue(sceneMain);
    
    return;
}

Entity* EngineMainAPI::CreateGameObject(float x, float y, float z, std::string meshName, std::string materialName, std::string colliderName) {
    Entity* newEntity = Renderer.CreateEntity();
    sceneMain->AddToSceneRoot(newEntity);
    if (meshName != "") {
        newEntity->mesh = Resources.CreateMeshFromTag(meshName);
        newEntity->mesh->SetDefaultAttributes();
    }
    if (materialName != "") {
        newEntity->mesh->shader = Renderer.defaultShader;
        newEntity->material = Resources.CreateMaterialFromTag(materialName);
        newEntity->material->color = Colors.white;
    }
    if (colliderName != "") {
        newEntity->rigidBody = Physics.CreateRigidBody(x, y, z);
        newEntity->rigidBody->setMass(0.1);
        newEntity->rigidBody->setAngularDamping(10);
        rp3d::BoxShape* collider = Resources.GetColliderFromTag(colliderName);
        newEntity->AddCollider(collider, 0, 0, 0);
    }
    return newEntity;
}

void EngineMainAPI::DestroyGameObject(Entity* entityPtr) {
    assert(entityPtr != nullptr);
    if (entityPtr->mesh != nullptr) Renderer.DestroyMesh(entityPtr->mesh);
    if (entityPtr->material != nullptr) Renderer.DestroyMaterial(entityPtr->material);
    Renderer.DestroyEntity(entityPtr);
    return;
}



