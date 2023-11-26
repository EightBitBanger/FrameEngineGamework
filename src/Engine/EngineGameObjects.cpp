// Engine game object related functions
//

#include "engine.h"

__declspec(dllexport) extern EngineComponents  Components;
__declspec(dllexport) extern ColorPreset       Colors;
__declspec(dllexport) extern RandomGen         Random;
__declspec(dllexport) extern Logger            Log;
__declspec(dllexport) extern Timer             PhysicsTime;
__declspec(dllexport) extern Timer             Time;

__declspec(dllexport) extern Serialization     Serializer;
__declspec(dllexport) extern ResourceManager   Resources;
__declspec(dllexport) extern ScriptSystem      Scripting;
__declspec(dllexport) extern RenderSystem      Renderer;
__declspec(dllexport) extern PhysicsSystem     Physics;
__declspec(dllexport) extern AudioSystem       Audio;
__declspec(dllexport) extern InputSystem       Input;
__declspec(dllexport) extern MathCore          Math;
__declspec(dllexport) extern ActorSystem       AI;

__declspec(dllexport) extern ApplicationLayer      Application;
__declspec(dllexport) extern EngineSystemManager   Engine;



GameObject* EngineSystemManager::CreateGameObject(void) {
    GameObject* newGameObject = mGameObjects.Create();
    mGameObjectActive.push_back(newGameObject);
    doUpdateDataStream = true;
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
        
        Component* componentPtr = gameObjectPtr->GetComponentIndex(i);
        DestroyComponent(componentPtr);
        continue;
    }
    
    mGameObjects.Destroy(gameObjectPtr);
    
    doUpdateDataStream = true;
    return true;
}

GameObject* EngineSystemManager::CreateCameraController(glm::vec3 position, glm::vec3 scale) {
    
    GameObject* cameraController = CreateGameObject();
    cameraController->name = "camera";
    cameraController->transform.position = position;
    
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
    
    // Collider
    BoxShape* boxShape = Physics.CreateColliderBox(scale.x, scale.y, scale.z);
    cameraController->AddColliderBox(boxShape, 0, 0, 0);
    
    doUpdateDataStream = true;
    return cameraController;
}

GameObject* EngineSystemManager::CreateSky(std::string meshTagName, Color colorLow, Color colorHigh, float biasMul) {
    
    Mesh* skyMesh = Resources.CreateMeshFromTag(meshTagName);
    if (skyMesh == nullptr) return nullptr;
    
    Material* skyMaterial = Renderer.CreateMaterial();
    
    skyMaterial->diffuse = Color(1, 1, 1);
    skyMaterial->DisableDepthTest();
    skyMaterial->ambient = Colors.MakeGrayScale(0.4);
    skyMaterial->diffuse = Colors.MakeGrayScale(0.4);
    skyMaterial->shader = shaders.color;
    
    for (unsigned int i=0; i < skyMesh->GetNumberOfVertices(); i++) {
        Vertex vertex = skyMesh->GetVertex(i);
        
        if (vertex.y > 0) {
            vertex.r = Math.Lerp(colorHigh.r, colorLow.r, vertex.y * biasMul);
            vertex.g = Math.Lerp(colorHigh.g, colorLow.g, vertex.y * biasMul);
            vertex.b = Math.Lerp(colorHigh.b, colorLow.b, vertex.y * biasMul);
        } else {
            vertex.r = Math.Lerp(colorLow.r, colorHigh.r, vertex.y * biasMul);
            vertex.g = Math.Lerp(colorLow.g, colorHigh.g, vertex.y * biasMul);
            vertex.b = Math.Lerp(colorLow.b, colorHigh.b, vertex.y * biasMul);
        }
        
        skyMesh->SetVertex(i, vertex);
    }
    skyMesh->UploadToGPU();
    
    GameObject* skyObject = CreateGameObject();
    skyObject->name = "sky";
    skyObject->AddComponent( CreateComponentMeshRenderer(skyMesh, skyMaterial) );
    
    skyObject->transform.SetScale(10000, 2000, 10000);
    
    doUpdateDataStream = true;
    return skyObject;
}

GameObject* EngineSystemManager::CreateAIActor(glm::vec3 position) {
    
    GameObject* newGameObject = CreateGameObject();
    newGameObject->AddComponent( CreateComponent(Components.Actor) );
    newGameObject->AddComponent( CreateComponent(Components.RigidBody) );
    newGameObject->AddComponent( CreateComponent(Components.MeshRenderer) );
    
    // Basic cube mesh
    Mesh* meshPtr = Resources.CreateMeshFromTag("cube");
    Material* materialPtr = Renderer.CreateMaterial();
    
    materialPtr->shader = shaders.color;
    
    // Mesh renderer component
    MeshRenderer* entityRenderer = newGameObject->mMeshRendererCache;
    
    entityRenderer->mesh = meshPtr;
    entityRenderer->material = materialPtr;
    
    if (sceneMain != nullptr) 
        sceneMain->AddMeshRendererToSceneRoot( entityRenderer );
    
    float scale = 1.0;
    
    // Collider
    BoxShape* boxShape = Physics.CreateColliderBox(scale, scale, scale);
    newGameObject->AddColliderBox(boxShape, 0, 0, 0);
    newGameObject->EnableGravity();
    
    // Physics
    newGameObject->SetMass(10);
    newGameObject->SetLinearDamping(3);
    newGameObject->SetAngularDamping(1);
    
    //newGameObject->CalculatePhysics();
    
    newGameObject->SetLinearAxisLockFactor(1, 1, 1);
    newGameObject->SetAngularAxisLockFactor(0, 1, 0);
    
    newGameObject->transform.SetScale(scale, scale, scale);
    newGameObject->SetPosition(position);
    
    doUpdateDataStream = true;
    return newGameObject;
}

GameObject* EngineSystemManager::CreateOverlayRenderer(void) {
    GameObject* overlayObject = Create<GameObject>();
    
    overlayObject->transform.RotateAxis(-180, Vector3(0, 1, 0));
    overlayObject->transform.RotateAxis( -90, Vector3(0, 0, 1));
    
    Mesh*     overlayMesh     = Create<Mesh>();
    Material* overlayMaterial = Create<Material>();
    
    overlayMaterial->shader = shaders.UI;
    overlayMaterial->ambient = Colors.black;
    
    overlayMaterial->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    overlayMaterial->SetTextureFiltration(MATERIAL_FILTER_NONE);
    overlayMaterial->DisableCulling();
    
    overlayObject->AddComponent( CreateComponent<MeshRenderer>(overlayMesh, overlayMaterial) );
    
    doUpdateDataStream = true;
    return overlayObject;
}

GameObject* EngineSystemManager::CreateOverlayTextRenderer(std::string text, unsigned int textSize, Color color, std::string materialTag) {
    
    GameObject* overlayObject = CreateOverlayRenderer();
    overlayObject->AddComponent( CreateComponent<Text>() );
    Text* textElement = overlayObject->GetComponent<Text>();
    
    textElement->text  = text;
    textElement->color = color;
    textElement->size = textSize;
    
    overlayObject->transform.scale = Vector3(textSize, 1, textSize);
    
    MeshRenderer* overlayRenderer = overlayObject->GetComponent<MeshRenderer>();
    
    // Sprite sheet material
    Destroy<Material>( overlayRenderer->material );
    overlayRenderer->material = Resources.CreateMaterialFromTag( materialTag );
    overlayRenderer->material->ambient  = Colors.black;
    overlayRenderer->material->shader = shaders.UI;
    
    overlayRenderer->material->SetBlending(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);
    overlayRenderer->material->EnableBlending();
    
    overlayRenderer->material->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    overlayRenderer->material->SetTextureFiltration(MATERIAL_FILTER_NONE);
    
    overlayRenderer->material->DisableCulling();
    
    doUpdateDataStream = true;
    return overlayObject;
}

GameObject* EngineSystemManager::CreateOverlayPanelRenderer(unsigned int scaleWidth, unsigned int scaleHeight, std::string materialTag) {
    
    GameObject* overlayObject = CreateOverlayRenderer();
    overlayObject->AddComponent( CreateComponent<Panel>() );
    
    MeshRenderer* overlayRenderer = overlayObject->GetComponent<MeshRenderer>();
    
    // Sprite base material
    Material* overlayMaterial = overlayRenderer->material;
    Mesh*     overlayMesh     = overlayRenderer->mesh;
    
    Destroy<Material>( overlayMaterial );
    overlayMaterial = Resources.CreateMaterialFromTag( materialTag );
    
    overlayMaterial->ambient  = Colors.black;
    overlayMaterial->shader = shaders.texture;
    
    overlayMaterial->SetBlending(BLEND_ONE, BLEND_ONE_MINUS_SRC_ALPHA);
    overlayMaterial->EnableBlending();
    
    overlayMaterial->SetDepthFunction(MATERIAL_DEPTH_ALWAYS);
    overlayMaterial->SetTextureFiltration(MATERIAL_FILTER_NONE);
    
    overlayMaterial->DisableCulling();
    
    overlayMesh->AddPlain(0, 0, 0, scaleWidth, scaleHeight, Colors.white, 1, 1);
    overlayMesh->UploadToGPU();
    
    doUpdateDataStream = true;
    return overlayObject;
}

GameObject* EngineSystemManager::CreateOverlayButtonRenderer(unsigned int scaleWidth, unsigned int scaleHeight, std::string materialTag) {
    assert(0); // This function doesnt work
    
    GameObject* panelObject = CreateOverlayPanelRenderer(10, 10, "panel");
    GameObject* textObject  = CreateOverlayTextRenderer("Button", 0.9, Colors.black, "font");
    
    textObject->parent = panelObject;
    
    return textObject;
}
