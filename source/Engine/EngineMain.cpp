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
    } else {
        newEntity->mesh = Renderer.CreateMesh();
    }
    newEntity->mesh->SetDefaultAttributes();
    newEntity->mesh->shader = Renderer.defaultShader;
    if (materialName != "") {
        newEntity->material = Resources.CreateMaterialFromTag(materialName);
        newEntity->material->color = Colors.white;
    } else {
        newEntity->material = Renderer.CreateMaterial();
    }
    if (colliderName != "") {
        newEntity->rigidBody = Physics.CreateRigidBody(x, y, z);
        newEntity->rigidBody->setMass(0.1);
        newEntity->rigidBody->setLinearDamping(0.01);
        newEntity->rigidBody->setAngularDamping(0.3);
        ColliderTag* colliderTag = Resources.FindColliderTag(colliderName);
        rp3d::BoxShape* collider = colliderTag->colliderShape;
        if (colliderTag->isStatic) {
            newEntity->rigidBody->setType(rp3d::BodyType::STATIC);} else {
            newEntity->rigidBody->setType(rp3d::BodyType::DYNAMIC);
        }
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


Camera* EngineMainAPI::CreateCameraController(float x, float y, float z) {
    Camera* newCamera = Renderer.CreateCamera();
    newCamera->transform.position = glm::vec3(x, y, z);
    newCamera->EnableMouseLook();
    newCamera->SetMouseCenter(Renderer.displayCenter.x, Renderer.displayCenter.y);
    return newCamera;
}
























