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

GameObject* EngineSystemManager::CreateAIActor(glm::vec3 position) {
    
    GameObject* newGameObject = CreateGameObject();
    
    Component* actorComponent = CreateComponent<Actor>();
    Component* rigidBodyComponent = CreateComponent<RigidBody>();
    newGameObject->AddComponent( actorComponent );
    newGameObject->AddComponent( rigidBodyComponent );
    
    newGameObject->renderDistance = -1;
    
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
    
    overlayObject->mTransformCache->matrix = glm::mat4(1);
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
    
    Component* component = CreateComponent<MeshRenderer>();
    
    overlayObject->AddComponent( component );
    MeshRenderer* renderer = overlayObject->GetComponent<MeshRenderer>();
    
    renderer->mesh     = overlayMesh;
    renderer->material = overlayMaterial;
    
    return overlayObject;
}

GameObject* EngineSystemManager::CreateOverlayTextRenderer(float x, float y, std::string text, float textSize, Color color, std::string materialTag) {
    
    GameObject* overlayObject = CreateOverlayRenderer();
    
    overlayObject->renderDistance = -1;
    
    overlayObject->AddComponent( CreateComponent<Text>() );
    Text* textElement = overlayObject->GetComponent<Text>();
    
    textElement->text  = text;
    textElement->color = color;
    textElement->size = textSize;
    
    overlayObject->mTransformCache->scale = Vector3(textSize, 1, textSize);
    
    textElement->canvas.x = x / textSize;
    textElement->canvas.y = y / textSize;
    
    MeshRenderer* overlayRenderer = overlayObject->GetComponent<MeshRenderer>();
    
    // Sprite sheet material
    Destroy<Material>( overlayRenderer->material );
    overlayRenderer->material = Resources.CreateMaterialFromTag( materialTag );
    
    overlayRenderer->material->ambient = Color(0.58f, 1, 0);
    overlayRenderer->material->shader = shaders.UI;
    
    overlayRenderer->material->SetBlending(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);
    overlayRenderer->material->EnableBlending();
    
    overlayRenderer->material->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    
    overlayRenderer->material->DisableCulling();
    overlayRenderer->material->DisableShadowVolumePass();
    
    return overlayObject;
}

GameObject* EngineSystemManager::CreateOverlayPanelRenderer(float x, float y, float width, float height, std::string materialTag) {
    
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
    
    overlayMesh->Load();
    
    return overlayObject;
}


Button* EngineSystemManager::CreateButtonUI(float buttonX, float buttonY, float buttonW, float buttonH, float textX, float textY, std::string text, std::string materialName, ButtonCallBack callback) {
    
    Button* buttonUI = CreateOverlayButtonCallback(buttonX, buttonY, buttonW, buttonH, callback);
    
    GameObject* panelOverlay = CreateOverlayPanelRenderer(buttonX, buttonY, buttonW / 2.0f, buttonH / 2.0f, materialName);
    buttonUI->panelOverlay = panelOverlay;
    
    MeshRenderer* buttonRenderer = panelOverlay->GetComponent<MeshRenderer>();
    sceneOverlay->AddMeshRendererToSceneRoot(buttonRenderer, RENDER_QUEUE_GEOMETRY);
    
    Panel* panel = panelOverlay->GetComponent<Panel>();
    panel->x = buttonW / 2;
    panel->y = (buttonH / 2) + 16;
    
    buttonUI->panel = panel;
    
    // Button text
    GameObject* textObject = CreateOverlayTextRenderer(buttonX, buttonY, "", 9, Colors.white, "font");
    buttonUI->textObject = textObject;
    
    MeshRenderer* buttonTextRenderer = textObject->GetComponent<MeshRenderer>();
    sceneOverlay->AddMeshRendererToSceneRoot(buttonTextRenderer, RENDER_QUEUE_POSTGEOMETRY);
    
    Text* textPtr = textObject->GetComponent<Text>();
    textPtr->text = text;
    
    textPtr->canvas.x = (buttonX / textPtr->size) + textX;
    textPtr->canvas.y = (buttonY / textPtr->size) - textY;
    
    textPtr->x = textX;
    textPtr->y = textY;
    
    return buttonUI;
}


