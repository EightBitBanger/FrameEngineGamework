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



GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = mGameObjects.Create();
    mGameObjectActive.push_back(newGameObject);
    
    newGameObject->AddComponent( CreateComponent<Transform>() );
    
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
    
    // Remove and destroy all components
    for (unsigned int i=0; i < gameObjectPtr->GetComponentCount(); i++) {
        
        Component* componentPtr = gameObjectPtr->GetComponentIndex(i);
        
        DestroyComponent(componentPtr);
        
    }
    
    mGameObjects.Destroy(gameObjectPtr);
    
    return true;
}

unsigned int EngineSystemManager::GetNumberOfGameObjects(void) {
    return mGameObjects.Size();
}

GameObject* EngineSystemManager::CreateCameraController(glm::vec3 position, glm::vec3 scale) {
    
    GameObject* cameraController = CreateGameObject();
    cameraController->name = "camera";
    cameraController->mTransformCache->position = position;
    
    // Add a camera component
    Component* cameraComponent = CreateComponent(Components.Camera);
    Camera* cameraMain = (Camera*)cameraComponent->mObject;
    cameraMain->EnableMouseLook();
    
    SetCursorPos(Renderer.displayCenter.x, Renderer.displayCenter.y);
    
    // Add a rigid body component
    Component* rigidBodyComponent = CreateComponent(Components.RigidBody);
    RigidBody* rigidBody = (RigidBody*)rigidBodyComponent->mObject;
    
    rp3d::Vector3 bodyPosition(position.x, position.y, position.z);
    rp3d::Quaternion quat = rp3d::Quaternion::identity();
    
    rp3d::Transform bodyTransform(bodyPosition, quat);
    rigidBody->setTransform(bodyTransform);
    
    // Add a scripting component
    Component* scriptComponent = CreateComponent(Components.Script);
    Script* script = (Script*)scriptComponent->mObject;
    script->name = "controller";
    script->gameObject = cameraController;
    script->isActive = true;
    
    cameraController->AddComponent(cameraComponent);
    cameraController->AddComponent(rigidBodyComponent);
    cameraController->AddComponent(scriptComponent);
    
    cameraController->SetAngularAxisLockFactor(0, 0, 0);
    cameraController->SetLinearDamping(3);
    cameraController->SetAngularDamping(1);
    cameraController->SetMass(10);
    
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
    skyMesh->UploadToGPU();
    
    GameObject* skyObject = CreateGameObject();
    skyObject->name = "sky";
    skyObject->AddComponent( CreateComponentMeshRenderer(skyMesh, skyMaterial) );
    skyObject->renderDistance = -1;
    
    skyObject->mTransformCache->SetScale(10000, 2000, 10000);
    
    return skyObject;
}

GameObject* EngineSystemManager::CreateAIActor(glm::vec3 position) {
    
    GameObject* newGameObject = CreateGameObject();
    
    newGameObject->renderDistance = 300;
    
    newGameObject->AddComponent( CreateComponent(Components.Actor) );
    newGameObject->AddComponent( CreateComponent(Components.RigidBody) );
    
    newGameObject->GetComponent<Actor>()->mPosition    = position;
    newGameObject->GetComponent<Actor>()->mTargetPoint = position;
    
    newGameObject->SetPosition(position);
    
    // Actor controllers do not use conventional physics gravity
    newGameObject->DisableGravity();
    newGameObject->SetDynamic();
    
    newGameObject->SetLinearDamping(30);
    newGameObject->SetAngularDamping(100);
    
    newGameObject->SetLinearAxisLockFactor(1, 1, 1);
    newGameObject->SetAngularAxisLockFactor(0, 0, 0);
    
    return newGameObject;
}

GameObject* EngineSystemManager::CreateOverlayRenderer(void) {
    GameObject* overlayObject = Create<GameObject>();
    
    overlayObject->renderDistance = -1;
    
    overlayObject->mTransformCache->RotateAxis(-180, Vector3(0, 1, 0));
    overlayObject->mTransformCache->RotateAxis( -90, Vector3(0, 0, 1));
    overlayObject->mTransformCache->inheritParentRotation = false;
    
    Mesh*     overlayMesh     = Create<Mesh>();
    Material* overlayMaterial = Create<Material>();
    
    overlayMaterial->shader = shaders.UI;
    overlayMaterial->ambient = Colors.black;
    
    overlayMaterial->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    overlayMaterial->DisableCulling();
    overlayMaterial->DisableShadowVolumePass();
    
    overlayObject->AddComponent( CreateComponent<MeshRenderer>(overlayMesh, overlayMaterial) );
    
    return overlayObject;
}

GameObject* EngineSystemManager::CreateOverlayTextRenderer(int x, int y, std::string text, unsigned int textSize, Color color, std::string materialTag) {
    
    GameObject* overlayObject = CreateOverlayRenderer();
    
    overlayObject->renderDistance = -1;
    
    overlayObject->AddComponent( CreateComponent<Text>() );
    Text* textElement = overlayObject->GetComponent<Text>();
    
    textElement->text  = text;
    textElement->color = color;
    textElement->size = textSize;
    
    overlayObject->mTransformCache->scale = Vector3(textSize, 1, textSize);
    
    textElement->canvas.x = x;
    textElement->canvas.y = y;
    
    MeshRenderer* overlayRenderer = overlayObject->GetComponent<MeshRenderer>();
    
    // Sprite sheet material
    Destroy<Material>( overlayRenderer->material );
    overlayRenderer->material = Resources.CreateMaterialFromTag( materialTag );
    overlayRenderer->material->ambient  = Color(0.58f, 0, 0);
    overlayRenderer->material->shader = shaders.UI;
    
    overlayRenderer->material->SetBlending(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);
    overlayRenderer->material->EnableBlending();
    
    overlayRenderer->material->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    
    overlayRenderer->material->DisableCulling();
    overlayRenderer->material->DisableShadowVolumePass();
    
    return overlayObject;
}

GameObject* EngineSystemManager::CreateOverlayPanelRenderer(int x, int y, int width, int height, std::string materialTag) {
    
    GameObject* overlayObject = CreateOverlayRenderer();
    
    overlayObject->renderDistance = -1;
    
    overlayObject->AddComponent( CreateComponent<Panel>() );
    Panel* overPanel = overlayObject->GetComponent<Panel>();
    
    overPanel->canvas.x = x;
    overPanel->canvas.y = y;
    
    MeshRenderer* overlayRenderer = overlayObject->GetComponent<MeshRenderer>();
    
    // Sprite base material
    Material* overlayMaterial = overlayRenderer->material;
    Mesh*     overlayMesh     = overlayRenderer->mesh;
    
    Destroy<Material>( overlayMaterial );
    overlayMaterial = Resources.CreateMaterialFromTag( materialTag );
    
    float alphaCutoff = 0;
    
    overlayMaterial->ambient = Color(alphaCutoff, 0, 0);
    overlayMaterial->shader  = shaders.UI;
    
    overlayMaterial->SetBlending(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);
    overlayMaterial->EnableBlending();
    
    overlayMaterial->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    
    overlayMaterial->DisableCulling();
    overlayMaterial->DisableShadowVolumePass();
    
    overlayMesh->AddPlain(0, 0, 0, height, width, Colors.white, 1, 1);
    
    overlayMesh->UploadToGPU();
    
    return overlayObject;
}



