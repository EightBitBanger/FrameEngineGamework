#include <GameEngineFramework/Engine/Engine.h>
#include <GameEngineFramework/Engine/EngineSystems.h>


GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = mGameObjects.Create();
    mGameObjectActive.push_back(newGameObject);
    newGameObject->AddComponent( CreateComponent<Transform>() );
    return newGameObject;
}

bool EngineSystemManager::DestroyGameObject(GameObject* gameObjectPtr) {
    gameObjectPtr->isGarbage = true;
    return false;
}

unsigned int EngineSystemManager::GetNumberOfGameObjects(void) {
    return mGameObjects.Size();
}

GameObject* EngineSystemManager::CreateCameraController(glm::vec3 position) {
    
    GameObject* cameraController = CreateGameObject();
    cameraController->name = "camera";
    cameraController->mTransformCache->position = position;
    
    // Add a camera component
    Component* cameraComponent = CreateComponent<Camera>();
    
    // Add a rigid body component
    Component* rigidBodyComponent = CreateComponent<RigidBody>();
    rp3d::RigidBody* rigidBody = (RigidBody*)rigidBodyComponent->mObject;
    
    rp3d::Vector3 bodyPosition(position.x, position.y, position.z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    
    rp3d::Transform bodyTransform(bodyPosition, quat);
    rigidBody->setTransform(bodyTransform);
    
    rigidBody->enableGravity(false);
    
    cameraController->AddComponent(cameraComponent);
    cameraController->AddComponent(rigidBodyComponent);
    
    cameraController->SetAngularAxisLockFactor(0, 0, 0);
    
    cameraController->SetLinearDamping(1);
    cameraController->SetAngularDamping(1);
    cameraController->SetMass(10);
    cameraController->DisableGravity();
    
    return cameraController;
}

GameObject* EngineSystemManager::CreateSky(std::string meshTagName, Color colorLow, Color colorHigh, float biasMul) {
    
    Mesh* skyMesh = Resources.CreateMeshFromTag(meshTagName);
    if (skyMesh == nullptr) return nullptr;
    
    Color skyHigh = colorHigh;
    Color skyLow  = colorLow;
    
    // Set the base range
    skyHigh += Colors.MakeGrayScale(0.997f);
    skyLow  += Colors.MakeGrayScale(0.799f);
    
    // Tweak the values
    skyHigh *= Colors.MakeGrayScale(0.961f);
    skyLow  *= Colors.MakeGrayScale(1.01f);
    
    // Uniform scale down
    skyHigh *= Colors.MakeGrayScale(0.24f);
    skyLow  *= Colors.MakeGrayScale(0.24f);
    
    Material* skyMaterial = Renderer.CreateMaterial();
    
    skyMaterial->ambient = Colors.MakeGrayScale(0.0f);
    skyMaterial->diffuse = Colors.MakeGrayScale(0.0f);
    
    skyMaterial->shader  = shaders.sky;
    
    skyMaterial->DisableDepthTest();
    skyMaterial->DisableShadowVolumePass();
    
    for (unsigned int i=0; i < skyMesh->GetNumberOfVertices(); i++) {
        Vertex vertex = skyMesh->GetVertex(i);
        
        if (vertex.y > 0) {
            vertex.r = Math.Lerp(skyHigh.r, skyLow.r, vertex.y * biasMul);
            vertex.g = Math.Lerp(skyHigh.g, skyLow.g, vertex.y * biasMul);
            vertex.b = Math.Lerp(skyHigh.b, skyLow.b, vertex.y * biasMul);
        } else {
            vertex.r = skyLow.r;
            vertex.g = skyLow.g;
            vertex.b = skyLow.b;
        }
        
        skyMesh->SetVertex(i, vertex);
    }
    skyMesh->Load();
    
    GameObject* skyObject = CreateGameObject();
    skyObject->name = "sky";
    
    Component* skyRendererComponent = CreateComponent<MeshRenderer>();
    skyObject->AddComponent( skyRendererComponent );
    MeshRenderer* skyRenderer = skyObject->GetComponent<MeshRenderer>();
    skyRenderer->mesh = skyMesh;
    skyRenderer->material = skyMaterial;
    
    skyObject->renderDistance = -1;
    
    skyObject->mTransformCache->SetScale(10000, 2000, 10000);
    
    return skyObject;
}

